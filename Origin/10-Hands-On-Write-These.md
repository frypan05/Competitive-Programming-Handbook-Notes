# 10 — Hands-On: Write These From Scratch

The most important file in this entire guide.
Close all notes. Set a timer. Write it. Then compare.
Do each exercise until you can write it in under 5 minutes without looking.

---

## HOW TO USE THIS FILE

1. Read the requirement carefully.
2. Close all other files.
3. Open a blank editor.
4. Write it from memory.
5. Compare with the answer below.
6. Repeat until fluent.

---

# PART 1 — DOCKERFILES

---

## Exercise 1.1 — Python FastAPI App (Production-Grade)

**Requirements:**
- Multi-stage build (builder + production)
- Copy dependency files before source code (cache optimisation)
- Non-root user
- Exec form ENTRYPOINT
- Minimal final image using python:3.12-slim

**Write it now. Answer below.**

---

```dockerfile
# ── Stage 1: Build dependencies ───────────────────────────────────────────────
FROM python:3.12-slim AS builder

WORKDIR /app

RUN apt-get update && \
    apt-get install -y --no-install-recommends gcc libpq-dev && \
    rm -rf /var/lib/apt/lists/*

# Copy dependency files FIRST (cache layer optimisation)
# This layer only rebuilds when requirements.txt changes
COPY requirements.txt .

# Install into a custom prefix so we can copy them to the final stage
RUN pip install --no-cache-dir --prefix=/install -r requirements.txt

# ── Stage 2: Production image ─────────────────────────────────────────────────
FROM python:3.12-slim AS production

WORKDIR /app

# Create non-root user and group
RUN addgroup --system appgroup && \
    adduser --system --ingroup appgroup --no-create-home appuser

# Copy installed packages from the builder stage only — no compiler in final image
COPY --from=builder /install /usr/local

# Copy application source code last (changes most often)
COPY --chown=appuser:appgroup . .

USER appuser

EXPOSE 8000

# Exec form ensures the process is PID 1 and receives signals directly
ENTRYPOINT ["uvicorn", "app.main:app", "--host", "0.0.0.0", "--port", "8000"]
```

---

## Exercise 1.2 — Node.js App with Dev and Production Targets

**Requirements:**
- Base stage with Alpine
- Separate `deps` stage for package install
- `development` stage (bind mount friendly, npm run dev)
- `production` stage (only prod deps, compiled dist)
- Non-root user throughout

**Write it now. Answer below.**

---

```dockerfile
FROM node:20-alpine AS base
WORKDIR /app
RUN addgroup -S appgroup && adduser -S appuser -G appgroup

# ── Dependency stage ───────────────────────────────────────────────────────────
FROM base AS deps
COPY package.json package-lock.json ./
RUN npm ci

# ── Development stage ──────────────────────────────────────────────────────────
FROM base AS development
COPY --from=deps /app/node_modules ./node_modules
COPY . .
USER appuser
EXPOSE 3000
CMD ["npm", "run", "dev"]

# ── Build stage ────────────────────────────────────────────────────────────────
FROM base AS builder
COPY --from=deps /app/node_modules ./node_modules
COPY . .
RUN npm run build

# ── Production stage ───────────────────────────────────────────────────────────
FROM base AS production
ENV NODE_ENV=production

# Install only production dependencies (no devDependencies)
COPY package.json package-lock.json ./
RUN npm ci --only=production && npm cache clean --force

# Copy built output from builder stage
COPY --from=builder --chown=appuser:appgroup /app/dist ./dist

USER appuser
EXPOSE 3000
ENTRYPOINT ["node"]
CMD ["dist/server.js"]
```

---

## Exercise 1.3 — Go Binary on distroless (Ultra-Minimal)

**Requirements:**
- Builder stage: golang:1.21-alpine
- Static binary (CGO_ENABLED=0)
- Final stage: gcr.io/distroless/static:nonroot
- Result should be ~8MB image

**Write it now. Answer below.**

---

```dockerfile
# ── Build stage ────────────────────────────────────────────────────────────────
FROM golang:1.21-alpine AS builder

WORKDIR /build

# Cache module downloads — only re-runs when go.mod/go.sum change
COPY go.mod go.sum ./
RUN go mod download

# Build a fully static binary (no libc dependency)
COPY . .
RUN CGO_ENABLED=0 GOOS=linux GOARCH=amd64 \
    go build \
    -ldflags="-w -s" \
    -o server \
    ./cmd/server

# ── Final stage: distroless ────────────────────────────────────────────────────
# No shell, no package manager, no OS utilities — just your binary
FROM gcr.io/distroless/static:nonroot

WORKDIR /app
COPY --from=builder /build/server .

USER nonroot:nonroot
EXPOSE 8080
ENTRYPOINT ["/app/server"]
```

---

## Exercise 1.4 — Dockerfile with BuildKit Cache Mounts

**Requirements:**
- Python app
- Use `--mount=type=cache` for pip cache (BuildKit)
- Use `--mount=type=cache` for apt cache
- Single-stage for simplicity

**Write it now. Answer below.**

---

```dockerfile
# syntax=docker/dockerfile:1
FROM python:3.12-slim

WORKDIR /app

# apt cache is shared between builds — packages not re-downloaded
RUN --mount=type=cache,target=/var/cache/apt \
    apt-get update && \
    apt-get install -y --no-install-recommends libpq-dev curl && \
    rm -rf /var/lib/apt/lists/*

COPY requirements.txt .

# pip cache is shared between builds — wheels not re-downloaded
RUN --mount=type=cache,target=/root/.cache/pip \
    pip install -r requirements.txt

COPY . .

RUN addgroup --system appgroup && \
    adduser --system --ingroup appgroup appuser
USER appuser

EXPOSE 8000
ENTRYPOINT ["gunicorn", "app.main:app", "--bind", "0.0.0.0:8000", "--workers", "4"]
```

---

## Exercise 1.5 — Entrypoint Script Pattern (wait-for-deps + exec)

**Requirements:**
- entrypoint.sh waits for PostgreSQL and Redis
- Uses `exec "$@"` so the app is PID 1
- Dockerfile copies and uses the script

**Write it now. Answer below.**

---

```bash
#!/bin/sh
# entrypoint.sh
set -e

wait_for() {
    local host="$1"
    local port="$2"
    local attempts=0
    local max=30

    echo "Waiting for ${host}:${port}..."
    until nc -z "$host" "$port" 2>/dev/null; do
        attempts=$((attempts + 1))
        if [ "$attempts" -ge "$max" ]; then
            echo "ERROR: ${host}:${port} not ready after ${max} attempts"
            exit 1
        fi
        echo "  attempt ${attempts}/${max} — sleeping 2s..."
        sleep 2
    done
    echo "${host}:${port} is ready."
}

wait_for "${DB_HOST:-db}" "${DB_PORT:-5432}"
wait_for "${REDIS_HOST:-redis}" "${REDIS_PORT:-6379}"

echo "All dependencies ready. Starting application..."
exec "$@"
```

```dockerfile
FROM python:3.12-slim

WORKDIR /app

RUN apt-get update && \
    apt-get install -y --no-install-recommends netcat-openbsd && \
    rm -rf /var/lib/apt/lists/*

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

COPY . .

RUN addgroup --system appgroup && \
    adduser --system --ingroup appgroup --no-create-home appuser
USER appuser

EXPOSE 8000
ENTRYPOINT ["/entrypoint.sh"]
CMD ["uvicorn", "app.main:app", "--host", "0.0.0.0", "--port", "8000"]
```

---

# PART 2 — JENKINSFILES

---

## Exercise 2.1 — Simple Declarative Pipeline (Docker + K8s deploy)

**Requirements:**
- Stages: Checkout, Test, Build Image, Push Image, Deploy to Staging
- Use Docker agent
- Use credentials for registry
- Post: Slack on success/failure
- Timeout of 20 minutes

**Write it now. Answer below.**

---

```groovy
pipeline {
    agent {
        docker {
            image 'python:3.12-slim'
            args '-u root -v /var/run/docker.sock:/var/run/docker.sock'
        }
    }

    environment {
        APP_NAME     = 'myapp'
        REGISTRY     = 'registry.company.com'
        IMAGE_TAG    = "${env.BRANCH_NAME}-${env.BUILD_NUMBER}"
        DOCKER_CREDS = credentials('registry-credentials')
        KUBECONFIG   = credentials('kubeconfig-staging')
    }

    options {
        timeout(time: 20, unit: 'MINUTES')
        buildDiscarder(logRotator(numToKeepStr: '10'))
        disableConcurrentBuilds()
    }

    stages {
        stage('Checkout') {
            steps {
                checkout scm
            }
        }

        stage('Test') {
            steps {
                sh '''
                    pip install -r requirements.txt -r requirements-dev.txt --quiet
                    pytest tests/ -v --junitxml=test-results.xml
                '''
            }
            post {
                always {
                    junit 'test-results.xml'
                }
            }
        }

        stage('Build Image') {
            steps {
                sh """
                    docker build \
                        -t ${REGISTRY}/${APP_NAME}:${IMAGE_TAG} \
                        -t ${REGISTRY}/${APP_NAME}:latest \
                        .
                """
            }
        }

        stage('Push Image') {
            steps {
                sh """
                    echo ${DOCKER_CREDS_PSW} | \
                        docker login ${REGISTRY} -u ${DOCKER_CREDS_USR} --password-stdin
                    docker push ${REGISTRY}/${APP_NAME}:${IMAGE_TAG}
                    docker push ${REGISTRY}/${APP_NAME}:latest
                """
            }
        }

        stage('Deploy to Staging') {
            when {
                branch 'develop'
            }
            steps {
                sh """
                    export KUBECONFIG=${KUBECONFIG}
                    kubectl set image deployment/${APP_NAME} \
                        ${APP_NAME}=${REGISTRY}/${APP_NAME}:${IMAGE_TAG} \
                        -n staging
                    kubectl rollout status deployment/${APP_NAME} \
                        -n staging --timeout=5m
                """
            }
        }
    }

    post {
        success {
            slackSend(
                channel: '#deployments',
                color: 'good',
                message: ":white_check_mark: ${APP_NAME} ${IMAGE_TAG} deployed to staging"
            )
        }
        failure {
            slackSend(
                channel: '#deployments',
                color: 'danger',
                message: ":x: ${APP_NAME} ${IMAGE_TAG} pipeline FAILED — ${env.BUILD_URL}"
            )
        }
        always {
            cleanWs()
        }
    }
}
```

---

## Exercise 2.2 — Pipeline with Parallel Tests and Manual Production Gate

**Requirements:**
- Parallel: Unit Tests + Security Scan
- Build & Push after both pass
- Manual approval gate before production
- `when { branch 'main' }` for production
- Kubernetes pod agent with separate containers

**Write it now. Answer below.**

---

```groovy
pipeline {
    agent {
        kubernetes {
            yaml """
apiVersion: v1
kind: Pod
spec:
  containers:
  - name: python
    image: python:3.12-slim
    command: [cat]
    tty: true
  - name: docker
    image: docker:24-dind
    securityContext:
      privileged: true
    env:
    - name: DOCKER_TLS_CERTDIR
      value: ""
  - name: kubectl
    image: bitnami/kubectl:1.28
    command: [cat]
    tty: true
"""
        }
    }

    environment {
        APP_NAME     = 'myapp'
        REGISTRY     = 'registry.company.com'
        IMAGE_TAG    = "${env.BRANCH_NAME}-${env.GIT_COMMIT[0..7]}"
        DOCKER_CREDS = credentials('registry-credentials')
    }

    options {
        timeout(time: 30, unit: 'MINUTES')
        disableConcurrentBuilds()
        buildDiscarder(logRotator(numToKeepStr: '20'))
    }

    stages {
        stage('Checkout') {
            steps {
                checkout scm
            }
        }

        stage('Quality Gates') {
            parallel {
                stage('Unit Tests') {
                    steps {
                        container('python') {
                            sh '''
                                pip install -r requirements.txt -r requirements-dev.txt -q
                                pytest tests/unit -v --junitxml=unit-results.xml --cov=app
                            '''
                        }
                    }
                    post {
                        always {
                            junit 'unit-results.xml'
                        }
                    }
                }

                stage('Security Scan') {
                    steps {
                        container('python') {
                            sh 'pip-audit -r requirements.txt'
                        }
                    }
                }
            }
        }

        stage('Build & Push') {
            steps {
                container('docker') {
                    sh """
                        echo ${DOCKER_CREDS_PSW} | \
                            docker login ${REGISTRY} -u ${DOCKER_CREDS_USR} --password-stdin
                        docker build -t ${REGISTRY}/${APP_NAME}:${IMAGE_TAG} .
                        docker push ${REGISTRY}/${APP_NAME}:${IMAGE_TAG}
                    """
                }
            }
        }

        stage('Deploy Staging') {
            when { branch 'develop' }
            steps {
                container('kubectl') {
                    sh """
                        kubectl set image deployment/${APP_NAME} \
                            ${APP_NAME}=${REGISTRY}/${APP_NAME}:${IMAGE_TAG} \
                            -n staging
                        kubectl rollout status deployment/${APP_NAME} -n staging --timeout=5m
                    """
                }
            }
        }

        stage('Approve Production') {
            when { branch 'main' }
            steps {
                input(
                    message: "Deploy ${APP_NAME}:${IMAGE_TAG} to PRODUCTION?",
                    ok: 'Deploy Now',
                    submitter: 'ops-team'
                )
            }
        }

        stage('Deploy Production') {
            when { branch 'main' }
            steps {
                container('kubectl') {
                    sh """
                        kubectl set image deployment/${APP_NAME} \
                            ${APP_NAME}=${REGISTRY}/${APP_NAME}:${IMAGE_TAG} \
                            -n production
                        kubectl rollout status deployment/${APP_NAME} -n production --timeout=10m
                    """
                }
            }
        }
    }

    post {
        always { cleanWs() }
        failure {
            slackSend channel: '#builds', color: 'danger',
                message: ":x: FAILED: ${APP_NAME} ${IMAGE_TAG} — ${env.BUILD_URL}"
        }
    }
}
```

---

# PART 3 — GITHUB ACTIONS WORKFLOWS

---

## Exercise 3.1 — CI Workflow (Test + Build + Push)

**Requirements:**
- Trigger: push to main/develop, PR to main
- Jobs: test, build (needs test)
- Matrix test: Python 3.11 and 3.12
- Cache pip packages
- Push to GHCR only on push (not PR)
- Use docker/build-push-action with GHA cache

**Write it now. Answer below.**

---

```yaml
name: CI

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

env:
  REGISTRY: ghcr.io
  IMAGE_NAME: ${{ github.repository }}

jobs:
  test:
    name: Test (Python ${{ matrix.python-version }})
    runs-on: ubuntu-latest
    strategy:
      fail-fast: false
      matrix:
        python-version: ['3.11', '3.12']

    steps:
      - uses: actions/checkout@v4

      - name: Set up Python ${{ matrix.python-version }}
        uses: actions/setup-python@v5
        with:
          python-version: ${{ matrix.python-version }}

      - name: Cache pip
        uses: actions/cache@v4
        with:
          path: ~/.cache/pip
          key: ${{ runner.os }}-pip-${{ hashFiles('requirements*.txt') }}
          restore-keys: ${{ runner.os }}-pip-

      - name: Install dependencies
        run: pip install -r requirements.txt -r requirements-dev.txt

      - name: Lint
        run: |
          flake8 . --max-line-length=120
          black --check .

      - name: Test
        run: pytest --cov=. --cov-report=xml -v

  build:
    name: Build & Push Image
    runs-on: ubuntu-latest
    needs: test
    if: github.event_name == 'push'
    permissions:
      contents: read
      packages: write

    steps:
      - uses: actions/checkout@v4

      - name: Set up Docker Buildx
        uses: docker/setup-buildx-action@v3

      - name: Login to GHCR
        uses: docker/login-action@v3
        with:
          registry: ${{ env.REGISTRY }}
          username: ${{ github.actor }}
          password: ${{ secrets.GITHUB_TOKEN }}

      - name: Extract metadata
        id: meta
        uses: docker/metadata-action@v5
        with:
          images: ${{ env.REGISTRY }}/${{ env.IMAGE_NAME }}
          tags: |
            type=sha,prefix={{branch}}-,format=short
            type=ref,event=branch
            type=raw,value=latest,enable=${{ github.ref == 'refs/heads/main' }}

      - name: Build and push
        uses: docker/build-push-action@v5
        with:
          context: .
          push: true
          tags: ${{ steps.meta.outputs.tags }}
          labels: ${{ steps.meta.outputs.labels }}
          cache-from: type=gha
          cache-to: type=gha,mode=max
```

---

# PART 4 — KUBERNETES MANIFESTS

---

## Exercise 4.1 — Full Deployment with Probes, Resources, and HPA

**Requirements:**
- Deployment: 3 replicas, rolling update (maxUnavailable: 0)
- Liveness and Readiness probes on /healthz and /ready
- Resource requests and limits
- ConfigMap for env vars, Secret for DB password
- HPA: min 2, max 10, scale at 70% CPU

**Write it now. Answer below.**

---

```yaml
# ── ConfigMap ─────────────────────────────────────────────────────────────────
apiVersion: v1
kind: ConfigMap
metadata:
  name: api-config
  namespace: production
data:
  LOG_LEVEL: "info"
  APP_PORT: "8080"

---
# ── Secret ────────────────────────────────────────────────────────────────────
apiVersion: v1
kind: Secret
metadata:
  name: api-secrets
  namespace: production
type: Opaque
stringData:
  DB_PASSWORD: "supersecretpassword"

---
# ── Deployment ────────────────────────────────────────────────────────────────
apiVersion: apps/v1
kind: Deployment
metadata:
  name: api
  namespace: production
  labels:
    app: api
spec:
  replicas: 3
  selector:
    matchLabels:
      app: api
  strategy:
    type: RollingUpdate
    rollingUpdate:
      maxSurge: 1
      maxUnavailable: 0
  minReadySeconds: 10
  template:
    metadata:
      labels:
        app: api
    spec:
      containers:
        - name: api
          image: registry.company.com/api:v1.0.0
          ports:
            - containerPort: 8080
          envFrom:
            - configMapRef:
                name: api-config
          env:
            - name: DB_PASSWORD
              valueFrom:
                secretKeyRef:
                  name: api-secrets
                  key: DB_PASSWORD
          resources:
            requests:
              memory: "128Mi"
              cpu: "250m"
            limits:
              memory: "512Mi"
              cpu: "1"
          livenessProbe:
            httpGet:
              path: /healthz
              port: 8080
            initialDelaySeconds: 15
            periodSeconds: 20
            failureThreshold: 3
          readinessProbe:
            httpGet:
              path: /ready
              port: 8080
            initialDelaySeconds: 5
            periodSeconds: 10
            failureThreshold: 3
          lifecycle:
            preStop:
              exec:
                command: ["/bin/sh", "-c", "sleep 5"]
      terminationGracePeriodSeconds: 30

---
# ── Service ───────────────────────────────────────────────────────────────────
apiVersion: v1
kind: Service
metadata:
  name: api-service
  namespace: production
spec:
  selector:
    app: api
  ports:
    - protocol: TCP
      port: 80
      targetPort: 8080
  type: ClusterIP

---
# ── HPA ───────────────────────────────────────────────────────────────────────
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: api-hpa
  namespace: production
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: api
  minReplicas: 2
  maxReplicas: 10
  metrics:
    - type: Resource
      resource:
        name: cpu
        target:
          type: Utilization
          averageUtilization: 70
```

---

## Exercise 4.2 — NetworkPolicy (allow only frontend to backend)

**Requirements:**
- Policy on `backend` pods
- Allow ingress only from pods with `app: frontend`
- Allow egress only to namespace labeled `name: database` on port 5432
- Allow DNS egress (UDP 53)

**Write it now. Answer below.**

---

```yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: backend-policy
  namespace: production
spec:
  podSelector:
    matchLabels:
      app: backend
  policyTypes:
    - Ingress
    - Egress

  ingress:
    - from:
        - podSelector:
            matchLabels:
              app: frontend
      ports:
        - protocol: TCP
          port: 8080

  egress:
    # Allow traffic to database namespace
    - to:
        - namespaceSelector:
            matchLabels:
              name: database
      ports:
        - protocol: TCP
          port: 5432
    # Allow DNS resolution
    - to:
        - namespaceSelector: {}
      ports:
        - protocol: UDP
          port: 53
```

---

# PART 5 — BASH SCRIPTS

---

## Exercise 5.1 — Deployment Script with Rollback on Failure

**Write a bash script that:**
- Takes APP_NAME, IMAGE_TAG, NAMESPACE from environment
- Validates required vars are set
- Runs `kubectl set image` and waits for rollout
- If rollout fails: auto-rolls back and exits non-zero
- Logs every step with timestamps

**Write it now. Answer below.**

---

```bash
#!/bin/bash
set -euo pipefail

# ── Config ─────────────────────────────────────────────────────────────────────
APP_NAME="${APP_NAME:?APP_NAME is required}"
IMAGE_TAG="${IMAGE_TAG:?IMAGE_TAG is required}"
NAMESPACE="${NAMESPACE:-staging}"
REGISTRY="${REGISTRY:-registry.company.com}"
TIMEOUT="${TIMEOUT:-5m}"

# ── Helpers ────────────────────────────────────────────────────────────────────
log()  { echo "[$(date '+%Y-%m-%d %H:%M:%S')] $*"; }
die()  { log "ERROR: $*" >&2; exit 1; }

# ── Pre-flight ─────────────────────────────────────────────────────────────────
command -v kubectl >/dev/null 2>&1 || die "kubectl not found in PATH"

log "Deploying ${APP_NAME}:${IMAGE_TAG} to namespace '${NAMESPACE}'"

# ── Deploy ─────────────────────────────────────────────────────────────────────
log "Setting image: ${REGISTRY}/${APP_NAME}:${IMAGE_TAG}"
kubectl set image deployment/"${APP_NAME}" \
    "${APP_NAME}=${REGISTRY}/${APP_NAME}:${IMAGE_TAG}" \
    --namespace "${NAMESPACE}"

log "Waiting for rollout (timeout: ${TIMEOUT})..."
if kubectl rollout status deployment/"${APP_NAME}" \
    --namespace "${NAMESPACE}" \
    --timeout "${TIMEOUT}"; then
    log "Rollout succeeded."
else
    log "Rollout FAILED. Initiating rollback..."
    kubectl rollout undo deployment/"${APP_NAME}" --namespace "${NAMESPACE}"

    log "Waiting for rollback to complete..."
    kubectl rollout status deployment/"${APP_NAME}" \
        --namespace "${NAMESPACE}" --timeout "${TIMEOUT}" || true

    die "Deployment failed. Rollback has been applied."
fi

log "Final pod state:"
kubectl get pods --namespace "${NAMESPACE}" \
    -l "app=${APP_NAME}" \
    --no-headers
```

---

## Exercise 5.2 — Disk Usage Monitor with Alert Threshold

**Write a script that:**
- Checks all mounted filesystems
- Alerts (echoes loudly + exits non-zero) if any filesystem exceeds THRESHOLD%
- THRESHOLD defaults to 80 but is configurable via env var
- Excludes tmpfs filesystems

**Write it now. Answer below.**

---

```bash
#!/bin/bash
set -euo pipefail

THRESHOLD="${THRESHOLD:-80}"

log()  { echo "[$(date '+%H:%M:%S')] $*"; }
warn() { echo "[$(date '+%H:%M:%S')] WARNING: $*" >&2; }

check_disk() {
    df -h --output=source,fstype,pcent,target \
    | awk 'NR>1 && $2 != "tmpfs" && $2 != "devtmpfs" {
        usage = $3
        gsub(/%/, "", usage)
        if (usage+0 > '"$THRESHOLD"') {
            print $0
        }
    }'
}

VIOLATIONS=$(check_disk)

if [ -n "$VIOLATIONS" ]; then
    warn "Disk usage exceeds ${THRESHOLD}% threshold!"
    warn "---"
    echo "$VIOLATIONS" >&2
    warn "---"
    exit 1
fi

log "All filesystems OK (below ${THRESHOLD}%)"
```

---

## Exercise 5.3 — Log Rotation (Keep Last 5 Versions)

**Write it now. Answer below.**

---

```bash
#!/bin/bash
set -euo pipefail

LOG_FILE="${1:?Usage: $0 <logfile> [max_versions]}"
MAX_VERSIONS="${2:-5}"

log() { echo "[$(date '+%H:%M:%S')] $*"; }

[ -f "$LOG_FILE" ] || { echo "File not found: $LOG_FILE"; exit 1; }

# Shift existing versions: N -> N+1, starting from the oldest
for i in $(seq $((MAX_VERSIONS - 1)) -1 1); do
    if [ -f "${LOG_FILE}.${i}" ]; then
        mv "${LOG_FILE}.${i}" "${LOG_FILE}.$((i + 1))"
        log "Moved: ${LOG_FILE}.${i} -> ${LOG_FILE}.$((i + 1))"
    fi
done

# Move current log to .1
mv "$LOG_FILE" "${LOG_FILE}.1"
log "Archived: ${LOG_FILE} -> ${LOG_FILE}.1"

# Create a fresh empty log file
touch "$LOG_FILE"
log "Created fresh: $LOG_FILE"

# Remove any versions beyond MAX_VERSIONS
OVERFLOW="${LOG_FILE}.$((MAX_VERSIONS + 1))"
if [ -f "$OVERFLOW" ]; then
    rm -f "$OVERFLOW"
    log "Removed overflow: $OVERFLOW"
fi
```

---

## Exercise 5.4 — Wait for TCP Port (Entrypoint Pattern)

**Write it now. Answer below.**

---

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
            echo "ERROR: ${host}:${port} not available after ${max_attempts} attempts"
            exit 1
        fi
        echo "  Attempt ${attempt}/${max_attempts} — retrying in ${delay}s..."
        sleep "$delay"
    done
    echo "${host}:${port} is READY (${attempt} attempts)"
}

wait_for_port "${DB_HOST:-localhost}"    "${DB_PORT:-5432}"
wait_for_port "${REDIS_HOST:-localhost}" "${REDIS_PORT:-6379}"

exec "$@"
```

---

# PART 6 — PYTHON SCRIPTS

---

## Exercise 6.1 — SQS Worker

**Write a Python SQS worker that:**
- Uses long polling (WaitTimeSeconds=20)
- Processes messages with error handling
- Deletes only on success (allows DLQ to catch failures)
- Has a `run_worker()` loop

**Write it now. Answer below.**

---

```python
import boto3
import json
import logging
import os

logging.basicConfig(level=logging.INFO, format="%(asctime)s %(levelname)s %(message)s")
logger = logging.getLogger(__name__)

sqs = boto3.client("sqs", region_name=os.environ.get("AWS_REGION", "us-east-1"))
QUEUE_URL = os.environ["SQS_QUEUE_URL"]


def receive_messages(max_count: int = 10) -> list:
    response = sqs.receive_message(
        QueueUrl=QUEUE_URL,
        MaxNumberOfMessages=max_count,
        WaitTimeSeconds=20,        # long polling — reduces empty responses and cost
        VisibilityTimeout=60,      # must be >= your max processing time
        MessageAttributeNames=["All"]
    )
    return response.get("Messages", [])


def delete_message(receipt_handle: str):
    sqs.delete_message(QueueUrl=QUEUE_URL, ReceiptHandle=receipt_handle)


def process(data: dict):
    """Your business logic here."""
    logger.info(f"Processing message: {data}")
    # raise Exception("simulated failure")  # uncomment to test DLQ


def run_worker():
    logger.info(f"Worker started. Polling: {QUEUE_URL}")
    while True:
        messages = receive_messages()
        if not messages:
            continue

        for msg in messages:
            msg_id = msg["MessageId"]
            try:
                data = json.loads(msg["Body"])
                process(data)
                delete_message(msg["ReceiptHandle"])
                logger.info(f"[OK] Processed and deleted message: {msg_id}")
            except Exception as e:
                # Do NOT delete — message becomes visible again after VisibilityTimeout
                # After maxReceiveCount failures it goes to the DLQ
                logger.error(f"[FAIL] Message {msg_id} failed: {e}")


if __name__ == "__main__":
    run_worker()
```

---

## Exercise 6.2 — Retry Decorator

**Write a retry decorator with exponential backoff.**

**Write it now. Answer below.**

---

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
) -> Callable:
    """Retry a function with exponential backoff."""
    def decorator(func: Callable) -> Callable:
        @functools.wraps(func)
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            current_delay = delay
            for attempt in range(1, max_attempts + 1):
                try:
                    return func(*args, **kwargs)
                except exceptions as e:
                    if attempt == max_attempts:
                        logger.error(
                            f"{func.__name__}: all {max_attempts} attempts failed. "
                            f"Last error: {e}"
                        )
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
import requests

@retry(max_attempts=3, delay=1.0, backoff=2.0, exceptions=(requests.exceptions.RequestException,))
def fetch_data(url: str) -> dict:
    response = requests.get(url, timeout=5)
    response.raise_for_status()
    return response.json()
```

---

## Exercise 6.3 — Health Check Script for Multiple Services

**Write it now. Answer below.**

---

```python
import requests
import sys
import logging

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
            logger.info(f"[OK]   {name:10s} {url}")
            return True
        logger.warning(f"[WARN] {name:10s} HTTP {r.status_code}")
        return False
    except requests.exceptions.RequestException as e:
        logger.error(f"[FAIL] {name:10s} {e}")
        return False


def main():
    results = {name: check(name, url) for name, url in SERVICES.items()}
    failed = [name for name, ok in results.items() if not ok]

    if failed:
        logger.error(f"Unhealthy services: {failed}")
        sys.exit(1)

    logger.info("All services are healthy.")


if __name__ == "__main__":
    main()
```

---

# PART 7 — QUICK-FIRE ORAL QUESTIONS

Practice answering these out loud in 30 seconds or less.

```
DOCKER:
  1. What is the difference between CMD and ENTRYPOINT?
  2. Why must cleanup be in the SAME RUN layer?
  3. What is PID 1 and why does it matter?
  4. What is the difference between a named volume and a bind mount?
  5. How does Docker image layer caching work? When is it invalidated?
  6. What is a multi-stage build and when would you use it?
  7. What does .dockerignore do?
  8. What is the difference between EXPOSE and -p?

AWS:
  1. What is the difference between SQS and Lambda?
  2. Why put SQS in front of Lambda instead of invoking Lambda directly?
  3. What is SQS visibility timeout? What happens if you don't delete the message?
  4. What is a DLQ and when does a message end up there?
  5. What is long polling and why should you always use it?
  6. What is the difference between an IAM Role and an IAM User?
  7. How does an EC2 instance get temporary AWS credentials without a hardcoded key?
  8. What is the difference between a Security Group and a NACL?
  9. What is a NAT Gateway and why is it in a public subnet?
  10. What is a Lambda cold start and how do you mitigate it?

KUBERNETES:
  1. What is the difference between Deployment, StatefulSet, DaemonSet?
  2. What is the difference between a liveness and readiness probe?
  3. What is a Service? What are the types?
  4. What is the difference between resource requests and limits?
  5. A pod is in CrashLoopBackOff — first two commands you run?
  6. How does a rolling update work with maxUnavailable: 0?
  7. What is a ConfigMap vs a Secret?

LINUX:
  1. What is the difference between SIGTERM and SIGKILL?
  2. What is a zombie process?
  3. What is an inode?
  4. What does set -euo pipefail do?
  5. What is the difference between [ ] and [[ ]]?
  6. What does exec "$@" do in an entrypoint script?

NETWORKING:
  1. Explain the TCP three-way handshake.
  2. What is the difference between 401 and 403?
  3. What is the difference between 502 and 504?
  4. What is the difference between Layer 4 and Layer 7 load balancers?
  5. What is the difference between DROP and REJECT in iptables?
  6. What is CIDR /24 in terms of number of hosts?
  7. How does DNS resolution work end-to-end?

CI/CD:
  1. What is the difference between Continuous Delivery and Continuous Deployment?
  2. How do you handle secrets in a Jenkins pipeline?
  3. What is pipeline as code and why is it important?
  4. How do you speed up a slow CI pipeline?
  5. What is a blue/green deployment?

MONITORING:
  1. What are the three pillars of observability?
  2. What is the RED method?
  3. What is the difference between a recording rule and an alerting rule in Prometheus?
  4. What is the difference between a Counter and a Gauge?
  5. What is an SLO vs an SLA vs an error budget?
  6. How does log collection work in Kubernetes?
```

---

# PART 8 — SCENARIO-BASED QUESTIONS

Practice these as if you're in the interview. Think out loud.

---

**Scenario 1:**
"You deploy a new version. The rollout completes but users report errors. You check the logs and see database connection timeouts. Walk me through your investigation."

```
My answer framework:
1. Check if the new deployment changed anything DB-related (connection string, pool size, schema migration).
2. kubectl top pods — is the new version using more CPU/memory than expected?
3. Check DB metrics: active connections, query latency, lock waits.
4. Is the connection pool exhausted? How many replicas * pool size connections?
5. Did a schema migration run? Is there a lock on a table?
6. Compare error rate before and after deployment on Grafana.
7. If critical: kubectl rollout undo deployment/myapp
8. Root cause: likely a missing RDS Proxy or connection pool misconfiguration.
```

---

**Scenario 2:**
"Your SQS queue depth is growing. What does that mean and what do you do?"

```
My answer framework:
Queue depth growing = consumers are processing messages SLOWER than producers are sending them.

Immediate checks:
1. Are consumers running? kubectl get pods -l app=worker
2. Are consumers erroring? kubectl logs <worker-pod> --tail=100
3. Check CloudWatch: SQS NumberOfMessagesSent vs NumberOfMessagesDeleted (should be equal).
4. Check ApproximateAgeOfOldestMessage — how old is the oldest message?
5. Check DLQ depth — are messages failing and landing in DLQ?

Actions:
- Scale up consumers (kubectl scale deployment worker --replicas=10)
- If Lambda: check if concurrency limit is hit (Lambda Throttles metric)
- If consumers are erroring: fix the bug, messages will retry
- If DLQ growing: inspect DLQ messages, fix processing logic, redrive
- Long-term: add an alarm on queue depth and auto-scaling
```

---

**Scenario 3:**
"How would you reduce the size of a Docker image that is currently 1.2GB?"

```
My answer framework:
1. docker history myapp:latest — find which layers are large.
2. Use multi-stage build: separate build stage from runtime stage.
3. Switch base image: ubuntu -> debian-slim -> alpine -> distroless.
4. Combine RUN commands — cleanup in the SAME layer.
5. Use --no-install-recommends for apt packages.
6. Use .dockerignore to exclude tests, docs, .git from build context.
7. Use pip install --no-cache-dir.
8. For compiled languages (Go, Rust): static binary in distroless/scratch.
```

---

**Scenario 4:**
"CI pipeline takes 25 minutes. How would you bring it under 10 minutes?"

```
My answer framework:
1. Profile first: which stage is slowest? (look at stage timings)
2. Cache dependencies — npm ci, pip install, maven dependencies.
3. Parallelise: run unit tests + lint + security scan in parallel.
4. Docker BuildKit + registry layer cache — don't rebuild unchanged layers.
5. Fail fast: put fastest checks first (lint takes 30s, integration tests take 15min).
6. Pre-built CI agent image with all tools already installed.
7. Split test suite: shard tests across multiple agents.
8. Run only tests affected by changed files (pytest --co with file filters).
```
```

Now creating the last two remaining files: