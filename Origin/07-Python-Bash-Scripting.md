# 07 — Python & Bash Scripting for DevOps

---

## SECTION 1 — Python Patterns

### Pattern 1: Run shell commands from Python

```python
import subprocess
import sys

def run(cmd: list[str], check: bool = True) -> subprocess.CompletedProcess:
    """Run a command, raise on failure, return result."""
    result = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        check=check   # raises CalledProcessError on non-zero exit code
    )
    return result

# Usage
result = run(["docker", "ps", "--format", "{{.Names}}"])
containers = result.stdout.strip().splitlines()
print(containers)

# Stream output in real-time (no capture)
subprocess.run(["kubectl", "rollout", "status", "deployment/myapp"], check=True)

# Get exit code without raising
result = run(["kubectl", "get", "pod", "mypod"], check=False)
if result.returncode != 0:
    print(f"Pod not found: {result.stderr}")
```

---

### Pattern 2: Config from environment variables

```python
import os
from dataclasses import dataclass

@dataclass
class Config:
    db_host: str
    db_port: int
    aws_region: str
    debug: bool
    log_level: str

    @classmethod
    def from_env(cls) -> "Config":
        return cls(
            db_host=os.environ.get("DB_HOST", "localhost"),
            db_port=int(os.environ.get("DB_PORT", "5432")),
            aws_region=os.environ.get("AWS_REGION", "us-east-1"),
            debug=os.environ.get("DEBUG", "false").lower() == "true",
            log_level=os.environ.get("LOG_LEVEL", "INFO"),
        )

def require_env(name: str) -> str:
    """Get a required env var — raise clearly if missing."""
    value = os.environ.get(name)
    if not value:
        raise EnvironmentError(f"Required environment variable not set: {name}")
    return value

# Usage
config = Config.from_env()
db_password = require_env("DB_PASSWORD")
```

---

### Pattern 3: AWS SQS worker with boto3

```python
import boto3
import json
import logging
import os

logger = logging.getLogger(__name__)
sqs = boto3.client("sqs", region_name="us-east-1")
QUEUE_URL = os.environ["SQS_QUEUE_URL"]

def send_message(body: dict) -> str:
    response = sqs.send_message(
        QueueUrl=QUEUE_URL,
        MessageBody=json.dumps(body)
    )
    return response["MessageId"]

def send_batch(messages: list[dict]):
    entries = [
        {"Id": str(i), "MessageBody": json.dumps(msg)}
        for i, msg in enumerate(messages)
    ]
    response = sqs.send_message_batch(QueueUrl=QUEUE_URL, Entries=entries)
    if response.get("Failed"):
        logger.error(f"Failed to send: {response['Failed']}")

def receive_messages(max_count: int = 10) -> list:
    response = sqs.receive_message(
        QueueUrl=QUEUE_URL,
        MaxNumberOfMessages=max_count,
        WaitTimeSeconds=20,        # LONG POLLING — always use this
        VisibilityTimeout=60,      # set >= your max processing time
        MessageAttributeNames=["All"]
    )
    return response.get("Messages", [])

def delete_message(receipt_handle: str):
    sqs.delete_message(QueueUrl=QUEUE_URL, ReceiptHandle=receipt_handle)

def process(data: dict):
    logger.info(f"Processing: {data}")
    # ... business logic here

def run_worker():
    logger.info("SQS worker started, polling...")
    while True:
        messages = receive_messages()
        for msg in messages:
            try:
                data = json.loads(msg["Body"])
                process(data)
                delete_message(msg["ReceiptHandle"])   # DELETE only on success
            except Exception as e:
                logger.error(f"Failed to process message: {e}")
                # DO NOT delete — reappears after VisibilityTimeout
                # After maxReceiveCount failures -> moves to DLQ automatically
```

---

### Pattern 4: S3 operations

```python
import boto3
from pathlib import Path

s3 = boto3.client("s3")

def upload_file(local_path: str, bucket: str, key: str):
    s3.upload_file(local_path, bucket, key)
    print(f"Uploaded: s3://{bucket}/{key}")

def download_file(bucket: str, key: str, dest: str):
    Path(dest).parent.mkdir(parents=True, exist_ok=True)
    s3.download_file(bucket, key, dest)

def list_objects(bucket: str, prefix: str = "") -> list[str]:
    """List all keys — handles pagination automatically."""
    paginator = s3.get_paginator("list_objects_v2")
    keys = []
    for page in paginator.paginate(Bucket=bucket, Prefix=prefix):
        for obj in page.get("Contents", []):
            keys.append(obj["Key"])
    return keys

def presigned_url(bucket: str, key: str, expires: int = 3600) -> str:
    return s3.generate_presigned_url(
        "get_object",
        Params={"Bucket": bucket, "Key": key},
        ExpiresIn=expires
    )

def bucket_exists(bucket: str) -> bool:
    try:
        s3.head_bucket(Bucket=bucket)
        return True
    except s3.exceptions.ClientError:
        return False
```

---

### Pattern 5: HTTP health checker

```python
import requests
import sys
import logging
import time

logging.basicConfig(level=logging.INFO, format="%(asctime)s %(levelname)s %(message)s")
logger = logging.getLogger(__name__)

SERVICES = {
    "api":    "http://api:8080/health",
    "auth":   "http://auth:8081/health",
    "worker": "http://worker:8082/health",
}

def check(name: str, url: str, timeout: int = 5) -> bool:
    try:
        r = requests.get(url, timeout=timeout)
        if r.status_code == 200:
            logger.info(f"[OK]   {name}")
            return True
        logger.warning(f"[WARN] {name} HTTP {r.status_code}")
        return False
    except requests.exceptions.RequestException as e:
        logger.error(f"[FAIL] {name}: {e}")
        return False

def wait_for_service(name: str, url: str, max_attempts: int = 30, delay: int = 2):
    """Block until a service is healthy or timeout."""
    for attempt in range(1, max_attempts + 1):
        if check(name, url):
            return
        logger.info(f"  {name}: attempt {attempt}/{max_attempts}, retrying in {delay}s...")
        time.sleep(delay)
    raise TimeoutError(f"Service '{name}' not healthy after {max_attempts} attempts")

def main():
    failed = [name for name, url in SERVICES.items() if not check(name, url)]
    if failed:
        logger.error(f"Unhealthy services: {failed}")
        sys.exit(1)

if __name__ == "__main__":
    main()
```

---

### Pattern 6: Retry decorator with exponential backoff

```python
import time
import functools
import logging
from typing import Callable, Any, Type

logger = logging.getLogger(__name__)

def retry(
    max_attempts: int = 3,
    delay: float = 1.0,
    backoff: float = 2.0,
    exceptions: tuple[Type[Exception], ...] = (Exception,)
):
    """Retry with exponential backoff."""
    def decorator(func: Callable) -> Callable:
        @functools.wraps(func)
        def wrapper(*args, **kwargs) -> Any:
            current_delay = delay
            for attempt in range(1, max_attempts + 1):
                try:
                    return func(*args, **kwargs)
                except exceptions as e:
                    if attempt == max_attempts:
                        raise
                    logger.warning(
                        f"{func.__name__}: attempt {attempt}/{max_attempts} failed: {e}. "
                        f"Retrying in {current_delay:.1f}s..."
                    )
                    time.sleep(current_delay)
                    current_delay *= backoff
        return wrapper
    return decorator

# Usage
@retry(max_attempts=3, delay=2.0, backoff=2.0, exceptions=(requests.exceptions.RequestException,))
def call_external_api(url: str) -> dict:
    response = requests.get(url, timeout=5)
    response.raise_for_status()
    return response.json()
```

---

### Pattern 7: Read and write YAML / JSON config files

```python
import json
import yaml          # pip install pyyaml
from pathlib import Path

# JSON
def read_json(path: str) -> dict:
    return json.loads(Path(path).read_text())

def write_json(data: dict, path: str, pretty: bool = True):
    Path(path).write_text(json.dumps(data, indent=2 if pretty else None))

# YAML
def read_yaml(path: str) -> dict:
    with open(path) as f:
        return yaml.safe_load(f)    # safe_load prevents code execution

def write_yaml(data: dict, path: str):
    with open(path, "w") as f:
        yaml.dump(data, f, default_flow_style=False, sort_keys=False)

# Example: patch a Kubernetes manifest image tag
def update_image_tag(manifest_path: str, new_tag: str):
    manifest = read_yaml(manifest_path)
    containers = manifest["spec"]["template"]["spec"]["containers"]
    for container in containers:
        image_name = container["image"].split(":")[0]
        container["image"] = f"{image_name}:{new_tag}"
    write_yaml(manifest, manifest_path)
    print(f"Updated {manifest_path} with tag: {new_tag}")
```

---

### Pattern 8: Context manager for temporary files / directories

```python
import tempfile
import os
import shutil
from contextlib import contextmanager

@contextmanager
def temp_dir():
    """Create a temporary directory that is cleaned up on exit."""
    tmpdir = tempfile.mkdtemp()
    try:
        yield tmpdir
    finally:
        shutil.rmtree(tmpdir, ignore_errors=True)

@contextmanager
def working_directory(path: str):
    """Temporarily change the working directory."""
    original = os.getcwd()
    try:
        os.chdir(path)
        yield
    finally:
        os.chdir(original)

# Usage
with temp_dir() as tmpdir:
    config_path = os.path.join(tmpdir, "kubeconfig")
    with open(config_path, "w") as f:
        f.write(os.environ["KUBECONFIG_DATA"])
    subprocess.run(["kubectl", "get", "pods"], env={**os.environ, "KUBECONFIG": config_path})
# tmpdir is automatically deleted here
```

---

### Pattern 9: Structured logging

```python
import logging
import json
import sys
from datetime import datetime

class JSONFormatter(logging.Formatter):
    """Emit logs as JSON — easier to parse in CloudWatch / ELK / Loki."""
    def format(self, record: logging.LogRecord) -> str:
        log_entry = {
            "timestamp": datetime.utcfromtimestamp(record.created).isoformat() + "Z",
            "level": record.levelname,
            "logger": record.name,
            "message": record.getMessage(),
            "module": record.module,
            "function": record.funcName,
            "line": record.lineno,
        }
        if record.exc_info:
            log_entry["exception"] = self.formatException(record.exc_info)
        return json.dumps(log_entry)

def setup_logging(level: str = "INFO"):
    handler = logging.StreamHandler(sys.stdout)
    handler.setFormatter(JSONFormatter())
    logging.basicConfig(level=level, handlers=[handler])

# Usage
setup_logging()
logger = logging.getLogger("myapp")
logger.info("Worker started", extra={"queue": "orders", "region": "us-east-1"})
```

---

## SECTION 2 — Python Q&A

**Q: How do you handle exceptions properly in Python for DevOps scripts?**

```python
import subprocess
import logging

logger = logging.getLogger(__name__)

def deploy(image_tag: str) -> bool:
    try:
        subprocess.run(
            ["kubectl", "set", "image", "deployment/app", f"app={image_tag}"],
            capture_output=True, text=True, check=True
        )
        return True
    except subprocess.CalledProcessError as e:
        logger.error(f"kubectl failed (exit {e.returncode}): {e.stderr.strip()}")
        return False
    except FileNotFoundError:
        logger.error("kubectl not found in PATH")
        raise   # re-raise — this is a configuration error, not a runtime error
    finally:
        logger.info("Deploy attempt finished")
```

---

**Q: What is the difference between `os.system`, `os.popen`, and `subprocess`?**

```
os.system(cmd)       — runs command in a shell, returns exit code only.
                       No access to stdout/stderr. Insecure with user input.
                       Deprecated for new code.

os.popen(cmd)        — runs command in a shell, returns a file-like object for stdout.
                       No stderr. No exit code. Deprecated.

subprocess (correct) — full control: stdout, stderr, exit code, timeout, env.
                       Can avoid the shell entirely (exec form) — safer, no shell injection.
                       subprocess.run() for simple cases.
                       subprocess.Popen() for streaming, complex I/O.
```

---

**Q: How do you run a Python script that needs to work both in Python 3.10 and 3.12?**

```python
# Use conditional imports for new features
import sys

if sys.version_info >= (3, 11):
    import tomllib                     # stdlib in 3.11+
else:
    import tomli as tomllib            # pip install tomli

# Use | for union types only in 3.10+
# For 3.9 compatibility: use Optional[str] or Union[str, None] from typing
from typing import Optional

def get_name() -> Optional[str]:       # compatible with 3.9+
    return None
```

---

**Q: What is the GIL (Global Interpreter Lock)?**

A: The GIL is a mutex in CPython that allows only ONE thread to execute Python bytecode at a
time. This means Python threads do NOT provide true parallelism for CPU-bound tasks.

```
CPU-bound tasks (heavy computation):
  - Use multiprocessing.Pool — each process has its own GIL.
  - Use concurrent.futures.ProcessPoolExecutor.

I/O-bound tasks (network calls, disk, DB queries):
  - GIL is released during I/O operations.
  - threading or asyncio work well here.
  - Use concurrent.futures.ThreadPoolExecutor for parallel HTTP calls.
```

---

**Q: How do you make parallel HTTP requests efficiently?**

```python
import asyncio
import aiohttp

async def fetch(session: aiohttp.ClientSession, url: str) -> dict:
    async with session.get(url) as response:
        return {"url": url, "status": response.status, "body": await response.json()}

async def check_all(urls: list[str]) -> list[dict]:
    async with aiohttp.ClientSession() as session:
        tasks = [fetch(session, url) for url in urls]
        return await asyncio.gather(*tasks, return_exceptions=True)

# Usage
urls = ["http://api:8080/health", "http://auth:8081/health", "http://worker:8082/health"]
results = asyncio.run(check_all(urls))

# Alternative: ThreadPoolExecutor for sync requests
from concurrent.futures import ThreadPoolExecutor
import requests

def check(url: str) -> dict:
    r = requests.get(url, timeout=5)
    return {"url": url, "status": r.status_code}

with ThreadPoolExecutor(max_workers=10) as executor:
    results = list(executor.map(check, urls))
```

---

## SECTION 3 — Bash Scripting Patterns

### Deployment script with full safety checks

```bash
#!/bin/bash
set -euo pipefail

# ── Configuration ─────────────────────────────────────────────────────────────
APP_NAME="${APP_NAME:?APP_NAME is required}"
ENVIRONMENT="${ENVIRONMENT:-staging}"
IMAGE_TAG="${IMAGE_TAG:?IMAGE_TAG is required}"
REGISTRY="${REGISTRY:-registry.company.com}"
NAMESPACE="${NAMESPACE:-${ENVIRONMENT}}"
TIMEOUT="${TIMEOUT:-5m}"

# ── Helpers ───────────────────────────────────────────────────────────────────
log()  { echo "[$(date '+%Y-%m-%d %H:%M:%S')] $*"; }
die()  { log "ERROR: $*" >&2; exit 1; }
ok()   { log "OK: $*"; }

# ── Pre-flight checks ─────────────────────────────────────────────────────────
command -v kubectl >/dev/null 2>&1 || die "kubectl not found in PATH"
command -v docker  >/dev/null 2>&1 || die "docker not found in PATH"

log "Deploying ${APP_NAME}:${IMAGE_TAG} to ${ENVIRONMENT} (namespace: ${NAMESPACE})"

# ── Deploy ────────────────────────────────────────────────────────────────────
log "Updating image..."
kubectl set image deployment/"${APP_NAME}" \
    "${APP_NAME}=${REGISTRY}/${APP_NAME}:${IMAGE_TAG}" \
    --namespace "${NAMESPACE}"

log "Waiting for rollout (timeout: ${TIMEOUT})..."
if kubectl rollout status deployment/"${APP_NAME}" \
    --namespace "${NAMESPACE}" \
    --timeout "${TIMEOUT}"; then
    ok "Rollout complete."
else
    log "Rollout failed. Rolling back..."
    kubectl rollout undo deployment/"${APP_NAME}" --namespace "${NAMESPACE}"
    die "Rollout failed and was rolled back."
fi

log "Current pods:"
kubectl get pods --namespace "${NAMESPACE}" -l "app=${APP_NAME}" --no-headers
```

---

### Cleanup old Docker images — keep latest N

```bash
#!/bin/bash
set -euo pipefail

REPO="${1:?Usage: $0 <repository> [keep_count]}"
KEEP="${2:-5}"

log() { echo "[$(date +%H:%M:%S)] $*"; }

# Get all image IDs for this repo, sorted oldest first
mapfile -t IMAGE_IDS < <(
    docker images --format "{{.ID}}\t{{.CreatedAt}}" "$REPO" \
    | sort -k2 \
    | awk '{print $1}'
)

TOTAL=${#IMAGE_IDS[@]}
TO_DELETE=$(( TOTAL - KEEP ))

if [ "$TO_DELETE" -le 0 ]; then
    log "Only $TOTAL images found for '$REPO'. Nothing to delete."
    exit 0
fi

log "Found $TOTAL images. Deleting oldest $TO_DELETE, keeping $KEEP..."

for (( i=0; i<TO_DELETE; i++ )); do
    log "  Removing: ${IMAGE_IDS[$i]}"
    docker rmi "${IMAGE_IDS[$i]}" 2>/dev/null || log "  Warning: could not remove ${IMAGE_IDS[$i]}"
done

log "Done. Remaining images:"
docker images "$REPO"
```

---

### Wait for TCP port to be ready (used in entrypoint scripts)

```bash
#!/bin/bash
set -euo pipefail

wait_for_port() {
    local host="$1"
    local port="$2"
    local max_attempts="${3:-30}"
    local delay="${4:-2}"
    local attempt=0

    echo "Waiting for ${host}:${port}..."
    until nc -z "$host" "$port" 2>/dev/null; do
        attempt=$(( attempt + 1 ))
        if [ "$attempt" -ge "$max_attempts" ]; then
            echo "ERROR: ${host}:${port} not ready after ${max_attempts} attempts (${delay}s each)"
            exit 1
        fi
        echo "  Attempt ${attempt}/${max_attempts} — sleeping ${delay}s..."
        sleep "$delay"
    done
    echo "${host}:${port} is ready! (after ${attempt} attempts)"
}

# Wait for all dependencies before starting the app
wait_for_port "${DB_HOST:-localhost}"    "${DB_PORT:-5432}"
wait_for_port "${REDIS_HOST:-localhost}" "${REDIS_PORT:-6379}"

# exec replaces the shell with the app process (making it PID 1)
exec "$@"
```

---

### Parse arguments with getopts

```bash
#!/bin/bash
set -euo pipefail

usage() {
    cat <<EOF
Usage: $0 [OPTIONS]

Options:
  -e, --env ENV       Target environment (default: staging)
  -t, --tag TAG       Docker image tag (required)
  -n, --namespace NS  Kubernetes namespace (default: ENV value)
  -d, --dry-run       Print commands without executing
  -h, --help          Show this help

Examples:
  $0 -e production -t v1.2.3
  $0 --env staging --tag abc1234 --dry-run
EOF
    exit "${1:-0}"
}

ENVIRONMENT="staging"
IMAGE_TAG=""
NAMESPACE=""
DRY_RUN=false

while [[ $# -gt 0 ]]; do
    case "$1" in
        -e|--env)       ENVIRONMENT="$2"; shift 2 ;;
        -t|--tag)       IMAGE_TAG="$2";   shift 2 ;;
        -n|--namespace) NAMESPACE="$2";   shift 2 ;;
        -d|--dry-run)   DRY_RUN=true;     shift   ;;
        -h|--help)      usage 0 ;;
        *) echo "Unknown option: $1"; usage 1 ;;
    esac
done

[ -z "$IMAGE_TAG" ] && { echo "Error: --tag is required"; usage 1; }
NAMESPACE="${NAMESPACE:-${ENVIRONMENT}}"

run() {
    if $DRY_RUN; then
        echo "[DRY-RUN] $*"
    else
        "$@"
    fi
}

run kubectl set image deployment/myapp myapp="registry.company.com/myapp:${IMAGE_TAG}" \
    -n "${NAMESPACE}"
```

---

### Backup script with timestamped archives

```bash
#!/bin/bash
set -euo pipefail

BACKUP_DIR="${BACKUP_DIR:-/backups}"
SOURCE_DIR="${1:?Usage: $0 <source_dir>}"
RETENTION_DAYS="${RETENTION_DAYS:-7}"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
HOSTNAME_SHORT=$(hostname -s)
ARCHIVE_NAME="${BACKUP_DIR}/${HOSTNAME_SHORT}_$(basename "${SOURCE_DIR}")_${TIMESTAMP}.tar.gz"

log() { echo "[$(date '+%Y-%m-%d %H:%M:%S')] $*"; }

[ -d "$SOURCE_DIR" ] || { log "Source not found: $SOURCE_DIR"; exit 1; }
mkdir -p "$BACKUP_DIR"

log "Backing up: $SOURCE_DIR -> $ARCHIVE_NAME"
tar -czf "$ARCHIVE_NAME" -C "$(dirname "$SOURCE_DIR")" "$(basename "$SOURCE_DIR")"
log "Backup complete: $(du -sh "$ARCHIVE_NAME" | cut -f1)"

# Remove backups older than RETENTION_DAYS
log "Removing backups older than ${RETENTION_DAYS} days..."
find "$BACKUP_DIR" -name "*.tar.gz" -mtime "+${RETENTION_DAYS}" -delete
log "Cleanup complete."

log "Current backups:"
ls -lh "$BACKUP_DIR"/*.tar.gz 2>/dev/null || log "(none)"
```

---

### Cron patterns — common schedules

```bash
# Crontab format: minute hour day-of-month month day-of-week command
#                 *      *    *             *     *

# Every day at 2:00 AM
0 2 * * * /opt/scripts/backup.sh >> /var/log/backup.log 2>&1

# Every 5 minutes
*/5 * * * * /opt/scripts/health-check.sh

# Every Monday at midnight
0 0 * * 1 /opt/scripts/weekly-report.sh

# First day of every month at 3:30 AM
30 3 1 * * /opt/scripts/monthly-cleanup.sh

# Every weekday at 8 AM
0 8 * * 1-5 /opt/scripts/notify-team.sh

# Every 15 minutes during business hours (9-5, Mon-Fri)
*/15 9-17 * * 1-5 /opt/scripts/check-services.sh
```

---

## SECTION 4 — Bash Q&A

**Q: How do you trap signals and clean up in a Bash script?**

```bash
#!/bin/bash
set -euo pipefail

TMPFILE=$(mktemp)
LOCK_FILE="/tmp/myscript.lock"

cleanup() {
    local exit_code=$?
    echo "Cleaning up..."
    rm -f "$TMPFILE" "$LOCK_FILE"
    exit $exit_code
}

# Register cleanup for EXIT (runs on normal exit, errors, and signals)
trap cleanup EXIT
trap 'echo "Interrupted"; exit 130' INT TERM

# Prevent concurrent execution
if [ -f "$LOCK_FILE" ]; then
    echo "Another instance is running (lock: $LOCK_FILE)"
    exit 1
fi
touch "$LOCK_FILE"

echo "Running..."
# ... your script work here
```

---

**Q: What is the difference between `$()` and backticks?**

```bash
# Both capture command output — functionally equivalent
result=$(date +%Y-%m-%d)
result=`date +%Y-%m-%d`   # older syntax, avoid

# Why prefer $() over backticks:
# 1. Easier to nest: $(date $(another_command))  vs  `date \`another_command\``
# 2. Easier to read
# 3. Consistent with arithmetic $(( ))
```

---

**Q: How do you process each line of a file or command output?**

```bash
# Process lines from a file
while IFS= read -r line; do
    echo "Processing: $line"
done < /etc/hosts

# Process output of a command
while IFS= read -r pod; do
    echo "Restarting: $pod"
    kubectl delete pod "$pod"
done < <(kubectl get pods -o name | grep "Evicted")

# IFS=  — prevents stripping leading/trailing whitespace
# -r    — prevents backslash interpretation
# < <() — process substitution — avoids subshell (changes to vars are visible in parent)
```

---

**Q: What is the difference between `[ -z "$VAR" ]` and `[ -z $VAR ]`?**

```bash
VAR=""

# Unsafe — if VAR is empty, expands to [ -z ], which is a syntax error
if [ -z $VAR ]; then echo "empty"; fi

# Safe — always quote your variables in test expressions
if [ -z "$VAR" ]; then echo "empty"; fi

# Even safer — use [[ ]] which handles unquoted empty variables
if [[ -z $VAR ]]; then echo "empty"; fi
```

---

**Q: How do you check if a command exists before using it?**

```bash
# Method 1: command -v (POSIX, recommended)
if ! command -v kubectl >/dev/null 2>&1; then
    echo "kubectl is not installed" >&2
    exit 1
fi

# Method 2: which (less portable, not always available)
if ! which docker >/dev/null 2>&1; then
    echo "docker not found"
fi

# Method 3: type (bash built-in)
if ! type terraform &>/dev/null; then
    echo "terraform not installed"
fi
```

---

**Q: How do you do arithmetic in Bash?**

```bash
# Arithmetic expansion (integer only)
count=$((count + 1))
total=$(( 10 * 5 ))
remainder=$(( 17 % 3 ))

# let command
let "count += 1"

# Floating point — Bash does not support it, use bc or awk
result=$(echo "scale=2; 10 / 3" | bc)         # 3.33
result=$(awk 'BEGIN { printf "%.2f\n", 10/3 }')  # 3.33

# Check if a number is within range
if (( count > 100 )); then
    echo "Too many"
fi
```

---

## SECTION 5 — Cron & Scheduled Tasks

**Q: How do you schedule a task in Linux?**

```bash
# User crontab — for current user
crontab -e          # edit
crontab -l          # list
crontab -r          # remove all

# System crontab
/etc/crontab                    # system-wide crontab (has user column)
/etc/cron.d/                    # drop-in cron files
/etc/cron.daily/                # scripts run daily
/etc/cron.hourly/               # scripts run hourly

# Run a script every 10 minutes as the 'ubuntu' user
# /etc/cron.d/myjob
*/10 * * * *   ubuntu   /opt/scripts/monitor.sh >> /var/log/monitor.log 2>&1

# Systemd timer (modern alternative to cron)
# /etc/systemd/system/myapp.timer
[Unit]
Description=Run myapp cleanup every hour

[Timer]
OnCalendar=hourly
Persistent=true

[Install]
WantedBy=timers.target
```

---

**Q: How do you ensure a cron job does not run concurrently?**

```bash
#!/bin/bash
# Use flock to prevent concurrent execution
exec 9>/tmp/myjob.lock
if ! flock -n 9; then
    echo "$(date): Another instance is running, exiting." >&2
    exit 1
fi

# Your job code here...
echo "$(date): Job started"
sleep 30
echo "$(date): Job completed"

# Lock is released automatically when the script exits
```
