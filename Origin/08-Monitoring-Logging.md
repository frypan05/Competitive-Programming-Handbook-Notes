# 08 — Monitoring, Logging & Alerting

---

## SECTION 1 — Observability: The Three Pillars

**Q: What is observability and how does it differ from monitoring?**

```
Monitoring:
  - Tells you WHEN something is wrong.
  - Based on pre-defined thresholds and known failure modes.
  - You must know in advance what to look for.
  - Example: alert when CPU > 90% or error rate > 5%.

Observability:
  - Lets you understand WHY something is wrong.
  - Built on three pillars: Metrics, Logs, Traces.
  - Enables you to debug failures you didn't anticipate.
  - Requires well-structured telemetry emitted by the application.
  - You can ask arbitrary questions about system behaviour.
```

**The Three Pillars:**

```
Metrics — aggregated numerical measurements over time.
          What: CPU%, memory%, RPS, error rate, latency p99, queue depth.
          Tools: Prometheus, CloudWatch, Datadog, InfluxDB.
          Best for: alerting, dashboards, capacity planning, SLA tracking.

Logs    — timestamped, structured event records.
          What: application events, errors, access logs, audit trails.
          Tools: ELK Stack, EFK Stack, Loki, CloudWatch Logs, Datadog Logs.
          Best for: debugging specific failures, forensics, audit compliance.

Traces  — end-to-end record of a request as it flows through multiple services.
          What: spans with timing, each span is one service call.
          Tools: Jaeger, Zipkin, AWS X-Ray, Datadog APM, OpenTelemetry.
          Best for: finding bottlenecks in distributed systems, debugging latency.
```

---

## SECTION 2 — RED and USE Methods

These are frameworks for deciding WHAT to monitor.

**RED Method (for services/APIs):**
```
Rate     — requests per second (how busy is the service?)
Errors   — error rate (what fraction of requests are failing?)
Duration — latency distribution (how long do requests take? p50/p95/p99)
```

**USE Method (for infrastructure resources):**
```
Utilisation — what percentage of capacity is being used?
              CPU: 85% used, Memory: 70% used
Saturation  — how much extra work is waiting?
              CPU run queue length, disk I/O queue depth
Errors      — error events count.
              CPU errors, disk errors, network packet drops
```

Apply RED to every microservice. Apply USE to every node, CPU, disk, network interface.

---

## SECTION 3 — Prometheus

**Q: What is Prometheus and how does it work?**

A: Prometheus is an open-source monitoring system and time-series database (TSDB).
It works by PULLING (scraping) metrics from targets over HTTP at regular intervals (default 15s).
Targets expose metrics at a `/metrics` endpoint in a plain-text format.

```
Architecture:
  Prometheus server
    |-- Scrapes /metrics from targets on a schedule
    |-- Stores time-series data in its local TSDB
    |-- Evaluates alerting and recording rules
    |-- Exposes a query API (PromQL)

  Exporters — expose metrics for systems that don't natively support Prometheus:
    node_exporter    — Linux system metrics (CPU, memory, disk, network)
    blackbox_exporter — probe HTTP, DNS, TCP, ICMP endpoints
    mysqld_exporter  — MySQL metrics
    redis_exporter   — Redis metrics
    postgres_exporter — PostgreSQL metrics
    kube-state-metrics — Kubernetes object state (pod counts, deployment status)

  Pushgateway — for short-lived batch jobs that can't be scraped.
                Jobs push metrics to Pushgateway; Prometheus scrapes Pushgateway.

  Alertmanager — receives firing alerts from Prometheus, handles:
                 deduplication, grouping, inhibition, silencing, routing to receivers.

  Grafana — visualises Prometheus metrics with dashboards and panels.
```

---

**Q: What is the Prometheus data model?**

A: Every time series is uniquely identified by a metric name + a set of label key-value pairs.

```
# Format: metric_name{label1="value1", label2="value2"} <value> [timestamp]

http_requests_total{method="GET",  status="200", service="api"} 12543
http_requests_total{method="POST", status="500", service="api"} 7
http_requests_total{method="GET",  status="200", service="auth"} 4821

node_cpu_seconds_total{cpu="0", mode="idle"}   98432.5
node_cpu_seconds_total{cpu="0", mode="user"}   1234.7
node_cpu_seconds_total{cpu="1", mode="idle"}   96210.3
```

---

**Q: What are the four Prometheus metric types?**

```
Counter:
  - Monotonically increasing value. Resets to 0 on restart.
  - Examples: http_requests_total, errors_total, bytes_sent_total
  - Query: use rate() or increase() — never query raw counters.
  - Never use for values that can go down.

Gauge:
  - Value that can go up and down.
  - Examples: memory_usage_bytes, active_connections, temperature_celsius, queue_depth
  - Query: current value directly, or delta(), deriv().

Histogram:
  - Samples observations into configurable buckets.
  - Automatically provides: _bucket (cumulative counts per bucket), _sum, _count.
  - Examples: http_request_duration_seconds, request_payload_size_bytes
  - Query: histogram_quantile() to compute percentiles.
  - Can be aggregated across instances (unlike Summary).

Summary:
  - Similar to Histogram but calculates quantiles on the CLIENT side.
  - Provides: _sum, _count, and configured quantile values.
  - Cannot be aggregated across instances (quantiles are pre-computed).
  - Rarely used in new code — prefer Histogram.
```

---

**Q: What is PromQL? Give practical examples.**

A: Prometheus Query Language. Used to query, filter, aggregate, and compute over time-series data.

```promql
# --- Basic selectors ---

# All time series for this metric
http_requests_total

# Filter by labels
http_requests_total{service="api", status="200"}

# Regex label match
http_requests_total{status=~"5.."}        # all 5xx statuses
http_requests_total{status!~"2.."}        # exclude 2xx


# --- Rate and Increase ---

# Request rate per second over last 5 minutes (for counters)
rate(http_requests_total[5m])

# Total increase over last hour
increase(http_requests_total[1h])

# Always use rate() with counters, never query raw counter values directly


# --- Aggregation operators ---

# Total RPS across all services
sum(rate(http_requests_total[5m]))

# RPS per service
sum(rate(http_requests_total[5m])) by (service)

# Average CPU utilisation per node
avg by (instance) (
  100 - (rate(node_cpu_seconds_total{mode="idle"}[5m]) * 100)
)

# Top 5 services by request rate
topk(5, sum(rate(http_requests_total[5m])) by (service))


# --- Histograms and Percentiles ---

# 99th percentile request latency per service
histogram_quantile(
  0.99,
  sum(rate(http_request_duration_seconds_bucket[5m])) by (le, service)
)

# 95th percentile latency
histogram_quantile(0.95, rate(http_request_duration_seconds_bucket[5m]))


# --- Error rate ---

# HTTP error rate (fraction of requests that are 5xx)
sum(rate(http_requests_total{status=~"5.."}[5m]))
/
sum(rate(http_requests_total[5m]))

# As a percentage
(
  sum(rate(http_requests_total{status=~"5.."}[5m]))
  /
  sum(rate(http_requests_total[5m]))
) * 100


# --- Memory ---

# Memory utilisation percentage
(
  node_memory_MemTotal_bytes - node_memory_MemAvailable_bytes
) / node_memory_MemTotal_bytes * 100

# Used memory in GB
(node_memory_MemTotal_bytes - node_memory_MemAvailable_bytes) / 1024 / 1024 / 1024


# --- Disk ---

# Disk usage percentage per device
(
  node_filesystem_size_bytes{fstype!="tmpfs"} -
  node_filesystem_avail_bytes{fstype!="tmpfs"}
) / node_filesystem_size_bytes{fstype!="tmpfs"} * 100

# Disk I/O rate
rate(node_disk_read_bytes_total[5m])
rate(node_disk_written_bytes_total[5m])


# --- Kubernetes ---

# Pod restart rate
increase(kube_pod_container_status_restarts_total[1h]) > 0

# Pods not running
kube_pod_status_phase{phase!="Running", phase!="Succeeded"}

# CPU throttling percentage
rate(container_cpu_throttled_seconds_total[5m])
/
rate(container_cpu_usage_seconds_total[5m])

# Memory utilisation vs limit
container_memory_working_set_bytes
/
container_spec_memory_limit_bytes
```

---

**Q: What is a recording rule vs an alerting rule?**

```
Recording rule:
  - Pre-computes an expensive PromQL expression and saves the result as a new metric.
  - Purpose: speed up dashboard queries, reduce Prometheus query load.
  - The pre-computed metric is queryable like any other metric.

Alerting rule:
  - Evaluates a PromQL expression on every evaluation interval.
  - If the expression returns results AND has been true for the `for` duration, fires an alert.
  - Fired alerts are sent to Alertmanager.
```

```yaml
# prometheus/rules/api.yml
groups:
  - name: api.rules
    interval: 30s   # evaluation interval for this group

    rules:
      # --- Recording Rules ---

      # Pre-compute RPS per service (expensive to compute on every dashboard load)
      - record: job:http_requests:rate5m
        expr: sum(rate(http_requests_total[5m])) by (job)

      # Pre-compute 99th percentile latency
      - record: job:http_request_duration_seconds:p99_5m
        expr: |
          histogram_quantile(0.99,
            sum(rate(http_request_duration_seconds_bucket[5m])) by (le, job)
          )


      # --- Alerting Rules ---

      - alert: HighErrorRate
        expr: |
          (
            sum(rate(http_requests_total{status=~"5.."}[5m])) by (service)
            /
            sum(rate(http_requests_total[5m])) by (service)
          ) > 0.05
        for: 5m       # must be true for 5 minutes before firing
        labels:
          severity: critical
          team: backend
        annotations:
          summary: "High error rate on {{ $labels.service }}"
          description: >
            Error rate is {{ $value | humanizePercentage }} on {{ $labels.service }}.
            This has been above 5% for 5 minutes.
          runbook: "https://wiki.company.com/runbooks/high-error-rate"

      - alert: HighLatencyP99
        expr: |
          histogram_quantile(0.99,
            sum(rate(http_request_duration_seconds_bucket[5m])) by (le, service)
          ) > 2
        for: 10m
        labels:
          severity: warning
          team: backend
        annotations:
          summary: "p99 latency above 2s for {{ $labels.service }}"
          description: "p99 latency is {{ $value | humanizeDuration }} on {{ $labels.service }}"

      - alert: DiskSpaceRunningOut
        expr: |
          (
            node_filesystem_avail_bytes{fstype!="tmpfs"}
            / node_filesystem_size_bytes{fstype!="tmpfs"}
          ) < 0.10
        for: 5m
        labels:
          severity: warning
        annotations:
          summary: "Disk space below 10% on {{ $labels.instance }}:{{ $labels.mountpoint }}"

      - alert: PodCrashLooping
        expr: increase(kube_pod_container_status_restarts_total[1h]) > 3
        for: 0m       # fire immediately, no waiting
        labels:
          severity: warning
        annotations:
          summary: "Pod {{ $labels.namespace }}/{{ $labels.pod }} is crash looping"

      - alert: NodeMemoryPressure
        expr: |
          (
            node_memory_MemTotal_bytes - node_memory_MemAvailable_bytes
          ) / node_memory_MemTotal_bytes > 0.90
        for: 10m
        labels:
          severity: critical
        annotations:
          summary: "Memory above 90% on {{ $labels.instance }}"
```

---

## SECTION 4 — Alertmanager

**Q: How does Alertmanager work?**

A: Alertmanager receives firing alerts from Prometheus (via HTTP POST), then handles:
1. **Grouping**: combines related alerts into a single notification (e.g., all alerts from same service)
2. **Inhibition**: suppress lower-priority alerts when a higher-priority one is firing
3. **Silencing**: mute specific alerts for a time window (maintenance)
4. **Routing**: send alerts to different receivers based on label matchers

```yaml
# alertmanager.yml

global:
  resolve_timeout: 5m
  slack_api_url: 'https://hooks.slack.com/services/T00/B00/XXX'
  smtp_smarthost: 'smtp.company.com:587'
  smtp_from: 'alertmanager@company.com'

# Tree of routes — each route can match and route to a receiver
route:
  receiver: slack-default         # default receiver if no child matches
  group_by: [alertname, service, cluster]
  group_wait: 30s                 # wait 30s to batch grouped alerts before sending
  group_interval: 5m              # how long to wait before sending new alerts in the group
  repeat_interval: 12h            # resend if alert is still firing after 12h

  routes:
    # Critical alerts go to PagerDuty AND Slack
    - match:
        severity: critical
      receiver: pagerduty-critical
      continue: true              # don't stop here; also evaluate following routes

    # Database team alerts
    - match:
        team: database
      receiver: db-team-slack

    # SQS/Queue alerts
    - match_re:
        alertname: ".*Queue.*|.*SQS.*"
      receiver: platform-slack

receivers:
  - name: slack-default
    slack_configs:
      - channel: '#alerts'
        send_resolved: true
        title: '[{{ .Status | toUpper }}{{ if eq .Status "firing" }}:{{ .Alerts.Firing | len }}{{ end }}] {{ .GroupLabels.alertname }}'
        text: |
          {{ range .Alerts }}
          *Alert:* {{ .Labels.alertname }}
          *Severity:* {{ .Labels.severity }}
          *Description:* {{ .Annotations.description }}
          *Runbook:* {{ .Annotations.runbook }}
          {{ end }}

  - name: pagerduty-critical
    pagerduty_configs:
      - service_key: '<pagerduty-integration-key>'
        description: '{{ .GroupLabels.alertname }}: {{ .Annotations.summary }}'

  - name: db-team-slack
    slack_configs:
      - channel: '#database-alerts'
        title: 'DB Alert: {{ .GroupLabels.alertname }}'

  - name: platform-slack
    slack_configs:
      - channel: '#platform-alerts'

# Inhibition rules
inhibit_rules:
  # If a critical alert fires, suppress warning alerts for same service
  - source_match:
      severity: critical
    target_match:
      severity: warning
    equal: [alertname, service, cluster]
```

---

## SECTION 5 — Grafana

**Q: How do you build a useful dashboard in Grafana?**

**Design principles:**
```
1. Apply RED method for services: Rate, Error rate, Duration (latency).
2. Apply USE method for infrastructure: Utilisation, Saturation, Errors.
3. Overview first, drill-down capability (use variables for filtering).
4. Use dashboard variables for: environment, cluster, namespace, time range.
5. Annotate deployments and incidents on graphs.
6. Set meaningful thresholds (coloured zones: green/yellow/red).
7. Title every panel with a clear question: "API Error Rate (%)" not "errors".
```

**Key panel types:**
```
Time Series  — latency trends, RPS over time, error rate over time.
Stat         — current single value: uptime %, error rate, active connections.
Gauge        — utilisation: CPU %, memory %, disk %.
Bar Chart    — comparisons: top 10 slowest endpoints.
Table        — pod status, top-N slowest queries, recent errors.
Heatmap      — request latency distribution over time (from histogram).
Logs panel   — correlated log view (with Loki or Elasticsearch datasource).
Alert List   — active firing alerts.
```

**Dashboard Variables:**
```
In Dashboard Settings -> Variables -> Add Variable:
  Name: environment
  Type: Query
  Datasource: Prometheus
  Query: label_values(up{job="api"}, environment)
  Multi-value: true
  Include All: true

Usage in panels:
  rate(http_requests_total{environment="$environment"}[5m])
  node_cpu_seconds_total{instance=~"$instance"}
```

**Example: Service Overview Dashboard panels**
```
Row 1: Traffic
  - RPS (sum(rate(http_requests_total[5m])) by (service))
  - Error Rate (%)
  - p99 Latency (ms)
  - Active Connections

Row 2: Infrastructure
  - CPU Utilisation per node
  - Memory Utilisation per node
  - Disk I/O
  - Network In/Out

Row 3: Kubernetes
  - Pod count (running vs desired)
  - Pod restart count (last 1h)
  - HPA scale events
  - PVC usage

Row 4: Application
  - Queue depth (SQS ApproximateNumberOfMessagesVisible)
  - DLQ depth (ApproximateNumberOfMessagesVisible on DLQ)
  - Cache hit rate
  - DB connection pool usage
```

---

## SECTION 6 — ELK Stack (Elasticsearch, Logstash, Kibana)

**Q: What is the ELK/EFK stack? Explain each component.**

```
Elasticsearch:
  - Distributed search and analytics engine built on Apache Lucene.
  - Stores documents as JSON. Full-text search, structured queries, aggregations.
  - Horizontally scalable: shards data across nodes.
  - Used as the log storage and search backend.
  - REST API for all operations.

Logstash:
  - Data processing pipeline (ETL for logs).
  - Input: reads from files, syslog, Kafka, Redis, Beats.
  - Filter: parse, transform, enrich (Grok patterns for unstructured logs, GeoIP, date parsing).
  - Output: sends to Elasticsearch, S3, Kafka, or other destinations.
  - Heavy: runs on JVM. High memory usage.

Kibana:
  - Web UI for Elasticsearch.
  - Discover: search and explore log data.
  - Visualize: charts, graphs, maps from log data.
  - Dashboard: combine visualizations.
  - Alerting: set up alerts on log patterns.
  - ML: anomaly detection on log data.

Beats (lightweight shippers — replace Logstash for simple forwarding):
  - Filebeat:    tail log files, forward to Logstash or Elasticsearch directly.
  - Metricbeat:  system and service metrics.
  - Packetbeat:  network traffic analysis.
  - Heartbeat:   uptime monitoring (ping services).
  - Auditbeat:   Linux audit framework data.
```

**EFK Stack (preferred for Kubernetes):**
```
Elasticsearch + Fluentd + Kibana

Why Fluentd over Logstash for K8s:
  - Runs as a DaemonSet (one pod per node).
  - Lower memory footprint.
  - Kubernetes-aware: enriches logs with pod metadata automatically.
  - Better plugin ecosystem for cloud-native environments.
  - Written in Ruby+C (lighter than JVM-based Logstash).
```

---

**Q: How does log collection work in Kubernetes?**

```
Step 1: Applications write logs to STDOUT and STDERR.
        (This is the Kubernetes best practice — 12-factor app rule 11.)
        Never write to files inside the container.

Step 2: The container runtime (containerd) captures STDOUT/STDERR and writes to files:
        /var/log/pods/<namespace>_<pod>_<uid>/<container>/<n>.log
        /var/log/containers/<pod>_<namespace>_<container>_<id>.log (symlinks)

Step 3: Fluentd/Filebeat runs as a DaemonSet.
        It tails the log files on each node via hostPath volume mounts.
        It enriches each log line with Kubernetes metadata:
          - pod name, namespace, node name, labels, annotations, container name.

Step 4: Logs are forwarded to Elasticsearch (or Loki, CloudWatch, etc.)
        with structured JSON fields.

Step 5: Kibana (or Grafana for Loki) provides search, dashboards, alerting.
```

**Fluentd DaemonSet configuration (key parts):**
```yaml
apiVersion: apps/v1
kind: DaemonSet
metadata:
  name: fluentd
  namespace: kube-system
spec:
  selector:
    matchLabels:
      name: fluentd
  template:
    metadata:
      labels:
        name: fluentd
    spec:
      tolerations:
        - key: node-role.kubernetes.io/control-plane
          effect: NoSchedule
      serviceAccountName: fluentd
      containers:
        - name: fluentd
          image: fluent/fluentd-kubernetes-daemonset:v1-debian-elasticsearch
          env:
            - name: FLUENT_ELASTICSEARCH_HOST
              value: "elasticsearch.logging.svc.cluster.local"
            - name: FLUENT_ELASTICSEARCH_PORT
              value: "9200"
            - name: FLUENT_ELASTICSEARCH_SCHEME
              value: "http"
          resources:
            limits:
              memory: 512Mi
            requests:
              cpu: 100m
              memory: 200Mi
          volumeMounts:
            - name: varlog
              mountPath: /var/log
            - name: varlibdockercontainers
              mountPath: /var/lib/docker/containers
              readOnly: true
      volumes:
        - name: varlog
          hostPath:
            path: /var/log
        - name: varlibdockercontainers
          hostPath:
            path: /var/lib/docker/containers
```

---

**Q: What is Loki and how does it differ from Elasticsearch?**

```
Loki (Grafana Labs):
  - Designed specifically for logs, inspired by Prometheus.
  - Indexes only metadata (labels), NOT the full log content.
  - Much cheaper storage: logs stored compressed in S3/GCS.
  - Query language: LogQL (similar to PromQL).
  - Integrated with Grafana — see metrics and logs side by side.
  - Lightweight: no cluster to manage for small to medium scale.
  - Tradeoff: full-text search is slower than Elasticsearch.

Elasticsearch:
  - Inverted index on all log content — extremely fast full-text search.
  - Higher storage cost (indexes are large).
  - More powerful: aggregations, anomaly detection, ML features.
  - Better for: compliance, audit logs, complex ad-hoc queries.
  - Higher operational complexity.

Choose Loki when: you have high log volume, costs matter, and you primarily filter by labels.
Choose Elasticsearch when: you need powerful full-text search or complex log analytics.
```

---

## SECTION 7 — Structured Logging Best Practices

**Q: What is structured logging and why does it matter?**

A: Writing logs as JSON (or another machine-parseable format) instead of plain text strings.
Structured logs are easier to query, filter, aggregate, and alert on.

```python
# BAD — unstructured, impossible to query field-by-field
import logging
logging.info(f"User 12345 placed order abc-789 for $99.99")

# GOOD — structured, every field is queryable
import structlog
logger = structlog.get_logger()

logger.info(
    "order_placed",
    user_id=12345,
    order_id="abc-789",
    amount=99.99,
    currency="USD",
    payment_method="credit_card",
    latency_ms=45
)
# Output:
# {"event": "order_placed", "user_id": 12345, "order_id": "abc-789",
#  "amount": 99.99, "currency": "USD", "timestamp": "2024-01-15T10:30:00Z",
#  "level": "info", "service": "order-service", "version": "v2.1.0"}
```

**Fields every log line should have:**
```
timestamp    — ISO 8601 with timezone
level        — DEBUG, INFO, WARNING, ERROR, CRITICAL
service      — which service emitted this
version      — service version (important for debugging after deploys)
trace_id     — distributed trace ID (correlate logs across services)
span_id      — current span in the trace
request_id   — unique ID for this HTTP request
user_id      — who triggered this action (if applicable)
event        — what happened (machine-readable event name)
message      — human-readable description
error        — error message (if level is ERROR)
stack_trace  — only in ERROR/CRITICAL, not in every log line
```

---

## SECTION 8 — Key Metrics to Monitor by Layer

**Infrastructure metrics (node_exporter):**
```
CPU:
  - Utilisation per core and overall
  - iowait (high iowait = CPU waiting for disk I/O — disk bottleneck)
  - steal (high steal = hypervisor taking CPU — noisy neighbour problem on VMs)

Memory:
  - Used / Available (use "available", not "free")
  - Swap usage (any swap usage in production = problem)
  - OOM kill events (dmesg | grep -i oom)

Disk:
  - Usage percentage per mount point
  - I/O read/write bytes per second
  - I/O await time (how long requests wait in the queue)
  - IOPS

Network:
  - Bytes in/out per interface
  - Packets dropped / errors
  - TCP connection states (TIME_WAIT count, etc.)
```

**Kubernetes metrics (kube-state-metrics + cAdvisor):**
```
- Pod restarts (kube_pod_container_status_restarts_total)
- Pods in non-Running state (kube_pod_status_phase)
- OOMKilled events
- Node NotReady events (kube_node_status_condition)
- HPA current vs desired replicas
- PVC usage percentage
- Scheduler pending pods (kube_pod_status_scheduled)
- Deployment available replicas vs desired
```

**Application metrics (RED method):**
```
- Request rate (RPS) per endpoint and service
- Error rate (%) — break down by error type if possible
- Latency: p50 (median), p95, p99 — NEVER just average latency
  (average hides tail latency issues; use histograms)
```

**Business metrics (custom):**
```
- Orders per minute
- Payment success / failure rate
- Active users
- SQS queue depth and age
- Cache hit rate
- Background job processing rate
```

---

## SECTION 9 — SLI, SLO, SLA, Error Budgets

**Q: Explain SLI, SLO, SLA, and Error Budget.**

```
SLI (Service Level Indicator):
  - A quantitative measurement of service behaviour.
  - Examples: availability percentage, p99 latency, error rate, throughput.
  - "What are we measuring?"

SLO (Service Level Objective):
  - A target value or range for an SLI.
  - Agreed upon internally by the team.
  - Examples:
      Availability SLO: 99.9% of requests succeed over a rolling 30-day window.
      Latency SLO: p99 latency < 500ms for 95% of 5-minute windows.
  - "What is our target?"

SLA (Service Level Agreement):
  - A contractual agreement with customers about service levels.
  - Usually LESS strict than the SLO (SLO is your internal target to stay safe).
  - Violation has financial/legal consequences (service credits, refunds).
  - "What do we promise customers?"

Error Budget:
  - The amount of unreliability you are ALLOWED to have while still meeting your SLO.
  - Error budget = 1 - SLO target.
  - 99.9% availability SLO = 0.1% error budget = 43.8 minutes/month allowed downtime.
  - If error budget is consumed: stop feature development, focus on reliability.
  - If error budget is healthy: confident to ship and take risks.
  - Bridges gap between development (wants to ship) and ops (wants stability).
```

**Calculating error budget burn rate:**
```promql
# Current 30-day availability
(
  1 - (
    sum(rate(http_requests_total{status=~"5.."}[30d]))
    /
    sum(rate(http_requests_total[30d]))
  )
) * 100

# Error budget remaining (assuming 99.9% SLO)
# Budget = 0.1% of all requests
# Remaining = budget - consumed errors
```

---

## SECTION 10 — Prometheus Configuration

```yaml
# prometheus.yml — full configuration example
global:
  scrape_interval: 15s        # how often to scrape targets
  evaluation_interval: 15s    # how often to evaluate rules
  scrape_timeout: 10s

# Alert rules files
rule_files:
  - "rules/*.yml"

# Alertmanager connection
alerting:
  alertmanagers:
    - static_configs:
        - targets:
            - alertmanager:9093

# Scrape configurations
scrape_configs:
  # Prometheus itself
  - job_name: prometheus
    static_configs:
      - targets: ['localhost:9090']

  # All Kubernetes nodes via node_exporter
  - job_name: node-exporter
    static_configs:
      - targets:
          - node1.internal:9100
          - node2.internal:9100

  # Kubernetes service discovery — auto-discover all pods with annotation
  - job_name: kubernetes-pods
    kubernetes_sd_configs:
      - role: pod
    relabel_configs:
      # Only scrape pods with annotation prometheus.io/scrape: "true"
      - source_labels: [__meta_kubernetes_pod_annotation_prometheus_io_scrape]
        action: keep
        regex: 'true'
      # Use the annotated port if specified
      - source_labels: [__meta_kubernetes_pod_annotation_prometheus_io_port]
        action: replace
        target_label: __address__
        regex: (.+)
        replacement: ${1}
      # Add pod metadata as labels
      - source_labels: [__meta_kubernetes_namespace]
        target_label: namespace
      - source_labels: [__meta_kubernetes_pod_name]
        target_label: pod
      - source_labels: [__meta_kubernetes_pod_label_app]
        target_label: app

  # Kubernetes API server
  - job_name: kubernetes-apiservers
    kubernetes_sd_configs:
      - role: endpoints
    scheme: https
    tls_config:
      ca_file: /var/run/secrets/kubernetes.io/serviceaccount/ca.crt
    bearer_token_file: /var/run/secrets/kubernetes.io/serviceaccount/token
    relabel_configs:
      - source_labels: [__meta_kubernetes_namespace, __meta_kubernetes_service_name]
        action: keep
        regex: default;kubernetes
```

---

## SECTION 11 — Interview Scenarios

**Q: Production is slow. How do you investigate using monitoring?**

```
1. Check the RED dashboard:
   - Is RPS normal? Sudden spike could indicate traffic-driven degradation.
   - What is the error rate? HTTP 5xx? Timeout errors?
   - What is p99 latency? When did it start increasing?

2. Correlate with infrastructure:
   - Did a deployment happen around the same time? (check deployment annotations on graphs)
   - Is CPU, memory, or disk I/O spiking?
   - Are there pod restarts? OOMKilled events?

3. Check downstream dependencies:
   - Is the database slow? (query latency, connection pool usage)
   - Is the cache hit rate normal? (cache miss = more DB load)
   - Is the SQS queue growing? (consumers not keeping up)

4. Check logs:
   - Filter logs for errors and exceptions around the start time.
   - Look for patterns: same error message, same user, same endpoint.

5. Check distributed traces:
   - Find slow traces for the affected endpoint.
   - Which span/service is contributing most of the latency?
```

---

**Q: Design a monitoring setup for a new microservice. What would you instrument?**

```
Application metrics (instrument in code):
  - http_requests_total (counter, label: method, endpoint, status)
  - http_request_duration_seconds (histogram, label: method, endpoint)
  - active_db_connections (gauge)
  - cache_hits_total, cache_misses_total (counters)
  - background_jobs_processed_total, background_jobs_failed_total (counters)
  - any business metric relevant to this service

Infrastructure metrics (node_exporter, kube-state-metrics):
  - CPU, memory, disk per pod and node (automatic via Kubernetes)
  - Pod restart count, OOMKills

Logging:
  - Structured JSON logs to STDOUT
  - Every request logged: trace_id, user_id, endpoint, latency_ms, status
  - All errors logged with stack traces

Alerting:
  - Error rate > 1% for 5 minutes -> warning
  - Error rate > 5% for 5 minutes -> critical, page on-call
  - p99 latency > 1s for 10 minutes -> warning
  - p99 latency > 3s for 5 minutes -> critical
  - Pod restarts > 3 in 1 hour -> warning
  - Any DLQ messages -> critical

Dashboard (Grafana):
  - RED panels at top
  - Infrastructure panels in middle
  - Business metrics at bottom
  - Variables for environment and time range
```
