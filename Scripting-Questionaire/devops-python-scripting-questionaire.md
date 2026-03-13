# DevOps & SRE Python Scripting Interview Questionnaire (LeetCode + Codeforces Style)

> Priority order followed: **Python scripting first**, then Docker, Jenkins, Prometheus, Grafana Loki, and practical SRE automation.
>  
> Every problem is written in a scenario-first style with:
> - Difficulty
> - Topic tags
> - 5–8 line descriptive context
> - Input format
> - Output format
> - Constraints
> - Concrete sample test cases with explanation
> - Follow-up interview prompts

---

## Problem 1 — Rotate Logs by Size

**Difficulty:** Easy  
**Topic Tags:** `Scripting` `Files` `Automation` `Linux`

### Description
You are maintaining a Python-based log collector running on a VM with limited disk space.  
The service writes continuously to `app.log`, and if this file grows too large, the host can run out of disk.  
Implement a rotation routine that checks log size and rotates it when threshold is exceeded.  
Backups should be shifted as `app.log.1`, `app.log.2`, ..., up to `k` backups.  
If `k` backups already exist, the oldest one must be deleted before shifting.  
If file size is within limit, do nothing and return `False`.  
Your function should return whether a rotation happened.

### Input
- `log_path: str`
- `max_size: int` (bytes)
- `k: int` (number of retained backups)
- `current_size: int` (for test simulation)

### Output
- `bool` — `True` if rotated, otherwise `False`.

### Constraints
- `1 <= max_size <= 10^9`
- `1 <= k <= 100`
- `0 <= current_size <= 10^12`

### Example 1
**Input**
- `log_path = "app.log"`
- `max_size = 1048576`
- `k = 3`
- `current_size = 2200000`

**Output**
- `True`

**Explanation**  
`current_size > max_size`, so rotate. Existing backups are shifted (`.2 -> .3`, `.1 -> .2`), `app.log -> app.log.1`.

### Example 2
**Input**
- `current_size = 500000`, `max_size = 1048576`

**Output**
- `False`

**Explanation**  
No rotation needed because the file is still under threshold.

### Follow-up
- How would you make rotation safe under concurrent writers?
- How would you implement both date-based and size-based rotation?

---

## Problem 2 — Validate Cron Expression (5 Fields)

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Cron` `Parsing` `Regex`

### Description
Your team stores cron schedules in a YAML configuration that developers edit frequently.  
Many production incidents occur because invalid cron strings are merged and deployed.  
Write a validator for standard 5-field cron format:
`minute hour day-of-month month day-of-week`.  
Support wildcard `*`, list `a,b,c`, range `a-b`, and step `*/n` or `a-b/n`.  
Return whether expression is valid under numeric bounds for each field.  
Ignore nicknames like `@daily` in this version.  
The validator is used in CI as a fail-fast quality gate.

### Input
- `expr: str`

### Output
- `bool`

### Constraints
- Minute: `0..59`
- Hour: `0..23`
- DOM: `1..31`
- Month: `1..12`
- DOW: `0..7`
- Exactly 5 fields separated by single/multiple spaces

### Example 1
**Input**
- `expr = "*/15 0 1,15 * 1-5"`

**Output**
- `True`

**Explanation**  
All fields are syntactically valid and values are inside permitted ranges.

### Example 2
**Input**
- `expr = "61 * * * *"`

**Output**
- `False`

**Explanation**  
Minute field `61` is outside valid range `0..59`.

### Follow-up
- Add named months/days (`JAN`, `MON`).
- Return field-wise error details instead of a boolean.

---

## Problem 3 — Compute Next Cron Run Time

**Difficulty:** Hard  
**Topic Tags:** `Scripting` `Cron` `Datetime` `Algorithms`

### Description
Your incident bot needs to predict the next run time of every scheduled job for a dashboard.  
Given a valid 5-field cron expression and a current UTC timestamp, compute the next execution time strictly greater than current time.  
The logic must correctly handle day, month, and year rollovers.  
Assume cron resolution is at minute granularity (no seconds field).  
Inputs are guaranteed valid according to Problem 2.  
Return exact UTC datetime for next occurrence.  
This function is used by both alerting and planner services.

### Input
- `expr: str`
- `current_ts: datetime` (UTC)

### Output
- `datetime` (UTC)

### Constraints
- Year range in tests: `1970..2100`
- Expression always valid
- Must return in finite time

### Example
**Input**
- `expr = "0 */6 * * *"`
- `current_ts = 2025-01-01 05:59:00 UTC`

**Output**
- `2025-01-01 06:00:00 UTC`

**Explanation**  
Runs every 6 hours at minute 0 (`00:00,06:00,12:00,18:00`); next after `05:59` is `06:00`.

### Follow-up
- Add timezone and DST-safe support.
- Generate next `N` occurrences efficiently.

---

## Problem 4 — Grep with Context Windows

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Regex` `Logs` `Streaming`

### Description
You are debugging intermittent production failures using very large log files (multi-GB).  
Build a grep-like Python utility that returns matching lines plus surrounding context.  
For each matched line, include `before` lines and `after` lines like `grep -B -A`.  
Output must include line numbers and preserve file order.  
The solution should be stream-friendly and not load the whole file into memory.  
Support case-sensitive and case-insensitive modes.  
Overlapping context windows should be merged cleanly.

### Input
- `lines: List[str]` (simulate file)
- `pattern: str` (regex)
- `before: int`
- `after: int`
- `case_sensitive: bool`

### Output
- List of blocks; each block is list of `(line_no, text)`.

### Constraints
- `0 <= before, after <= 1000`
- `1 <= len(lines) <= 10^6`

### Example
**Input**
- `lines = ["ok", "warn", "ERROR timeout", "retry", "ok"]`
- `pattern = "ERROR"`
- `before = 1`
- `after = 1`
- `case_sensitive = True`

**Output**
- `[[ (2,"warn"), (3,"ERROR timeout"), (4,"retry") ]]`

**Explanation**  
Line 3 matches; one line before and one line after are included.

### Follow-up
- Add inverse match (`-v` behavior).
- Add multiple-file glob support.

---

## Problem 5 — Count Unique Client IPs

**Difficulty:** Easy  
**Topic Tags:** `Scripting` `Logs` `Regex` `Networking`

### Description
A security analyst asks you to estimate unique visitors from web access logs.  
Each line starts with an IP address, but malformed lines may exist.  
Parse logs and count unique valid client IPs (IPv4 and IPv6).  
Ignore invalid addresses and empty lines.  
Return only the count for this version.  
The parser should be robust against noisy data in real production logs.  
This is a common interview warm-up for log parsing.

### Input
- `log_lines: List[str]`

### Output
- `int` unique IP count

### Constraints
- `1 <= len(log_lines) <= 10^6`
- Line length up to `4096`

### Example
**Input**
- `["10.0.0.1 - - [..]", "10.0.0.2 - - [..]", "10.0.0.1 - - [..]", "bad line"]`

**Output**
- `2`

**Explanation**  
Two valid unique IPs (`10.0.0.1`, `10.0.0.2`) are found; malformed line is ignored.

### Follow-up
- Return top-K IPs by request count.
- Stream from file handle instead of in-memory list.

---

## Problem 6 — Top N Normalized Error Messages

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Logs` `Parsing` `Collections`

### Description
Your service emits verbose error logs where dynamic values (request IDs, user IDs, timestamps) make similar errors look different.  
SRE wants a frequency report of actual error patterns, not raw lines.  
Given log lines, extract entries at `ERROR` level and normalize dynamic tokens.  
For example, numbers/UUID-like values can be replaced with placeholders.  
After normalization, count frequency of each message template.  
Return top `N` templates sorted by count descending, then lexicographically.  
This mirrors real incident triage workflows.

### Input
- `lines: List[str]`
- `n: int`

### Output
- `List[(message_template: str, count: int)]`

### Constraints
- `1 <= n <= 1000`
- `1 <= len(lines) <= 10^6`
- You must clearly define normalization policy in solution

### Example
**Input**
- `lines = [`
  - `"2025-01-01 ERROR user 42 timeout after 3000ms",`
  - `"2025-01-01 INFO health ok",`
  - `"2025-01-02 ERROR user 57 timeout after 2800ms",`
  - `"2025-01-02 ERROR user 57 db unavailable"`
- `]`
- `n = 2`

**Output**
- `[("ERROR user <NUM> timeout after <NUM>ms", 2), ("ERROR user <NUM> db unavailable", 1)]`

**Explanation**  
Two timeout lines differ by numbers but normalize to one template with count 2.

### Follow-up
- Support JSON logs where message is nested in `log.message`.
- Persist rolling counts between script runs.

---

## Problem 7 — Detect Error Spike in Rolling Window

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Monitoring` `Sliding Window` `SRE`

### Description
On-call policy says: “page if more than `T` errors happen in any 5-minute window.”  
Given sorted error timestamps, detect whether policy is violated.  
Your function should run in O(n) using two pointers or deque.  
If violated, optionally return first violating window bounds.  
This is commonly used in custom alert simulation tools.  
Input size can be very large, so avoid nested loops.  
Assume timestamps are UTC and strictly non-decreasing.

### Input
- `events: List[int]` (epoch seconds, sorted)
- `T: int`
- `window_sec: int` (e.g., 300)

### Output
- `bool` (or tuple with window bounds)

### Constraints
- `1 <= len(events) <= 10^6`
- `1 <= T <= 10^6`
- `1 <= window_sec <= 86400`

### Example
**Input**
- `events = [0, 10, 20, 40, 100, 120, 140]`
- `T = 4`
- `window_sec = 60`

**Output**
- `True`

**Explanation**  
Within `[0,60]`, there are 4 events at 0,10,20,40 which meets threshold.

### Follow-up
- Return all violating windows.
- Make threshold “strictly greater than T” configurable.

---

## Problem 8 — Endpoint Health Check with Retry + Backoff

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `HTTP` `Reliability` `Retry`

### Description
You are writing a deployment gate that checks service health before traffic switch.  
A single failed ping should not fail deployment because transient errors are common.  
Implement retry logic for timeout/5xx responses with exponential backoff and jitter.  
Stop immediately on first 2xx response.  
Return whether health check succeeded, number of attempts, and total wait time.  
Backoff must be capped to avoid excessive delays.  
This mirrors real pre-deploy verification scripts.

### Input
- `responses: List[int or "TIMEOUT"]` (simulated attempts)
- `max_retries: int`
- `base_delay_ms: int`
- `max_delay_ms: int`

### Output
- `(success: bool, attempts: int, total_delay_ms: int)`

### Constraints
- `0 <= max_retries <= 10`
- `1 <= base_delay_ms <= max_delay_ms <= 60000`

### Example
**Input**
- `responses = ["TIMEOUT", 503, 200]`
- `max_retries = 4`
- `base_delay_ms = 100`
- `max_delay_ms = 1000`

**Output**
- `(True, 3, 300)`  *(jitter omitted in sample)*

**Explanation**  
First two attempts fail and backoff delays are applied (100 + 200), third succeeds.

### Follow-up
- Add circuit-breaker state.
- Retry only idempotent methods.

---

## Problem 9 — CPU Threshold Consecutive Alert

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Processes` `Monitoring` `Linux`

### Description
You collect process metrics every minute and want noise-resistant alerts.  
Alert only if a process exceeds CPU threshold for `k` consecutive samples.  
Given snapshots, detect all process IDs that satisfy this condition.  
Process list can change between snapshots (new/terminated processes).  
Counter for a process resets when it drops below threshold or disappears.  
Return all alerting PIDs sorted ascending.  
This reduces false positives in real monitoring pipelines.

### Input
- `samples: List[List[(pid:int, cpu:float)]]`
- `threshold: float`
- `k: int`

### Output
- `List[int]`

### Constraints
- `1 <= len(samples) <= 10^4`
- Up to `10^4` processes per sample
- `1 <= k <= len(samples)`

### Example
**Input**
- `samples = [`
  - `[(1,30),(2,90)],`
  - `[(1,40),(2,92)],`
  - `[(1,20),(2,95)]`
- `]`
- `threshold = 80`
- `k = 3`

**Output**
- `[2]`

**Explanation**  
PID 2 stays above 80 for all 3 consecutive samples.

### Follow-up
- Distinguish process restarts via `(pid,start_time)`.
- Add cooldown to suppress repeated alerts.

---

## Problem 10 — Cleanup Candidate Finder

**Difficulty:** Easy  
**Topic Tags:** `Scripting` `Filesystem` `Automation`

### Description
A maintenance script must identify stale large files for cleanup review.  
Each file has path, size in bytes, and age in days since last access.  
A file is candidate if `age >= D` and `size >= S`.  
Some critical directories are protected and must never be included.  
Return candidate file paths sorted by size descending.  
This task tests filtering + sorting under operational constraints.  
Do not actually delete files in this problem.

### Input
- `files: List[(path:str, size:int, age_days:int)]`
- `D: int`
- `S: int`
- `protected_prefixes: List[str]`

### Output
- `List[str]`

### Constraints
- `1 <= len(files) <= 10^6`
- `0 <= age_days <= 10^5`
- `0 <= size <= 10^12`

### Example
**Input**
- `files = [("/var/log/a.log", 2000, 40), ("/etc/passwd", 2048, 400), ("/tmp/b.bin", 500, 10)]`
- `D = 30`
- `S = 1000`
- `protected_prefixes = ["/etc"]`

**Output**
- `["/var/log/a.log"]`

**Explanation**  
`/etc/passwd` matches size/age but is protected; `/tmp/b.bin` too young.

### Follow-up
- Add dry-run report with reclaimable bytes.
- Add trash/restore semantics.

---

## Problem 11 — JSON Log Field Extractor

**Difficulty:** Easy  
**Topic Tags:** `Scripting` `JSON` `Logs`

### Description
Your observability pipeline receives one JSON object per log line.  
Different teams only need a few selected fields in TSV format.  
Implement extractor supporting nested keys like `kubernetes.pod_name`.  
If a field is missing, emit empty string for that column.  
Malformed JSON lines should be ignored safely.  
Return extracted rows in original order.  
This is common in ad-hoc incident analysis tooling.

### Input
- `lines: List[str]`
- `fields: List[str]`

### Output
- `List[str]` (TSV rows)

### Constraints
- `1 <= len(fields) <= 100`
- `1 <= len(lines) <= 10^6`

### Example
**Input**
- `lines = ['{"ts":"1","level":"ERROR","msg":"timeout"}', '{"ts":"2","msg":"ok"}']`
- `fields = ["ts","level","msg"]`

**Output**
- `["1\tERROR\ttimeout", "2\t\tok"]`

**Explanation**  
Second line lacks `level`, so middle column is empty.

### Follow-up
- Add array-path syntax (`items[0].name`).
- Add schema summary of missing-field rates.

---

## Problem 12 — Environment Variable Linter

**Difficulty:** Easy  
**Topic Tags:** `Scripting` `Config` `Validation`

### Description
A startup script fails often due to missing or placeholder environment values.  
Write a linter that verifies required keys and suspicious values.  
Treat empty strings, whitespace-only strings, and placeholders (`changeme`, `todo`, `xxx`) as invalid.  
Return two lists: missing keys and invalid keys.  
Comparison should be case-insensitive for placeholder detection.  
This check is used in CI and local pre-run validation.  
No secret values should be printed in output.

### Input
- `env: Dict[str,str]`
- `required: List[str]`

### Output
- `(missing: List[str], invalid: List[str])`

### Constraints
- `0 <= len(env), len(required) <= 10^5`

### Example
**Input**
- `env = {"API_KEY":"changeme", "PORT":"8080"}`
- `required = ["API_KEY","DB_URL"]`

**Output**
- `missing = ["DB_URL"]`
- `invalid = ["API_KEY"]`

**Explanation**  
`DB_URL` absent; `API_KEY` uses placeholder value.

### Follow-up
- Add typed validation (int/bool/url).
- Add warnings for keys that look like secrets but are too short.

---

## Problem 13 — Merge Multi-layer .env Config

**Difficulty:** Easy  
**Topic Tags:** `Scripting` `Config` `DevOps`

### Description
Your deployment uses layered environment files (`base`, `shared`, `prod`, `override`).  
Later layers must override earlier values for the same key.  
Implement merge function with precedence order based on list position.  
Preserve first-seen key order in final output for readability.  
Return merged dictionary only (no file parsing needed here).  
This simulates config resolution in real deployment tooling.  
Be careful with key collisions.

### Input
- `layers: List[Dict[str,str]]` (low to high precedence)

### Output
- `Dict[str,str]`

### Constraints
- Up to `10^5` total keys across layers

### Example
**Input**
- `layers = [{"A":"1","B":"1"}, {"B":"2"}, {"C":"3","A":"9"}]`

**Output**
- `{"A":"9","B":"2","C":"3"}`

**Explanation**  
Latest layer overrides previous values while key display order follows first appearance: A, B, C.

### Follow-up
- Return provenance map (`key -> layer_index`).
- Detect conflicting keys marked as immutable.

---

## Problem 14 — Safe Command Runner

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Subprocess` `Security`

### Description
You need a reusable helper to run system commands in automation scripts safely.  
Input can come from users/configs, so shell injection risks must be minimized.  
Implement command execution using argument list, timeout, captured stdout/stderr, and return code.  
If timeout happens, kill process and report timed-out status.  
Never use unsafe interpolation patterns in implementation.  
Return a structured result object suitable for logging and retries.  
This is a high-frequency interview topic for DevOps scripting.

### Input
- `cmd: List[str]`
- `timeout_sec: int`
- `simulated: {stdout, stderr, returncode, runtime_sec}`

### Output
- `{stdout, stderr, returncode, timed_out}`

### Constraints
- `0 <= timeout_sec <= 3600`

### Example
**Input**
- `cmd = ["python","--version"]`
- `timeout_sec = 2`
- `runtime_sec = 0.1, returncode=0`

**Output**
- `{"stdout":"Python 3.x", "stderr":"", "returncode":0, "timed_out":False}`

**Explanation**  
Command finished within timeout; output is captured and returned.

### Follow-up
- Stream stdout line-by-line via callback.
- Add redaction of secrets before logging.

---

## Problem 15 — Tail Last N Lines Without Full Read

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Files` `Algorithms`

### Description
An on-call engineer frequently needs the last lines of huge logs (>50GB).  
Implement `tail(file, n)` efficiently without loading whole file into RAM.  
Use backward block reads and split by newline.  
Return exactly last `n` lines, preserving order.  
Handle files that may not end with newline.  
This is a classic systems scripting interview problem.  
Focus on I/O efficiency and correctness.

### Input
- `lines: List[str]` (simulated file content)
- `n: int`

### Output
- `List[str]` (last `n` lines)

### Constraints
- `1 <= n <= 10^6`
- `1 <= total_lines <= 10^7`

### Example
**Input**
- `lines = ["a","b","c","d","e"]`
- `n = 2`

**Output**
- `["d","e"]`

**Explanation**  
Return only the final two lines in same order.

### Follow-up
- Add follow mode (`tail -f`) semantics.
- Support mixed line endings (`\n`, `\r\n`).

---

## Problem 16 — Parse `df -h` and Raise Disk Alerts

**Difficulty:** Easy  
**Topic Tags:** `Scripting` `Linux` `Parsing` `Monitoring`

### Description
You receive `df -h` output from remote hosts and need automated disk alerts.  
Parse rows and identify mounts whose used percentage exceeds threshold.  
Ignore pseudo filesystems if configured.  
Return alert tuples with mount and used percentage.  
The script is part of a daily health check job.  
Input formatting may have multiple spaces between columns.  
This tests robust text parsing in ops contexts.

### Input
- `rows: List[(filesystem, size, used, avail, use_pct, mount)]`
- `threshold: int`

### Output
- `List[(mount:str, used_percent:int)]`

### Constraints
- `0 <= threshold <= 100`

### Example
**Input**
- `rows = [("dev/sda1","100G","91G","9G","91%","/"), ("tmpfs","1G","0G","1G","0%","/run")]`
- `threshold = 80`

**Output**
- `[("/", 91)]`

**Explanation**  
Only root mount crosses 80%.

### Follow-up
- Parse POSIX format (`df -P`) for better portability.
- Include inode usage (`df -i`) checks.

---

## Problem 17 — Parse `ps` Snapshot and Aggregate

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Processes` `Linux` `Parsing`

### Description
During incidents you often paste `ps aux` output and quickly need summaries.  
Implement parser that returns top 5 processes by CPU, top 5 by memory, and process count by user.  
Assume command column may contain spaces and should be captured fully.  
Handle variable spacing between columns robustly.  
If fewer than 5 processes exist, return all.  
This problem mimics practical Linux troubleshooting tasks.  
You may define explicit expected column positions.

### Input
- `procs: List[(user,pid,cpu,mem,command)]`

### Output
- `{top_cpu: List[pid], top_mem: List[pid], count_by_user: Dict[user,int]}`

### Constraints
- `1 <= len(procs) <= 10^6`

### Example
**Input**
- `procs = [("root",1,0.1,0.2,"init"),("app",10,50.0,10.0,"python a.py"),("app",11,40.0,12.0,"python b.py")]`

**Output**
- `top_cpu = [10,11,1]`
- `top_mem = [11,10,1]`
- `count_by_user = {"root":1,"app":2}`

**Explanation**  
Sorting is based on numeric CPU/MEM values, descending.

### Follow-up
- Add zombie process detection.
- Add regex filter on command name.

---

## Problem 18 — Generate Retry Plan from Failed Job Log

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Automation` `Regex` `Reliability`

### Description
A batch pipeline executes many shell commands and logs `CMD=... STATUS=...`.  
When run fails, operators manually copy failed commands and rerun them.  
Automate this by extracting failed commands in original order.  
Generate a retry script template that retries each command up to `R` times with fixed delay.  
Optionally deduplicate exact duplicate failed commands.  
Return generated script text as output.  
This reflects realistic DevOps recovery scripting.

### Input
- `records: List[(cmd:str, status:str)]`
- `max_retries: int`
- `delay_sec: int`
- `dedup: bool`

### Output
- `str` (script content)

### Constraints
- `0 <= max_retries <= 20`
- `0 <= delay_sec <= 3600`

### Example
**Input**
- `records = [("echo ok","SUCCESS"),("python sync.py","FAIL"),("curl x","FAIL")]`
- `max_retries = 2`
- `delay_sec = 5`
- `dedup = False`

**Output**
- Script that retries `python sync.py` and `curl x` up to 2 times each.

**Explanation**  
Only failed entries are included; success lines are skipped.

### Follow-up
- Add checkpointing to resume partially retried batch.
- Add per-command timeout setting.

---

## Problem 19 — Concurrent Host Executor (Simulated SSH Fan-out)

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Concurrency` `Automation` `Infra`

### Description
You manage 200 servers and need to run one diagnostic command across all hosts.  
Sequential execution is too slow, so implement bounded parallel execution.  
Given host outcomes and durations (simulated), process at most `max_parallel` concurrently.  
Collect per-host result: success/failure, output, and elapsed time.  
Return results in original host order for easy reading.  
Include timeout handling per host.  
This is a common SRE automation interview pattern.

### Input
- `hosts: List[str]`
- `max_parallel: int`
- `host_result: Dict[host -> (success:bool, duration_sec:float)]`
- `timeout_sec: float`

### Output
- `List[(host, success, timed_out, duration)]`

### Constraints
- `1 <= len(hosts) <= 10^5`
- `1 <= max_parallel <= 1000`

### Example
**Input**
- `hosts=["h1","h2","h3"]`, `max_parallel=2`, `timeout_sec=3`
- `host_result={"h1":(True,1), "h2":(False,2), "h3":(True,5)}`

**Output**
- `[("h1",True,False,1), ("h2",False,False,2), ("h3",False,True,5)]`

**Explanation**  
`h3` exceeds timeout and is marked timed out.

### Follow-up
- Stop whole rollout if failures exceed threshold.
- Add retry only for network-related failures.

---

## Problem 20 — YAML Deployment Rule Validator

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `YAML` `Validation` `CI`

### Description
Your CI pipeline checks deployment YAML quality before merge.  
Implement custom validation rules on parsed YAML docs.  
Ensure required keys exist (name/image/resources), service names are unique, and resource limits are present.  
Return all validation errors, not just first failure.  
Input may contain multiple YAML documents in one file.  
Error messages should be human-readable for developers.  
This problem reflects policy-as-code checks.

### Input
- `docs: List[dict]` (already parsed YAML docs)

### Output
- `List[str]` error messages

### Constraints
- Up to `10^4` documents

### Example
**Input**
- `docs=[{"service":"api","image":"x","resources":{"requests":{"cpu":"100m"}}}]`

**Output**
- `["service api: missing resources.limits"]`

**Explanation**  
Requests exist but limits are required and missing.

### Follow-up
- Validate against JSON Schema.
- Auto-fix simple defaults (with explicit flag).

---

## Problem 21 — Flatten Nested JSON for Labeling

**Difficulty:** Easy  
**Topic Tags:** `Scripting` `JSON` `Prometheus`

### Description
Metric enrichment script receives deeply nested metadata objects.  
For label extraction, you need flat dot-notated keys.  
Implement recursive flattening for dicts and arrays.  
Array items should use numeric indices in key path.  
Keep primitive values as-is in output map.  
This utility is reused in log-to-metrics adapters.  
Ignore non-serializable values in this version.

### Input
- `obj: dict`

### Output
- `Dict[str, primitive]`

### Constraints
- Depth up to 100
- Total flattened keys up to `10^6`

### Example
**Input**
- `{"a":{"b":1},"arr":[{"x":2}]}`

**Output**
- `{"a.b":1, "arr.0.x":2}`

**Explanation**  
Nested object and array index are converted to dot paths.

### Follow-up
- Add include/exclude key prefixes.
- Add max-depth guard for malformed data.

---

## Problem 22 — Secret Leak Scanner

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Security` `Regex` `CI`

### Description
Your pre-commit hook must detect accidentally committed secrets.  
Scan file contents for patterns like AWS access keys, private key blocks, and long high-entropy tokens.  
Report findings with file path, line number, rule ID, and short snippet.  
Allow lines marked with explicit ignore annotation to be skipped.  
Goal is to catch obvious leaks while reducing false positives.  
Return all matches across all files.  
This is a practical DevSecOps interview problem.

### Input
- `files: Dict[str, List[str]]` (`path -> lines`)

### Output
- `List[(path, line_no, rule_id, snippet)]`

### Constraints
- Up to `10^5` files
- Up to `10^8` total characters

### Example
**Input**
- `files = {"a.txt":["token=AKIA1234567890ABCD"], "b.txt":["safe"]}`

**Output**
- `[("a.txt",1,"AWS_ACCESS_KEY","AKIA1234...")]`

**Explanation**  
Line in `a.txt` matches AWS key rule.

### Follow-up
- Add entropy-based generic token detector.
- Support baseline file for accepted false positives.

---

## Problem 23 — Service Startup Order (Topological Sort)

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Graphs` `Automation`

### Description
You are building a local orchestrator for microservices.  
Some services depend on others (e.g., `api` depends on `db`).  
Compute a valid startup order such that dependencies start first.  
If dependencies contain a cycle, report it as invalid config.  
Return one valid order if multiple exist.  
This is a common graph problem with direct DevOps relevance.  
Use topological sorting.

### Input
- `deps: Dict[str, List[str]]` (service -> required services)

### Output
- `List[str]` or `"CYCLE"`

### Constraints
- `1 <= number_of_services <= 10^5`

### Example
**Input**
- `{"web":["api"], "api":["db"], "db":[]}`

**Output**
- `["db","api","web"]`

**Explanation**  
Each service appears after all its prerequisites.

### Follow-up
- Return all services that can start in parallel at each level.
- Include cycle path if cycle exists.

---

## Problem 24 — Rolling Deployment Simulator

**Difficulty:** Hard  
**Topic Tags:** `Scripting` `SRE` `Reliability` `Simulation`

### Description
You are simulating rollout safety for a service with `N` instances.  
Deployment proceeds in batches of size `B`.  
Each instance update can succeed/fail based on provided outcomes.  
If total failures exceed rollback threshold, deployment aborts and rolls back.  
Return timeline of each batch with counts and final status.  
This helps interviewers test control-flow design under operational rules.  
Assume deterministic outcomes are provided in input.

### Input
- `N: int`
- `B: int`
- `outcomes: List[bool]` length `N` (True=success)
- `max_failures: int`

### Output
- `{batches: List[(updated, failed)], final_status: "SUCCESS"|"ROLLBACK"}`

### Constraints
- `1 <= B <= N <= 10^6`
- `0 <= max_failures <= N`

### Example
**Input**
- `N=6, B=2, outcomes=[T,T,F,T,F,T], max_failures=1`

**Output**
- `batches=[(2,0),(2,1),(2,1)]`
- `final_status="ROLLBACK"`

**Explanation**  
After third batch, cumulative failures become 2, exceeding allowed 1.

### Follow-up
- Add canary phase before full rollout.
- Add pause/resume checkpoints.

---

## Problem 25 — SLO Burn Rate Calculator

**Difficulty:** Hard  
**Topic Tags:** `Scripting` `SRE` `Metrics` `Algorithms`

### Description
Your SRE dashboard computes error-budget burn rates from request/error counters.  
Given per-minute totals and errors, compute burn rate over short and long windows.  
Burn rate compares observed error ratio against allowed error ratio (`1 - SLO`).  
Output burn rate per timestamp for each window where enough data exists.  
Handle missing data points by skipping incomplete windows.  
This problem mirrors real multi-window alert logic.  
Numerical precision should be stable.

### Input
- `points: List[(ts:int,total:int,errors:int)]`
- `slo_target: float` (e.g., `99.9`)
- `windows: List[int]` minutes (e.g., `[5,60]`)

### Output
- `Dict[window -> List[(ts,burn_rate)]]`

### Constraints
- `0 <= errors <= total`
- `1 <= len(points) <= 10^6`

### Example
**Input**
- `slo_target=99.0`, allowed error ratio `0.01`
- 5-minute window sum: total=1000, errors=20

**Output**
- burn rate for that point = `0.02 / 0.01 = 2.0`

**Explanation**  
Service is burning error budget 2x faster than allowed.

### Follow-up
- Trigger alerts using common multi-burn thresholds.
- Use rolling counters efficiently for O(n).

---

## Problem 26 — Alert Dedup with Cooldown

**Difficulty:** Easy  
**Topic Tags:** `Scripting` `Alerting` `SRE`

### Description
Your notifier floods Slack with identical alerts every few seconds.  
To reduce noise, suppress repeated alerts with same fingerprint inside cooldown interval.  
Given sorted events, emit only those not suppressed.  
Maintain independent cooldown per fingerprint.  
Return emitted alerts in original order.  
This is a practical anti-noise mechanism for alert routing.  
Do not modify timestamps.

### Input
- `events: List[(fingerprint:str, ts:int)]` sorted by `ts`
- `cooldown_sec: int`

### Output
- `List[(fingerprint, ts)]`

### Constraints
- `0 <= cooldown_sec <= 10^6`
- `1 <= len(events) <= 10^6`

### Example
**Input**
- `events=[("A",0),("A",30),("A",130),("B",131)]`
- `cooldown_sec=120`

**Output**
- `[("A",0),("A",130),("B",131)]`

**Explanation**  
Second `A` is within 120s of first and is suppressed.

### Follow-up
- Cooldown by severity level.
- Add “max suppress duration” then force re-notify.

---

## Problem 27 — Token Bucket Rate Limiter

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Algorithms` `Reliability`

### Description
You need to protect an internal API from burst traffic.  
Implement token bucket with capacity `C` and refill rate `R` tokens/sec.  
Each request consumes one token if available; otherwise it is denied.  
Given request timestamps, output allow/deny decisions in order.  
State updates should be O(1) per request.  
Use floating-point or integer arithmetic carefully.  
This tests foundational rate-limiting logic.

### Input
- `capacity: int`
- `refill_per_sec: float`
- `request_ts: List[float]` sorted

### Output
- `List[bool]`

### Constraints
- `1 <= capacity <= 10^6`
- `0 < refill_per_sec <= 10^6`
- `1 <= len(request_ts) <= 10^6`

### Example
**Input**
- `capacity=2, refill_per_sec=1`
- `request_ts=[0.0,0.1,0.2,1.2]`

**Output**
- `[True, True, False, True]`

**Explanation**  
First two consume initial tokens, third denied, by `1.2s` one token refills.

### Follow-up
- Distributed token bucket using Redis.
- Different costs per request type.

---

## Problem 28 — Nginx Log Latency Percentiles

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Logs` `Statistics`

### Description
Your team needs quick latency stats from Nginx access logs during incidents.  
Each parsed record includes HTTP status code and request time in seconds.  
Compute p50, p95, p99 latency and overall error rate (status >= 500).  
Ignore malformed rows.  
Return metrics in a dictionary with numeric values.  
For simplicity, exact percentile by sorting is acceptable.  
This is a common observability scripting task.

### Input
- `records: List[(status:int, req_time:float)]`

### Output
- `{p50:float, p95:float, p99:float, error_rate:float}`

### Constraints
- `1 <= len(records) <= 10^6`
- `0 <= req_time <= 3600`

### Example
**Input**
- `[(200,0.10),(200,0.20),(500,1.20),(502,0.80),(200,0.15)]`

**Output**
- `error_rate = 2/5 = 0.4` and corresponding percentiles from sorted latencies

**Explanation**  
Two of five responses are 5xx.

### Follow-up
- Approximate percentiles with t-digest/histogram.
- Compute per-endpoint percentiles.

---

## Problem 29 — Syslog Severity Distribution

**Difficulty:** Easy  
**Topic Tags:** `Scripting` `Logs` `Linux`

### Description
You are given parsed syslog lines with severity labels.  
Produce distribution counts for standard levels: DEBUG, INFO, WARN, ERROR, CRITICAL.  
Severity matching should be case-insensitive.  
Unknown labels can be ignored or counted as `OTHER` (choose one and document).  
Return counts in fixed severity order.  
This is a straightforward parser + counter task.  
Useful as interview warm-up.

### Input
- `levels: List[str]`

### Output
- `Dict[str,int]`

### Constraints
- `1 <= len(levels) <= 10^7`

### Example
**Input**
- `["info","ERROR","Warn","error","DEBUG"]`

**Output**
- `{"DEBUG":1,"INFO":1,"WARN":1,"ERROR":2,"CRITICAL":0}`

**Explanation**  
Case-insensitive normalization maps labels to canonical forms.

### Follow-up
- Break down by service name as second dimension.
- Add hourly time buckets.

---

## Problem 30 — Stale PID File Watchdog

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Processes` `Reliability`

### Description
A legacy daemon uses PID files and often crashes, leaving stale files behind.  
On restart, stale PID file blocks service startup.  
Given PID-file content and process table snapshot, decide action:
`OK`, `REMOVE_STALE`, or `ALERT`.  
If PID does not exist, file is stale and removable.  
If PID exists but command does not match expected pattern, escalate `ALERT`.  
This reflects real operational hardening.  
Never kill unrelated processes in this task.

### Input
- `pid_in_file: int or None`
- `processes: Dict[int, str]` (`pid -> command`)
- `expected_pattern: str` (regex)

### Output
- `"OK" | "REMOVE_STALE" | "ALERT"`

### Constraints
- PID values in `1..10^7`

### Example
**Input**
- `pid_in_file=1234`
- `processes={5678:"daemon --start"}`
- `expected_pattern="daemon"`

**Output**
- `"REMOVE_STALE"`

**Explanation**  
PID 1234 not present in process table, so lock is stale.

### Follow-up
- Add stale age check before auto-removal.
- Add heartbeat file validation.

---

## Problem 31 — Stacktrace Multi-line Grouper

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Logs` `Parsing`

### Description
Many log collectors ingest line-by-line and split stacktraces incorrectly.  
You need to merge related lines into one event.  
Given raw lines, identify start of a new event and append continuation lines until next start.  
For this problem, treat lines starting with timestamp as new events; indented lines are continuations.  
Return grouped events as concatenated strings.  
Preserve original order and content.  
This is crucial for searchable error analysis.

### Input
- `lines: List[str]`

### Output
- `List[str]` grouped events

### Constraints
- `1 <= len(lines) <= 10^6`

### Example
**Input**
- `["2025-01-01 ERROR x","  at a()","  at b()","2025-01-01 INFO ok"]`

**Output**
- `["2025-01-01 ERROR x\n  at a()\n  at b()", "2025-01-01 INFO ok"]`

**Explanation**  
Indented lines are attached to previous event.

### Follow-up
- Support multiple start-patterns (JSON/plain text).
- Emit event metadata with line ranges.

---

## Problem 32 — CSV to Prometheus Exposition Converter

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Prometheus` `Metrics`

### Description
A legacy batch job exports metrics as CSV and must feed Prometheus text format.  
Implement converter using mapping config: metric name, label columns, and value column.  
For each row, produce one Prometheus sample line.  
Escape label values according to exposition rules.  
Skip rows with invalid numeric values.  
Return final exposition text lines.  
This is a practical adapter-building exercise.

### Input
- `rows: List[Dict[str,str]]`
- `metric_name: str`
- `label_cols: List[str]`
- `value_col: str`

### Output
- `List[str]`

### Constraints
- Up to `10^6` rows
- Value must parse as float

### Example
**Input**
- `rows=[{"host":"a","cpu":"72.5"},{"host":"b","cpu":"80"}]`
- `metric_name="node_cpu_usage"`
- `label_cols=["host"]`
- `value_col="cpu"`

**Output**
- `['node_cpu_usage{host="a"} 72.5', 'node_cpu_usage{host="b"} 80']`

**Explanation**  
Each CSV row becomes one gauge sample.

### Follow-up
- Generate `_count` and `_sum` companion metrics.
- Add HELP/TYPE headers.

---

## Problem 33 — Kubernetes Pod Restart Analyzer

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Kubernetes` `Ops`

### Description
On-call receives frequent pod restart alerts and wants quick triage summary.  
Given parsed pod status objects, report containers whose restart count exceeds threshold.  
For each finding, include namespace, pod, container, restart count, and last termination reason.  
If a pod has multiple containers, evaluate each separately.  
Return findings sorted by restart count descending.  
This script is used in post-deploy checks.  
Handle missing optional fields gracefully.

### Input
- `pods: List[dict]`
- `threshold: int`

### Output
- `List[(ns,pod,container,restarts,last_reason)]`

### Constraints
- `0 <= threshold <= 10^6`
- Up to `10^5` pods

### Example
**Input**
- One pod `api-1` with containers: `app restarts=5 reason=OOMKilled`, `sidecar restarts=0`
- `threshold=3`

**Output**
- `[("prod","api-1","app",5,"OOMKilled")]`

**Explanation**  
Only container restart count above threshold is included.

### Follow-up
- Aggregate by namespace/team.
- Flag patterns like crash loops after deployment time.

---

## Problem 34 — Kubernetes Event Noise Filter

**Difficulty:** Easy  
**Topic Tags:** `Scripting` `Kubernetes` `Alerting`

### Description
Cluster event streams include many repetitive benign events (e.g., image pulling).  
You need a filter that keeps only meaningful anomalies for operators.  
Given events with reason/message/count, drop known low-signal reasons.  
Also drop high-frequency duplicates beyond configurable count threshold.  
Return condensed event list sorted by recency.  
This reduces alert fatigue in operational channels.  
Rules should be configurable.

### Input
- `events: List[(reason:str, message:str, count:int, ts:int)]`
- `ignore_reasons: Set[str]`
- `max_repeat: int`

### Output
- `List[(reason,message,count,ts)]`

### Constraints
- `1 <= len(events) <= 10^6`

### Example
**Input**
- events: `Pulling(count=50)`, `BackOff(count=3)`
- `ignore_reasons={"Pulling"}`
- `max_repeat=10`

**Output**
- only `BackOff` event

**Explanation**  
`Pulling` is explicitly ignored as noise.

### Follow-up
- Learn noise baselines automatically from history.
- Group similar messages by template.

---

## Problem 35 — Dockerfile Basic Linter

**Difficulty:** Medium  
**Topic Tags:** `Docker` `Scripting` `Security` `Best Practices`

### Description
Your CI must fail risky Dockerfiles before image build.  
Implement linter checks: avoid `latest` tags, prefer non-root user, and discourage unnecessary layer bloat patterns.  
Input is tokenized Dockerfile instructions with line numbers.  
Return all violations with severity and suggestion.  
Do not modify file in this problem.  
This mirrors practical DevSecOps gatekeeping.  
Clear messaging matters for developer adoption.

### Input
- `instructions: List[(line_no:int, instr:str)]`

### Output
- `List[(line_no, rule_id, severity, message)]`

### Constraints
- `1 <= len(instructions) <= 10^5`

### Example
**Input**
- `[(1,"FROM python:latest"), (2,"RUN apt-get update"), (3,"USER root")]`

**Output**
- include issue for `FROM ...:latest`
- include issue for running as root (if no later non-root USER)

**Explanation**  
Both base image pinning and least-privilege rules are violated.

### Follow-up
- Add autofix recommendations.
- Add allowlist for controlled exceptions.

---

## Problem 36 — Docker Image Reference Parser

**Difficulty:** Easy  
**Topic Tags:** `Docker` `Scripting` `Parsing`

### Description
Your deployment script accepts Docker image references in different forms.  
Parse each into components: registry, repository, optional tag, optional digest.  
Apply Docker Hub defaults when registry is omitted.  
Support references like `nginx`, `nginx:1.25`, `ghcr.io/org/app:2`, `repo@sha256:...`.  
Return normalized structured output.  
This task checks careful string parsing under real conventions.  
Invalid formats should return explicit error.

### Input
- `image_ref: str`

### Output
- `{registry, repository, tag, digest}` or error

### Constraints
- Length up to 2048 chars

### Example
**Input**
- `"nginx:1.25"`

**Output**
- `{"registry":"docker.io","repository":"library/nginx","tag":"1.25","digest":null}`

**Explanation**  
Default registry and `library/` namespace are applied.

### Follow-up
- Validate semantic-version tags.
- Support normalization for private registries with ports.

---

## Problem 37 — Parse `docker ps -a` Report

**Difficulty:** Medium  
**Topic Tags:** `Docker` `Scripting` `Parsing` `Ops`

### Description
A cleanup job receives container inventory snapshot and must summarize risks.  
Given parsed rows from `docker ps -a`, report:
1) exited containers older than `X` days,  
2) containers in restarting state,  
3) most-used images by container count.  
Return a structured report used by ops dashboards.  
Preserve exact container IDs/names in output.  
This reflects practical fleet hygiene automation.

### Input
- `containers: List[(id,name,image,status,age_days)]`
- `x_days: int`

### Output
- `{old_exited: List[name], restarting: List[name], image_usage: List[(image,count)]}`

### Constraints
- `1 <= len(containers) <= 10^6`

### Example
**Input**
- `[("1","a","img1","Exited",40),("2","b","img1","Up",2),("3","c","img2","Restarting",1)]`
- `x_days=30`

**Output**
- `old_exited=["a"]`
- `restarting=["c"]`
- `image_usage=[("img1",2),("img2",1)]`

**Explanation**  
Container `a` is exited older than threshold; `c` is in restart loop.

### Follow-up
- Prefer machine-readable CLI formats in production.
- Include reclaimable disk estimate.

---

## Problem 38 — Docker Compose Dependency Validator

**Difficulty:** Medium  
**Topic Tags:** `Docker` `Compose` `Scripting` `Validation`

### Description
A platform team maintains many Compose files with cross-service dependencies.  
Invalid dependency references and cycles cause startup failures.  
Given parsed compose config, verify every `depends_on` target exists.  
Detect cycles in dependency graph.  
Also ensure required env keys exist for each service profile.  
Return all validation errors together.  
This resembles real CI validation for Compose projects.

### Input
- `services: Dict[str, {depends_on: List[str], env: Dict[str,str]}]`
- `required_env: Dict[str, List[str]]` (`service -> required keys`)

### Output
- `List[str]`

### Constraints
- Up to `10^5` services

### Example
**Input**
- `services={"web":{"depends_on":["api"]},"api":{"depends_on":["db"]},"db":{"depends_on":["web"]}}`

**Output**
- `["dependency cycle detected: web -> api -> db -> web"]`

**Explanation**  
Graph contains a cycle, so startup order is invalid.

### Follow-up
- Simulate start levels with healthcheck readiness.
- Validate resource limits per service.

---

## Problem 39 — Generate 3-Tier Compose Template

**Difficulty:** Medium  
**Topic Tags:** `Docker` `Compose` `Automation`

### Description
Given high-level app requirements, generate a minimal valid Compose model.  
You must include web, api, and db services, a shared network, and persistent db volume.  
Inject environment variable placeholders instead of real secrets.  
Set restart policies suitable for local resilience.  
Return generated YAML text (or structured dict in tests).  
This tests ability to translate requirements into deployment config.  
Keep output concise and production-aware.

### Input
- `req = {web_image, api_image, db_image, db_port, api_port, web_port}`

### Output
- `compose_config` (YAML string or dict)

### Constraints
- No hardcoded credentials
- Must include named volume for DB

### Example
**Input**
- `web_image="nginx:1.25", api_image="myapi:2", db_image="postgres:16"`

**Output**
- compose with services `web/api/db`, network `app_net`, volume `db_data`

**Explanation**  
All mandatory architectural components are present.

### Follow-up
- Add dev/prod profiles.
- Add healthchecks and dependency conditions.

---

## Problem 40 — Container Log Router by Labels

**Difficulty:** Medium  
**Topic Tags:** `Docker` `Logging` `Scripting` `Loki`

### Description
You ingest logs from many containers into different backends.  
Routing depends on container labels (`tier=system` vs `tier=app`).  
Implement router that maps each log line to sink A or B.  
Respect denylist for noisy containers that should be dropped.  
Preserve ordering per container stream.  
Return routed records grouped by sink.  
This mirrors production log pipeline routing.

### Input
- `records: List[(container_id:str, labels:Dict[str,str], line:str)]`
- `denylist: Set[str]`

### Output
- `{sink_a: List[record], sink_b: List[record], dropped: int}`

### Constraints
- `1 <= len(records) <= 10^7`

### Example
**Input**
- records from `c1{tier=system}`, `c2{tier=app}`, and `c3` in denylist

**Output**
- `c1` lines routed to `sink_a`, `c2` to `sink_b`, `c3` dropped

**Explanation**  
Routing is label-driven with explicit suppression for noisy containers.

### Follow-up
- Add disk buffering on sink outage.
- Add backpressure metrics.

---

## Problem 41 — Jenkinsfile Timeout Policy Enforcer

**Difficulty:** Medium  
**Topic Tags:** `Jenkins` `CI/CD` `Scripting` `Groovy`

### Description
Long-hanging CI stages waste agents and delay releases.  
Given parsed Jenkins pipeline stages, ensure each stage has timeout config.  
If missing, inject default timeout value.  
Do not overwrite existing custom timeout values.  
Return transformed stage config and list of modified stages.  
This enforces organizational CI policy programmatically.  
Formatting preservation is optional in this version.

### Input
- `stages: List[{name:str, timeout_min: int or None}]`
- `default_timeout_min: int`

### Output
- `{updated_stages, modified_stage_names}`

### Constraints
- `1 <= default_timeout_min <= 720`
- `1 <= len(stages) <= 10^4`

### Example
**Input**
- `stages=[{"name":"build","timeout_min":None},{"name":"test","timeout_min":30}]`
- `default_timeout_min=20`

**Output**
- build gets 20, test remains 30
- modified = `["build"]`

**Explanation**  
Only missing timeout fields are injected.

### Follow-up
- Also enforce retry policy for flaky stages.
- Enforce stage-level `post` cleanup blocks.

---

## Problem 42 — Jenkins Build Failure Classifier

**Difficulty:** Medium  
**Topic Tags:** `Jenkins` `Logs` `Scripting` `Regex`

### Description
You want faster triage for failed Jenkins builds.  
Implement rule-based classifier that maps build logs to categories:
`test_failure`, `dependency_failure`, `agent_offline`, `timeout`, or `unknown`.  
Use keyword/regex matching on log text.  
If multiple categories match, pick one with highest priority order.  
Return category and confidence score between 0 and 1.  
This enables automated incident routing to right team.  
Rules should be easy to extend.

### Input
- `build_log: str`

### Output
- `(category: str, confidence: float)`

### Constraints
- Log length up to `10^7` chars

### Example
**Input**
- log contains: `"There are test failures"` and no other category keywords

**Output**
- `("test_failure", 0.9)`

**Explanation**  
Direct high-signal keyword maps to test failure class.

### Follow-up
- Return top-2 categories for ambiguity.
- Attach remediation hints.

---

## Problem 43 — Jenkins Queue Time Analytics

**Difficulty:** Easy  
**Topic Tags:** `Jenkins` `Scripting` `Metrics`

### Description
Build latency includes queue delay before execution starts.  
Given build timing metadata, compute average queue time and p95 queue time.  
Also return top `k` builds with highest queue delay.  
Ignore records with invalid timestamp order.  
This is frequently used to identify underprovisioned CI agents.  
Output should be deterministic and sorted.  
Assume timestamps are epoch seconds.

### Input
- `builds: List[(id:str, queued:int, started:int, ended:int)]`
- `k: int`

### Output
- `{avg_queue: float, p95_queue: float, top_waiting: List[id]}`

### Constraints
- `1 <= len(builds) <= 10^6`
- `1 <= k <= len(valid_builds)`

### Example
**Input**
- `b1: queued=0,start=10`
- `b2: queued=0,start=30`
- `b3: queued=0,start=5`, `k=2`

**Output**
- `avg_queue = 15`
- `p95_queue = 30`
- `top_waiting = ["b2","b1"]`

**Explanation**  
Queue delays are 10,30,5 seconds.

### Follow-up
- Group metrics by agent label.
- Separate queueing due to blocked downstream jobs.

---

## Problem 44 — Prometheus Exposition Parser

**Difficulty:** Medium  
**Topic Tags:** `Prometheus` `Scripting` `Parsing` `Metrics`

### Description
You receive raw Prometheus text and need structured samples for custom checks.  
Parse lines of the form `metric{labels} value [timestamp]`.  
Ignore comment/meta lines (`# HELP`, `# TYPE`).  
Extract metric name, labels map, float value, optional timestamp.  
Return all parsed samples in input order.  
Malformed sample lines should be skipped with optional warning count.  
This is a practical interoperability utility.

### Input
- `text_lines: List[str]`

### Output
- `List[{name, labels, value, ts?}]`

### Constraints
- Up to `10^7` lines

### Example
**Input**
- `["# HELP x", 'http_requests_total{code="500"} 12', 'up 1']`

**Output**
- `[{"name":"http_requests_total","labels":{"code":"500"},"value":12}, {"name":"up","labels":{},"value":1}]`

**Explanation**  
Comment line is ignored; two metric samples are parsed.

### Follow-up
- Re-emit canonical sorted-label format.
- Validate metric naming conventions.

---

## Problem 45 — PromQL Query Template Builder

**Difficulty:** Easy  
**Topic Tags:** `Prometheus` `Scripting` `Observability`

### Description
Engineers manually write error-rate queries and often make syntax mistakes.  
Create builder that outputs standard PromQL templates for request rate, error rate, and availability.  
Given metric names, label filters, and window duration, return query strings.  
Ensure label values are properly escaped in generated query.  
This task is mostly string composition with correctness checks.  
Used by internal dashboard generators.  
Return deterministic formatting for stable diffs.

### Input
- `req_metric: str`
- `err_metric: str`
- `labels: Dict[str,str]`
- `window: str` (e.g., `"5m"`)

### Output
- `{req_rate_query, err_rate_query, availability_query}`

### Constraints
- Window format is guaranteed valid

### Example
**Input**
- `req_metric="http_requests_total"`
- `err_metric="http_requests_total{code=~\"5..\"}"` *(or separate metric in your design)*
- `labels={"service":"api"}`
- `window="5m"`

**Output**
- queries with `rate(...[5m])` and ratio expressions

**Explanation**  
Builder standardizes frequently reused formulas.

### Follow-up
- Add latency percentile query templates.
- Add namespace/service default filters.

---

## Problem 46 — Prometheus Alert Rule Checker

**Difficulty:** Medium  
**Topic Tags:** `Prometheus` `Alerting` `Scripting`

### Description
You are reviewing alert rule quality before deployment.  
Validate each rule has `for` duration, `severity` label, and `summary` annotation.  
Detect duplicate alert names within same rule group.  
Return all violations with group/rule context.  
Input is parsed YAML-like structure.  
This script helps reduce noisy/low-quality alerts in production.  
No PromQL evaluation required.

### Input
- `groups: List[{name:str, rules: List[rule]}]`
- each `rule` has `{alert, expr, for?, labels?, annotations?}`

### Output
- `List[str]`

### Constraints
- Up to `10^5` rules

### Example
**Input**
- rule `HighErrorRate` missing `severity`

**Output**
- `["group api: alert HighErrorRate missing label 'severity'"]`

**Explanation**  
Severity is mandatory by policy.

### Follow-up
- Check for too-short `for` durations.
- Require runbook URL annotation.

---

## Problem 47 — Grafana Dashboard Panel Auditor

**Difficulty:** Medium  
**Topic Tags:** `Grafana` `Scripting` `JSON` `Observability`

### Description
Dashboard drift causes broken panels and duplicate titles.  
Given parsed Grafana dashboard JSON, audit for common issues.  
Check panel title uniqueness, datasource reference validity, and required dashboard tags.  
Return issue list with panel IDs where possible.  
Nested rows/panels should be traversed recursively.  
This script acts as CI quality gate for dashboards-as-code.  
Do not mutate dashboard in this problem.

### Input
- `dashboard: dict`
- `valid_datasources: Set[str]`
- `required_tags: Set[str]`

### Output
- `List[str]`

### Constraints
- Up to `10^5` panels

### Example
**Input**
- two panels named `"CPU"`, one panel with datasource `"unknown"`

**Output**
- issues for duplicate title and invalid datasource

**Explanation**  
Both naming and datasource policies are violated.

### Follow-up
- Auto-fix title duplicates by suffixing index.
- Validate panel queries are non-empty.

---

## Problem 48 — Loki LogQL Constructor

**Difficulty:** Easy  
**Topic Tags:** `Loki` `Grafana` `Scripting` `Logs`

### Description
Operators often build LogQL queries manually and forget escaping rules.  
Build a function that composes query from selector labels, include terms, and exclude terms.  
Start with stream selector, then append filter operators in order.  
Support exact match filters for this version.  
Return final query string suitable for Grafana Explore.  
This utility standardizes query generation in tooling.  
Ensure deterministic output ordering of label keys.

### Input
- `selectors: Dict[str,str]`
- `include_terms: List[str]`
- `exclude_terms: List[str]`

### Output
- `str` LogQL query

### Constraints
- Up to 100 selector labels and 100 terms each

### Example
**Input**
- `selectors={"app":"api","env":"prod"}`
- `include_terms=["error"]`
- `exclude_terms=["health"]`

**Output**
- `{app="api",env="prod"} |= "error" != "health"`

**Explanation**  
Selector is built first, then include and exclude filters are appended.

### Follow-up
- Add regex filters.
- Generate metric mode query (`count_over_time`) option.

---

## Problem 49 — Loki Label Cardinality Risk Estimator

**Difficulty:** Medium  
**Topic Tags:** `Loki` `Scripting` `Observability` `Design`

### Description
High-cardinality labels in Loki can dramatically increase storage and query cost.  
Given sample stream labels across logs, estimate cardinality pressure per label.  
Flag labels whose unique value count is near number of streams (e.g., request IDs).  
Return total estimated stream cardinality and risky label list.  
This helps teams redesign labels before production rollout.  
Heuristic estimation is acceptable if clearly defined.  
Output should include simple actionable recommendations.

### Input
- `streams: List[Dict[str,str]]`

### Output
- `{estimated_streams:int, risky_labels: List[str]}`

### Constraints
- `1 <= len(streams) <= 10^6`

### Example
**Input**
- 1000 streams with unique `request_id` in each, same `app="api"`

**Output**
- `risky_labels` includes `request_id`

**Explanation**  
Label with near-unique values per stream is cardinality risk.

### Follow-up
- Suggest relabel/drop strategy automatically.
- Rank labels by impact score.

---

## Problem 50 — Multi-region Uptime Aggregator

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Monitoring` `SRE`

### Description
A synthetic monitoring system probes service health from multiple regions.  
Given check events, compute global uptime percentage and per-region uptime.  
Also compute MTBF (mean time between failures) from event timeline.  
Missing probes should not be treated as success; define handling clearly.  
Return a summary report object.  
This script supports monthly reliability reporting.  
Consistency of time ordering is important.

### Input
- `checks: List[(ts:int, region:str, success:bool)]`

### Output
- `{global_uptime:float, regional_uptime:Dict[str,float], mtbf_sec:float or null}`

### Constraints
- `1 <= len(checks) <= 10^7`

### Example
**Input**
- `[(0,"us",T),(10,"us",F),(20,"us",T),(0,"eu",T),(10,"eu",T)]`

**Output**
- regional uptime for `us=2/3`, `eu=2/2`; global based on all checks

**Explanation**  
Uptime is successful checks divided by total checks.

### Follow-up
- Weighted regions in global score.
- Compute SLI windows (daily/hourly).

---

## Problem 51 — Incident Timeline Merger

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `SRE` `Postmortem` `Logs`

### Description
After outages, teams need one merged timeline from different data sources.  
Given alert events, deployment events, and log markers, merge chronologically.  
Normalize all timestamps to UTC before merge.  
Preserve source tag so viewers know event origin.  
For identical timestamps, apply stable source priority order.  
Return unified timeline list.  
This is useful for automated postmortem generation.

### Input
- `alerts: List[(ts, msg)]`
- `deploys: List[(ts, msg)]`
- `logs: List[(ts, msg)]`
- `source_priority: List[str]` (e.g., `["deploy","alert","log"]`)

### Output
- `List[(ts, source, msg)]`

### Constraints
- Total events up to `10^6`

### Example
**Input**
- alert at `10:03`, deploy at `10:01`, log at `10:02`

**Output**
- deploy -> log -> alert in time order

**Explanation**  
All sources are merged into one sorted sequence.

### Follow-up
- Auto-cluster related events into phases.
- Compute likely trigger event.

---

## Problem 52 — Rollback Decision Engine

**Difficulty:** Hard  
**Topic Tags:** `Scripting` `SRE` `CI/CD` `Decisioning`

### Description
You are implementing automatic rollback guard in deployment pipeline.  
Given baseline and canary metrics, decide one of: `ROLLBACK`, `CONTINUE`, `HOLD`.  
Compare deltas for error rate, latency, and saturation against thresholds.  
If data is missing for required signals, choose conservative `HOLD`.  
Decision must include list of reasons for auditability.  
Use deterministic rule order to avoid ambiguity.  
This reflects realistic progressive-delivery gates.

### Input
- `baseline: {error_rate, p95_latency, cpu}`
- `canary: {error_rate, p95_latency, cpu}`
- `thresholds: {err_mult, latency_mult, cpu_max}`

### Output
- `{decision: str, reasons: List[str]}`

### Constraints
- Metric values non-negative

### Example
**Input**
- baseline `error_rate=0.01`, canary `0.08`, `err_mult=3`
- latency/cpu within limits

**Output**
- `decision="ROLLBACK"`

**Explanation**  
Canary error rate is 8x baseline, exceeding threshold multiplier 3.

### Follow-up
- Add confidence score.
- Add manual approval override when decision is HOLD.

---

## Problem 53 — File Integrity Diff Checker

**Difficulty:** Easy  
**Topic Tags:** `Scripting` `Security` `Filesystem`

### Description
A hardening script stores baseline file hashes for critical directories.  
On next run, compare current hash map with baseline map.  
Report three sets: added files, deleted files, and modified files.  
Ignore paths matching ignore patterns.  
Return paths sorted lexicographically in each category.  
This is a basic integrity monitoring primitive.  
No cryptographic implementation required (hashes provided).

### Input
- `baseline: Dict[path, hash]`
- `current: Dict[path, hash]`
- `ignore_prefixes: List[str]`

### Output
- `{added: List[path], deleted: List[path], modified: List[path]}`

### Constraints
- Up to `10^6` files per map

### Example
**Input**
- baseline: `a->h1, b->h2`
- current: `a->h1, b->h3, c->h4`

**Output**
- `added=["c"], deleted=[], modified=["b"]`

**Explanation**  
`b` changed hash; `c` is new.

### Follow-up
- Sign baseline to prevent tampering.
- Add severity levels by path criticality.

---

## Problem 54 — Certificate Expiry Alerting

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Security` `TLS` `Monitoring`

### Description
You run a cert-watcher for many internal/external endpoints.  
Given certificate expiry timestamps, alert when remaining lifetime <= `N` days.  
Return alerts sorted by nearest expiry first.  
Expired certificates should also be included with negative/zero days left.  
Handle timezone-aware datetime values consistently.  
This script prevents common production outages due to expired certs.  
No network calls are required in this problem.

### Input
- `certs: List[(host:str, expiry_ts:int)]`
- `now_ts: int`
- `N_days: int`

### Output
- `List[(host, days_left:int)]`

### Constraints
- `0 <= N_days <= 3650`

### Example
**Input**
- now = day 100
- certs: `("a", day 105)`, `("b", day 140)`
- `N_days = 14`

**Output**
- `[("a",5)]`

**Explanation**  
Only host `a` expires within next 14 days.

### Follow-up
- Add severity buckets (`critical`, `warning`).
- Add renewal provider hint per domain.

---

## Problem 55 — Cron Overlap Lock Guard

**Difficulty:** Medium  
**Topic Tags:** `Cron` `Scripting` `Reliability`

### Description
A cron job sometimes runs longer than schedule interval, causing overlapping executions and data corruption.  
Implement lock guard with stale-lock recovery.  
If no lock exists, acquire and run (`RUN`).  
If lock exists and still fresh, skip (`SKIP`).  
If lock exists but older than stale threshold, recover and run (`RECOVERED_STALE_LOCK`).  
Return status without running actual job body.  
This pattern is essential for safe cron automation.

### Input
- `lock_exists: bool`
- `lock_age_sec: int` (ignored if no lock)
- `stale_after_sec: int`

### Output
- `"RUN" | "SKIP" | "RECOVERED_STALE_LOCK"`

### Constraints
- `0 <= lock_age_sec <= 10^9`
- `1 <= stale_after_sec <= 10^9`

### Example
**Input**
- `lock_exists=True, lock_age_sec=500, stale_after_sec=300`

**Output**
- `"RECOVERED_STALE_LOCK"`

**Explanation**  
Existing lock is stale and should be replaced safely.

### Follow-up
- Use OS-level file locks.
- Design distributed lock for multi-host cron runners.

---

## Problem 56 — Daily Cron Execution Report

**Difficulty:** Easy  
**Topic Tags:** `Cron` `Scripting` `Reporting`

### Description
Your team wants a daily operational summary for scheduled jobs.  
Given run records, compute attempted runs, successes, failures, average duration, and missed schedules.  
A record includes scheduled time, actual start, end, and status.  
Missed schedule means no run record exists for an expected schedule slot.  
Return one summary object per day.  
This is a realistic reporting script for reliability governance.  
Assume all timestamps are UTC.

### Input
- `expected_slots: List[int]` (scheduled timestamps)
- `runs: List[(scheduled_ts, start_ts, end_ts, status)]`

### Output
- `{attempted, success, failed, avg_duration_sec, missed}`

### Constraints
- Up to `10^6` expected slots per day

### Example
**Input**
- expected 4 slots, runs exist for 3 slots (2 success, 1 fail)

**Output**
- `attempted=3, success=2, failed=1, missed=1`

**Explanation**  
One scheduled slot had no run entry.

### Follow-up
- Generate Markdown/Slack report formatting.
- Add percentile duration stats.

---

## Problem 57 — Idempotent Backup Planner

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Backup` `Reliability`

### Description
A backup script should avoid recopying unchanged files and support resume after interruption.  
Given source and destination manifests, generate action plan:
`copy`, `skip`, and `verify`.  
If checksum matches, skip; if missing or mismatched, copy then verify.  
Order actions deterministically by path.  
Do not perform actual copy in this problem.  
This tests idempotency design in automation workflows.  
Large manifests should be handled efficiently.

### Input
- `src: Dict[path, checksum]`
- `dst: Dict[path, checksum]`

### Output
- `List[(action, path)]`

### Constraints
- Up to `10^6` files

### Example
**Input**
- src: `a=h1, b=h2, c=h3`
- dst: `a=h1, b=old`

**Output**
- `[("skip","a"),("copy","b"),("verify","b"),("copy","c"),("verify","c")]`

**Explanation**  
`a` unchanged; `b` changed; `c` missing in destination.

### Follow-up
- Add retention generations.
- Add bandwidth throttling per host.

---

## Problem 58 — Blue/Green Switch Safety Validator

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Deployment` `SRE`

### Description
Before switching traffic from blue to green, you must validate readiness gates.  
Given health metrics for both environments, decide if switch is safe.  
Minimum healthy green instances, max error rate, and warm-up completion are mandatory checks.  
If any required metric is missing, return unsafe with reason.  
Output boolean plus detailed reasons list.  
This script protects production cutovers from unsafe transitions.  
Keep rule evaluation deterministic.

### Input
- `green: {healthy:int, error_rate:float, warmed_up:bool}`
- `policy: {min_healthy:int, max_error:float}`

### Output
- `{safe: bool, reasons: List[str]}`

### Constraints
- Non-negative values

### Example
**Input**
- green `{healthy:8,error_rate:0.01,warmed_up:true}`
- policy `{min_healthy:6,max_error:0.02}`

**Output**
- `safe=true, reasons=[]`

**Explanation**  
All required thresholds pass.

### Follow-up
- Add progressive traffic step checks (10%, 25%, 50%, 100%).
- Compare against blue baseline, not only absolute thresholds.

---

## Problem 59 — Runbook Skeleton Generator

**Difficulty:** Easy  
**Topic Tags:** `Scripting` `SRE` `Documentation`

### Description
Alert quality improves when every alert links to a runbook.  
Given alert metadata, generate markdown runbook skeleton sections.  
Each section should include symptoms, possible causes, diagnostics, mitigation, escalation, and owner.  
Return complete markdown text for all alerts.  
Ensure deterministic section order by alert name.  
This automation reduces manual documentation effort.  
No external data fetch required.

### Input
- `alerts: List[{name, severity, owner, dashboard_url}]`

### Output
- `str` markdown

### Constraints
- `1 <= len(alerts) <= 10^4`

### Example
**Input**
- alert `{name:"HighErrorRate", severity:"critical", owner:"payments"}`

**Output**
- markdown heading `## HighErrorRate` with template subsections

**Explanation**  
Generator creates standard operational playbook format.

### Follow-up
- Insert recent incident links if available.
- Generate checklist version for pager playbooks.

---

## Problem 60 — Jenkins + Docker Artifact Cleaner Planner

**Difficulty:** Medium  
**Topic Tags:** `Jenkins` `Docker` `Scripting` `Cleanup`

### Description
CI storage is filling up due to old build artifacts and unused images.  
Given build history and image metadata, identify safe deletion candidates.  
Keep last `k` successful artifacts per branch.  
Never delete artifacts/images referenced by active deployments.  
Return candidate lists and estimated reclaimed size.  
This mirrors common CI hygiene automation tasks.  
No actual deletion should occur.

### Input
- `artifacts: List[(id, branch, success, size_mb, active_ref:bool)]`
- `images: List[(image, size_mb, active_ref:bool)]`
- `k: int`

### Output
- `{artifact_delete: List[id], image_delete: List[image], reclaim_mb:int}`

### Constraints
- `1 <= k <= 100`
- Up to `10^6` artifacts

### Example
**Input**
- branch `main` has 8 successful artifacts, `k=5`, none active
- one old image not active

**Output**
- oldest 3 artifacts in `main` + old image selected

**Explanation**  
Retention policy keeps recent `k`; active refs are protected.

### Follow-up
- Add dry-run JSON report.
- Add policy exceptions for release tags.

---

## Problem 61 — Prometheus Target Down Root-cause Hints

**Difficulty:** Medium  
**Topic Tags:** `Prometheus` `Scripting` `Troubleshooting`

### Description
Your monitoring UI shows many `target down` alerts without clear cause.  
Given scrape error messages and connectivity diagnostics, classify likely root cause.  
Possible classes: DNS failure, timeout, TLS failure, auth failure, app down, unknown.  
Return classification and one-line remediation hint per target.  
Rule-based matching is sufficient.  
This helps route incidents faster to right ownership group.  
Keep rules deterministic and documented.

### Input
- `targets: List[{name, error_msg, http_status?, dns_ok?, tls_ok?}]`

### Output
- `List[(target, cause, hint)]`

### Constraints
- Up to `10^6` targets

### Example
**Input**
- target error: `"x509: certificate expired"`

**Output**
- `("svc-a","tls_failure","renew certificate and restart endpoint if needed")`

**Explanation**  
Error message directly maps to TLS/certificate category.

### Follow-up
- Add confidence score.
- Add runbook URL mapping by cause.

---

## Problem 62 — Loki Query Cost Heuristic

**Difficulty:** Medium  
**Topic Tags:** `Loki` `Scripting` `Performance` `Observability`

### Description
Broad or regex-heavy LogQL queries can overload Loki clusters.  
Given query metadata (time range, selector width, regex count, pipeline stages), estimate relative query cost.  
Compute a heuristic cost score and classify as `LOW/MEDIUM/HIGH`.  
Also return optimization tips for high-cost queries.  
This enables pre-execution guardrails in internal tools.  
Heuristic formula should be simple and transparent.  
No actual query execution is needed.

### Input
- `meta: {hours:int, selector_cardinality:int, regex_filters:int, pipeline_stages:int}`

### Output
- `{score: float, level: str, tips: List[str]}`

### Constraints
- Non-negative integers
- `hours <= 24*365`

### Example
**Input**
- `{hours:168, selector_cardinality:100000, regex_filters:3, pipeline_stages:4}`

**Output**
- `level="HIGH"` with tips like “narrow labels”, “reduce regex”

**Explanation**  
Very long range + broad selectors + regex pipelines increase cost.

### Follow-up
- Learn scoring weights from historical query durations.
- Add hard deny threshold for extreme scores.

---

## Problem 63 — Alertmanager Route Simulator

**Difficulty:** Hard  
**Topic Tags:** `Prometheus` `Alertmanager` `Scripting` `Routing`

### Description
You need to test alert routing rules before applying production config.  
Given alert labels and nested route tree, determine final receiver(s).  
Support matcher evaluation and first-match/continue behavior as configured.  
Apply inhibit rules to mark suppressed alerts.  
Return selected receiver, grouping key, and inhibited flag.  
This problem models real Alertmanager behavior at simplified level.  
Deterministic evaluation order is required.

### Input
- `alert: Dict[str,str]`
- `routes: route_tree`
- `inhibit_rules: List[rule]`
- `group_by: List[str]`

### Output
- `{receivers: List[str], group_key: str, inhibited: bool}`

### Constraints
- Route nodes up to `10^5`

### Example
**Input**
- alert labels: `{severity:"critical", team:"payments"}`
- route sends critical+payments to `payments-pager`

**Output**
- `receivers=["payments-pager"]`, `inhibited=false`

**Explanation**  
Matchers select team-specific critical receiver path.

### Follow-up
- Return route traversal path for debugging.
- Simulate grouping wait timers.

---

## Problem 64 — Replace `grep | awk | sort` in Python

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `Python` `Shell-to-Python` `Parsing`

### Description
Interviewers often ask you to rewrite shell pipelines in Python for maintainability.  
Given text lines, filter by regex, extract `k`-th whitespace field, count frequency, and sort descending by count.  
This replicates common `grep | awk '{print $k}' | sort | uniq -c | sort -nr` pattern.  
Skip lines with fewer than `k` fields.  
Return list of `(field_value, count)`.  
Prefer streaming logic to handle huge files.  
Tie-break by lexicographic order of field value.

### Input
- `lines: Iterable[str]`
- `regex: str`
- `k: int` (1-indexed)

### Output
- `List[(value:str, count:int)]`

### Constraints
- `1 <= k <= 1000`
- Up to `10^7` lines

### Example
**Input**
- lines: `["200 /a", "500 /x", "200 /b", "500 /x"]`
- regex: `".*"`
- `k=1`

**Output**
- `[("200",2),("500",2)]`

**Explanation**  
First field frequencies are both 2, sorted lexicographically on tie.

### Follow-up
- Add CLI flags equivalent to grep/awk options.
- Support custom field separators.

---

## Problem 65 — Parse `kubectl get pods -A` Non-healthy Pods

**Difficulty:** Easy  
**Topic Tags:** `Kubernetes` `Scripting` `Parsing`

### Description
During outages, you need a quick list of pods not in healthy states.  
Given parsed pod table rows, return those whose status is neither `Running` nor `Completed`.  
Include namespace, pod name, status, restarts, and age.  
Sort output by namespace then pod name.  
This is a frequent Kubernetes triage scripting question.  
Input may include many namespaces.  
No kubectl execution is required here.

### Input
- `pods: List[(ns,pod,status,restarts,age)]`

### Output
- `List[(ns,pod,status,restarts,age)]`

### Constraints
- Up to `10^6` pods

### Example
**Input**
- `[("prod","api-1","Running",0,"1d"),("prod","api-2","CrashLoopBackOff",7,"2h")]`

**Output**
- `[("prod","api-2","CrashLoopBackOff",7,"2h")]`

**Explanation**  
Only non-healthy pod is returned.

### Follow-up
- Prefer JSON parsing from API for robust tooling.
- Group failures by reason.

---

## Problem 66 — Paginated API Poller with Checkpoint

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `API` `Reliability` `Automation`

### Description
A sync job consumes paginated API data and may crash mid-run.  
Implement poller that processes records and persists checkpoint after each record/page.  
On rerun, it should resume from checkpoint without duplicating already-processed work.  
Return processed count and final checkpoint.  
Assume records have stable unique IDs and page ordering.  
Design should be idempotent under repeated retries.  
This reflects resilient automation design in production.

### Input
- `pages: List[List[record_id]]`
- `checkpoint: {page_idx:int, item_idx:int} or null`
- `max_to_process: int` (simulate interruption limit)

### Output
- `{processed_ids: List[id], new_checkpoint}`

### Constraints
- Up to `10^7` records

### Example
**Input**
- pages `[[1,2],[3,4]]`, checkpoint `null`, `max_to_process=3`

**Output**
- processed `[1,2,3]`
- checkpoint points to next item after `3`

**Explanation**  
After simulated interruption, rerun resumes from saved checkpoint.

### Follow-up
- Discuss exactly-once vs at-least-once processing.
- Add dead-letter handling for poisoned records.

---

## Problem 67 — Clock Skew Detector

**Difficulty:** Medium  
**Topic Tags:** `Scripting` `SRE` `Time` `Monitoring`

### Description
Distributed systems fail subtly when host clocks drift too far apart.  
Given per-host time offset from reference, flag hosts exceeding threshold.  
Return flagged hosts sorted by absolute skew descending.  
Include sign of skew for diagnostics (`ahead`/`behind`).  
This script is often part of infrastructure health checks.  
Input offsets are in milliseconds.  
No NTP interaction required.

### Input
- `offsets: List[(host:str, offset_ms:int)]`
- `threshold_ms: int`

### Output
- `List[(host, offset_ms)]`

### Constraints
- `0 <= threshold_ms <= 10^7`
- `1 <= len(offsets) <= 10^6`

### Example
**Input**
- `[("h1",10),("h2",-35),("h3",5000)]`
- `threshold_ms=1000`

**Output**
- `[("h3",5000)]`

**Explanation**  
Only `h3` exceeds absolute skew threshold.

### Follow-up
- Track drift trend over time.
- Trigger severity by skew magnitude.

---

## Problem 68 — CI Flaky Test Scorer

**Difficulty:** Medium  
**Topic Tags:** `Jenkins` `CI/CD` `Scripting` `Analytics`

### Description
Your CI has unstable tests that fail intermittently and waste engineering time.  
Given historical test outcomes, compute flaky score for each test.  
A test is flaky if it alternates pass/fail frequently across nearby builds.  
Define a clear scoring formula and return top flaky tests.  
Ignore tests with very low sample count (configurable minimum runs).  
This problem checks data aggregation and metric design.  
Result should be reproducible and easy to explain.

### Input
- `results: List[(test_name:str, build_id:int, status:str)]`
- `min_runs: int`
- `top_k: int`

### Output
- `List[(test_name, flaky_score:float)]`

### Constraints
- `1 <= len(results) <= 10^7`
- status in `{PASS, FAIL}`

### Example
**Input**
- `A: PASS,FAIL,PASS,FAIL`
- `B: PASS,PASS,PASS,FAIL`
- `min_runs=3, top_k=1`

**Output**
- `[("A", higher_score)]`

**Explanation**  
Test A alternates outcomes more frequently, indicating stronger flakiness.

### Follow-up
- Weight recent builds more heavily.
- Recommend quarantine candidates automatically.

---

## Problem 69 — End-to-End Ops CLI Challenge

**Difficulty:** Hard  
**Topic Tags:** `Scripting` `Docker` `Jenkins` `Prometheus` `Loki` `SRE` `System Design`

### Description
Build a production-style Python CLI named `ops_tool` with multiple subcommands.  
It should combine core interview topics into one cohesive utility: cron validation, grep-with-context, Docker audit, Jenkins log classification, Prometheus parsing, and Loki query construction.  
Each subcommand accepts file/stdin input and emits structured output.  
A final `report` command aggregates outputs into Markdown summary.  
Error handling, logging, and testability are mandatory.  
Design for clean module boundaries and reusable utilities.  
This is a capstone scenario resembling real DevOps scripting work.

### Input
- CLI args + input files for each subcommand

### Output
- per-command results + consolidated markdown report

### Constraints
- No hardcoded secrets
- Unit tests for each subcommand
- Deterministic output for same input

### Example
**Input**
- run sequence:
  - `ops_tool cron-validate schedules.txt`
  - `ops_tool log-grep app.log "ERROR" -B 2 -A 2`
  - `ops_tool report --from artifacts/*.json`

**Output**
- command-wise JSON/TSV outputs and final markdown report summarizing findings

**Explanation**  
Tool orchestrates multiple DevOps scripting tasks end-to-end.

### Follow-up
- Package and publish as pip module.
- Add CI pipeline, semantic versioning, and changelog automation.

---

# Bonus Interview Drills (Short Theory Prompts)

1. How do you make automation scripts idempotent and safe for reruns?  
2. What logging/metrics should every non-trivial script emit?  
3. How do you handle partial failures in multi-step automation?  
4. What security pitfalls exist when executing external commands from Python?  
5. How would you test scripts that rely on external systems (APIs, filesystem, shell)?

---

# Suggested Practice Plan (4 Weeks)

- **Week 1:** Problems 1–15 (core scripting fundamentals)
- **Week 2:** Problems 16–34 (Linux ops + Kubernetes + reliability)
- **Week 3:** Problems 35–49 (Docker/Jenkins/Prometheus/Loki)
- **Week 4:** Problems 50–69 (SRE depth + capstone challenge)

---