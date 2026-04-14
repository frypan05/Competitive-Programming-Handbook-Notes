# One2N SRE Interview Preparation - Daksh Sharma

---

## HOW TO USE THIS DOCUMENT

This document is organized in three sections mirroring One2N's interview stages:
1. **Introductory Call** - Personal and behavioral questions
2. **Programming Exercise** - What to expect and how to approach it
3. **Tech Discussion** - System design and open-ended scenarios

All answers are tailored to your resume. Treat them as a base to internalize, not scripts to memorize.

---

---

# PART 1: INTRODUCTORY CALL

---

## Section A: Your Introduction (5 mins)

---

### Q1. Tell us about yourself.

**Answer:**

"I am a final-year Computer Science student, graduating in June 2026, with a concentration in Data Science and Cloud Computing. My work sits at the intersection of backend engineering and DevOps - I care a lot about how software gets built, deployed, and kept alive in production.

I interned at Metagates Innovation where I built Django REST APIs for a healthcare platform, and more recently I worked as a Full Stack Engineer at HireZapp, where I owned an end-to-end event-driven job automation system using AWS SQS and Lambda. Outside of work, I have built personal projects ranging from a C++ CLI tool for research papers with Docker-based deployment, to a competitive programming platform with real-time WebSocket connections and containerized code execution.

What I find genuinely interesting about SRE is the intersection of software and systems thinking - reliability is not something you bolt on at the end, it has to be designed in. That is what draws me to One2N."

---

### Q2. Why One2N specifically?

**Answer:**

"A few reasons. First, One2N works on meaningful infrastructure problems for real clients - not toy systems. The kind of work described on your website, helping teams go from manual deployments to production-grade CI/CD, is exactly the kind of thing I have been doing in my own projects.

Second, the learning-oriented culture is something I actively look for. I picked up Go enough to understand infrastructure tooling, I taught myself Grafana Loki and Prometheus for observability on a personal project, and I am comfortable saying there is a lot I do not know yet. An environment where that is valued, not penalized, is where I want to be.

Third, the interview process itself signals good engineering culture. You ask about the 'why' and 'how', not linked-list reversal. That tells me something about how the team thinks."

---

### Q3. What are your strengths and weaknesses honestly?

**Answer:**

**Strengths:**
- I am good at owning things end-to-end. At HireZapp I did not just write code - I designed the SQS-Lambda architecture, deployed it, and monitored it.
- I am comfortable in ambiguity. Most of my projects started from scratch with no guidance.
- I pick up new tools quickly. I had not used Grafana Loki before the guidebook project - I integrated it in under a week.

**Weaknesses:**
- I have limited production SRE experience at scale. My largest system handled ~1,100 async jobs and 100 concurrent users - not thousands. I know this and I am actively reading about large-scale SRE patterns (SRE book, production Kubernetes war stories).
- I have not worked with Terraform deeply in production yet - I understand the concepts from coursework and documentation, but I am honest that I would need ramp-up time on complex Terraform state management at scale.

---

## Section B: Past Work Deep Dive (15 mins)

---

### Q4. Walk me through your most technically complex project and your personal contribution.

**Answer (HireZapp - event-driven automation):**

"The most technically interesting thing I built was the job application automation system at HireZapp. The problem was that recruiters were manually processing applications, which did not scale.

My contribution specifically: I designed and implemented the event-driven architecture from scratch. I chose AWS SQS as the message queue because we needed guaranteed delivery and the ability to handle spikes - a recruiter triggering 1,100 applications at once should not crash the system. Each application became a message in the queue, and Lambda functions consumed them asynchronously.

The key decision I made was to use SQS's visibility timeout and dead-letter queue to handle failures gracefully. If a Lambda invocation failed, the message became visible again and was retried automatically - without me writing retry logic manually.

I also built the mailing system using AWS SAM, which let me define the entire Lambda + API Gateway stack as code. The result was email dispatch time dropped by 65%.

What I learned: event-driven systems are powerful but you need to think carefully about idempotency. We had a bug early on where duplicate messages caused duplicate emails. I fixed it by adding a deduplication ID based on applicant+recruiter hash."

---

### Q5. Tell me about your CI/CD pipeline on the Guidebook project. Why did you make the choices you did?

**Answer:**

"The Guidebook project had a 3-stage GitHub Actions pipeline: linting, static analysis, and build verification. Let me explain the why behind each.

Linting first because it is the cheapest check - catches style violations before wasting compute on a full build. Static analysis second because it catches actual logic errors without running the code. Build verification last because it is the most expensive.

Branch protection rules meant nothing could merge to main without all three stages passing. This gave me 100% pipelined production releases - nothing went to production manually.

For deployment, I chose Vercel because it gave me immutable deployments out of the box. Every commit gets its own URL. If something breaks, rollback is instant - under 10 seconds - because you are just pointing DNS at the previous immutable build. This is infrastructure-as-code thinking at the deployment layer.

The trade-off I consciously made: Vercel is opinionated and vendor-specific. In a larger enterprise context, I would evaluate Kubernetes with ArgoCD for GitOps. But for this project, the simplicity of Vercel was the right call."

---

### Q6. What did you learn building the Chasing Cosmos CLI tool in C++?

**Answer:**

"A few things. First, distribution is hard. Writing a CLI in C++ means you need to handle different OS environments. I used Docker to create a consistent execution environment, and Windows Package Manager (winget) for distribution on Windows. The Docker containerization meant the tool ran identically regardless of the host system.

Second, I learned that performance optimization in C++ requires you to actually measure before you optimize. I used profiling to find that the bottleneck was I/O latency to the academic APIs, not my search algorithm. So the optimization was to parallelize requests, not rewrite the algorithm.

The GitHub Actions pipeline I set up for it automated building and releasing the binary - so a new version push triggered a release artifact automatically. That was my first real exposure to automated binary distribution."

---

### Q7. You used Grafana Loki and Prometheus - what problem were you solving and how?

**Answer:**

"On the Guidebook project, I added observability because I wanted to understand what was actually happening in the system - not just assume it was working. Prometheus scrapes metrics (request counts, error rates, latency), and Grafana provides the dashboards to visualize them. Loki is specifically for log aggregation - it indexes logs without parsing them, which is much cheaper than Elasticsearch for a project this size.

The practical thing I set up was alerting on build pipeline failures. If a deployment failed, I wanted to know before a user reported it.

What I realize now, working on this more deeply, is that observability is three pillars: metrics, logs, and traces. I had the first two covered. Distributed tracing with something like Jaeger or OpenTelemetry is the next layer I would add in a microservices context."

---

### Q8. How do you stay current with technology?

**Answer:**

"A few ways. I read the SRE book and the Google Site Reliability Workbook - they are the canonical references for production thinking. For current tooling, I follow the CNCF landscape to understand what is graduating vs incubating. I read Hacker News and specific Substack newsletters.

More practically: I build things. The Loki and Prometheus integration did not come from reading docs alone - it came from actually configuring dashboards and breaking things. I also contribute to open source when I find bugs or gaps in documentation, because reading code is the fastest way to understand how a tool actually works versus how its docs say it works.

I am also honest about the limits of this - there is a gap between what I have read about Kubernetes at scale and what I have actually operated at scale. That is a gap I want to close in a role like this."

---

---

# PART 2: PROGRAMMING EXERCISE

---

## What to Expect for SRE Role

One2N has listed these as likely problem statements for SRE:

- Write infrastructure as code using Terraform
- Write Ansible playbooks for automating some tasks
- Write a simple CRUD API and deploy it using simple automation on VM/Docker
- Write CI/CD code for a sample web application repo
- Create Helm charts or Kubernetes manifest files
- Write Bash scripts for automating common tasks on Linux VMs
- Live-debug why an application deployed on AWS using ALB + VMs is not healthy

---

## Section A: Bash Scripting

---

### Q9. Write a bash script that monitors disk usage and sends an alert if any partition exceeds 80%.

**Answer (approach + code):**

```bash
#!/bin/bash
# disk_monitor.sh
# Monitors disk usage across all mounted partitions and logs/alerts if threshold exceeded.

THRESHOLD=80
LOGFILE="/var/log/disk_monitor.log"
ALERT_EMAIL="ops@company.com"
TIMESTAMP=$(date '+%Y-%m-%d %H:%M:%S')

check_disk() {
    # df -h gives human-readable, -x filters virtual filesystems
    df -h -x tmpfs -x devtmpfs | awk 'NR>1 {
        # Remove the % sign and extract usage percentage
        gsub(/%/, "", $5)
        print $5, $6
    }' | while read usage mount; do
        if [ "$usage" -ge "$THRESHOLD" ]; then
            MESSAGE="[$TIMESTAMP] ALERT: $mount is at ${usage}% usage (threshold: ${THRESHOLD}%)"
            echo "$MESSAGE" >> "$LOGFILE"
            # Send email if mail utility is available
            if command -v mail &>/dev/null; then
                echo "$MESSAGE" | mail -s "Disk Alert: $mount" "$ALERT_EMAIL"
            fi
        fi
    done
}

check_disk
```

**Key talking points:**
- Used `-x tmpfs -x devtmpfs` to filter pseudo-filesystems - otherwise you get noise
- `awk NR>1` skips the header row
- `command -v mail` checks if the mail binary exists before calling it - defensive scripting
- In production, this would be a cron job: `*/5 * * * * /usr/local/bin/disk_monitor.sh`

---

### Q10. How would you write a script to automate log rotation for an application that does not use logrotate?

**Answer:**

```bash
#!/bin/bash
# rotate_logs.sh
# Manual log rotation for applications that don't use logrotate

APP_LOG="/var/log/myapp/app.log"
ARCHIVE_DIR="/var/log/myapp/archive"
RETENTION_DAYS=30
MAX_SIZE_MB=100

mkdir -p "$ARCHIVE_DIR"

rotate_if_large() {
    local file="$1"
    local size_mb
    size_mb=$(du -m "$file" 2>/dev/null | cut -f1)

    if [ "${size_mb:-0}" -ge "$MAX_SIZE_MB" ]; then
        local timestamp
        timestamp=$(date '+%Y%m%d_%H%M%S')
        local archived="${ARCHIVE_DIR}/app_${timestamp}.log.gz"
        
        # gzip and move atomically
        gzip -c "$file" > "$archived"
        # Truncate the original file without restarting the app (handles open file handles)
        > "$file"
        echo "Rotated $file to $archived (was ${size_mb}MB)"
    fi
}

cleanup_old_logs() {
    find "$ARCHIVE_DIR" -name "*.log.gz" -mtime +"$RETENTION_DAYS" -delete
    echo "Cleaned up logs older than $RETENTION_DAYS days"
}

rotate_if_large "$APP_LOG"
cleanup_old_logs
```

**Key talking point:** Truncating with `> "$file"` instead of deleting and recreating is important - if the application has an open file descriptor to the log, deleting it leaves an unlinked file still being written to. Truncation keeps the inode intact.

---

## Section B: Docker and Containerization

---

### Q11. Write a Dockerfile for a Python Django application with production best practices.

**Answer:**

```dockerfile
# Multi-stage build to keep final image small
FROM python:3.11-slim AS builder

WORKDIR /app

# Copy only requirements first (layer caching - deps change less often than code)
COPY requirements.txt .
RUN pip install --no-cache-dir --user -r requirements.txt

# Final stage
FROM python:3.11-slim

# Create non-root user - never run app as root
RUN useradd --create-home --shell /bin/bash appuser

WORKDIR /app

# Copy installed packages from builder
COPY --from=builder /root/.local /home/appuser/.local

# Copy application code
COPY --chown=appuser:appuser . .

USER appuser

# PATH for user-installed packages
ENV PATH=/home/appuser/.local/bin:$PATH

# Don't use development server in production
CMD ["gunicorn", "--bind", "0.0.0.0:8000", "--workers", "4", "myapp.wsgi:application"]

EXPOSE 8000
```

**What to discuss:**
- Multi-stage builds: builder stage installs deps; final stage is lean
- Non-root user: security baseline
- Layer ordering: requirements.txt before source code so code changes don't invalidate the dependency cache
- Gunicorn, not `manage.py runserver` - the dev server is single-threaded and not production-safe

---

### Q12. Write a docker-compose.yml for a typical web app stack (Django + PostgreSQL + Redis).

**Answer:**

```yaml
version: "3.9"

services:
  db:
    image: postgres:15-alpine
    environment:
      POSTGRES_DB: appdb
      POSTGRES_USER: appuser
      POSTGRES_PASSWORD_FILE: /run/secrets/db_password
    volumes:
      - postgres_data:/var/lib/postgresql/data
    healthcheck:
      test: ["CMD-SHELL", "pg_isready -U appuser -d appdb"]
      interval: 10s
      timeout: 5s
      retries: 5

  redis:
    image: redis:7-alpine
    command: redis-server --maxmemory 256mb --maxmemory-policy allkeys-lru
    healthcheck:
      test: ["CMD", "redis-cli", "ping"]
      interval: 10s
      timeout: 3s
      retries: 3

  web:
    build: .
    environment:
      DATABASE_URL: postgresql://appuser@db:5432/appdb
      REDIS_URL: redis://redis:6379/0
    depends_on:
      db:
        condition: service_healthy
      redis:
        condition: service_healthy
    ports:
      - "8000:8000"
    volumes:
      - static_files:/app/staticfiles

volumes:
  postgres_data:
  static_files:
```

**What to discuss:**
- `depends_on` with `condition: service_healthy` means web does not start until DB is actually ready, not just started
- `POSTGRES_PASSWORD_FILE` points to a Docker secret instead of hardcoding the password
- Redis `maxmemory-policy allkeys-lru` prevents unbounded memory growth

---

## Section C: CI/CD

---

### Q13. Write a GitHub Actions workflow for a Python application with lint, test, and Docker build stages.

**Answer:**

```yaml
name: CI Pipeline

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  lint:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-python@v4
        with:
          python-version: "3.11"
          cache: "pip"
      - run: pip install flake8 black isort
      - run: |
          flake8 . --max-line-length=120
          black --check .
          isort --check-only .

  test:
    runs-on: ubuntu-latest
    needs: lint
    services:
      postgres:
        image: postgres:15
        env:
          POSTGRES_PASSWORD: testpass
          POSTGRES_DB: testdb
        options: >-
          --health-cmd pg_isready
          --health-interval 10s
          --health-timeout 5s
          --health-retries 5
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-python@v4
        with:
          python-version: "3.11"
          cache: "pip"
      - run: pip install -r requirements.txt
      - run: pytest --cov=. --cov-report=xml
        env:
          DATABASE_URL: postgresql://postgres:testpass@localhost:5432/testdb
      - uses: codecov/codecov-action@v3

  docker-build:
    runs-on: ubuntu-latest
    needs: test
    if: github.ref == 'refs/heads/main'
    steps:
      - uses: actions/checkout@v4
      - uses: docker/setup-buildx-action@v3
      - uses: docker/login-action@v3
        with:
          username: ${{ secrets.DOCKERHUB_USERNAME }}
          password: ${{ secrets.DOCKERHUB_TOKEN }}
      - uses: docker/build-push-action@v5
        with:
          push: true
          tags: myapp:${{ github.sha }},myapp:latest
          cache-from: type=gha
          cache-to: type=gha,mode=max
```

**Key talking points:**
- `needs: lint` chains stages - test only runs if lint passes
- GitHub Actions service containers for Postgres - real integration tests, not mocked
- Docker layer caching with `type=gha` - reuses layers across CI runs, much faster builds
- Tags with `github.sha` - every image is immutably traceable to a commit

---
## Application Load Balancer
ALB` stands for Application Load Balancer — an AWS Elastic Load Balancing (ELB) product that operates at the application (Layer 7) level. If you’ve deployed a web app on AWS, an ALB commonly sits in front of your servers or containers and routes user requests to the appropriate back-end targets. Here’s a concise breakdown so you know what it does and when to use it.

What an ALB does
- Routes HTTP/HTTPS (Layer 7) traffic based on content: path-based routing (e.g., `/api/*` → service A, `/app/*` → service B), host-based routing (e.g., `api.example.com` → service A), query/HTTP header rules, etc.
- Terminates TLS (SSL) so your backend can run plain HTTP if you want — certificate management typically uses AWS Certificate Manager (ACM).
- Supports WebSockets and HTTP/2 (and gRPC over HTTP/2 in supported setups).
- Performs health checks and only sends traffic to healthy targets.
- Integrates with target groups (EC2 instances, IPs, ECS tasks, Lambda functions) and autoscaling.
- Provides sticky sessions (session affinity) and deregistration delay (connection draining) to allow in-flight requests to finish.

When to use ALB vs other AWS load balancers
- Use ALB when you need application-layer routing or HTTP-specific features (host/path routing, headers, redirects, WebSocket).
- Use Network Load Balancer (NLB) when you need ultra-high throughput, static IPs, or TCP/UDP/ TLS pass-through (Layer 4).
- Classic Load Balancer (CLB) is legacy — ALB is preferred for modern HTTP apps.

Typical architecture in a deployed app
- Internet → Route 53 (optional) → ALB (public subnets) → Target Group(s) → EC2/ECS/EKS/Lambda (private subnets).
- ALB has listeners (e.g., 80/443) and listener rules mapping to target groups.
- You usually put ALB in public subnets with security groups allowing inbound HTTP/HTTPS and restrict backend security groups to accept only ALB traffic.

Key things to configure or check
- Listeners and listener rules (paths/hosts).
- Target groups and health check path & thresholds.
- TLS certificate in ACM and HTTPS listener for secure traffic.
- Security groups: allow ALB inbound, backends allow traffic from ALB security group.
- Auto Scaling / ECS service registration so targets are added/removed automatically.
- Logging (access logs), metrics (CloudWatch), and optionally AWS WAF for protection.

Operational notes & best practices
- Use Route 53 alias records to point your custom domain to ALB’s DNS name (ALB has no fixed IP).
- Choose appropriate health check path and thresholds to avoid false deregistrations.
- Enable access logs for troubleshooting and security auditing.
- Use multiple AZs with ALB for high availability.
- Consider sticky sessions only if you can’t make your app stateless — prefer stateless apps + shared session store.
- Watch cost: ALB charges per hour + per GB processed.

## Section D: Live Debugging

---

### Q14. An application deployed on AWS behind an ALB is showing unhealthy targets. Walk through your debugging process.

**Answer:**

"I approach this systematically, starting at the load balancer and working inward.

**Step 1: Check ALB target group health in AWS Console**
Look at what the health check failure reason is. ALB gives you a reason code: `Target.Timeout`, `Target.FailedHealthChecks`, `Elb.InternalError`, etc. This narrows it down significantly.

**Step 2: Verify the health check configuration**
- Is the health check path correct? (`/health` vs `/healthz` vs `/`)
- Is the port correct? (ALB might be checking port 80 but app is on 8080)
- Is the threshold too aggressive? (2 consecutive failures mark unhealthy by default)

**Step 3: SSH into the VM and check if the app is running**
```bash
# Check if process is alive
ps aux | grep myapp
systemctl status myapp

# Check if port is listening
ss -tlnp | grep 8000

# Test the health check endpoint directly
curl -v http://localhost:8000/health
```

**Step 4: Check application logs**
```bash
journalctl -u myapp -n 100 --no-pager
tail -f /var/log/myapp/app.log
```

**Step 5: Check security groups**
The ALB's security group needs an outbound rule to the VM's security group on the app port. This is a common misconfiguration - the ALB can accept traffic from the internet but cannot reach the backend.

**Step 6: Check if the VM is CPU/memory starved**
```bash
top
free -m
df -h  # Disk full causes weird failures
```

**Common root causes I have seen:**
- App crashed and systemd did not restart it (check `Restart=always` in unit file)
- Health check path returns 200 only after DB connection - if DB is down, health check fails
- Security group rules blocking ALB-to-VM traffic
- App is listening on `127.0.0.1` instead of `0.0.0.0`"

---

---

# PART 3: TECH DISCUSSION (SYSTEM DESIGN)

---

## Scenario 1: Productionising a Monolith on a Single VM

---

### Q15. How do you "productionise" a monolith deployed manually on a single VM with no CI/CD, no security, no scaling?

**Answer framework - think in layers:**

**Layer 1: Observability first (you cannot fix what you cannot see)**
- Deploy Prometheus + Grafana for metrics (CPU, memory, request rate, error rate, latency)
- Add Loki or ELK for centralized log aggregation
- Define SLIs: what does "working" mean? (e.g., p99 latency < 500ms, error rate < 0.1%)
- Set up alerting on SLO breaches before making changes

**Layer 2: Infrastructure as Code**
- Write Terraform to codify the existing VM, security groups, and networking
- Even if you are not changing the infra, you need it in code so you can recreate it
- Store Terraform state in S3 with DynamoDB locking

**Layer 3: CI/CD**
- Set up a staging environment identical to production
- GitHub Actions pipeline: lint -> test -> build -> deploy to staging -> manual approval -> deploy to production
- Deployment strategy: start with rolling deploys, graduate to blue/green when you have load balancer in front

**Layer 4: Security baseline**
- Security groups: principle of least privilege - only port 443 from the internet, app port only from ALB
- Secrets management: move all secrets from .env files to AWS Secrets Manager
- IAM roles for EC2 instead of access keys hardcoded in the app
- Enable AWS CloudTrail for audit logging

**Layer 5: Reliability**
- Put an ALB in front of the VM
- Move to an ASG (Auto Scaling Group) with minimum 2 instances for redundancy
- Set up RDS Multi-AZ for the database instead of a DB on the same VM
- Define and test backup and restore procedures

**Layer 6: On-call readiness**
- Write runbooks for every alert
- Define incident severity levels and escalation paths
- Conduct a game day (chaos engineering) to find failure modes before users do

**Trade-offs to mention:**
"I would not do all of this at once. The order matters. Observability first means every subsequent change is visible. CI/CD second means I stop accruing deployment risk. Security and reliability can be parallelized by different team members. The biggest risk is trying to change the architecture and the process simultaneously."

---

## Scenario 2: Improving Availability to 99.5%

---

### Q16. How do you go from unknown availability to 99.5% SLO?

**Answer:**

**Step 1: Define what you are measuring**
"99.5% of what? The first thing I do is define the SLI (Service Level Indicator). For a web service, the canonical SLI is: `(successful requests / total requests) * 100` over a rolling 28-day window. 99.5% means you can afford roughly 3.6 hours of downtime per month.

**Step 2: Establish a baseline**
You cannot improve availability if you do not know what it is currently. I would add synthetic monitoring (a cron job that hits a production endpoint every 60 seconds and records success/failure) and look at historical error logs to estimate current availability.

**Step 3: Identify the failure modes**
Build a failure mode catalog:
- Application crashes (single process, no redundancy)
- Database goes down (single point of failure)
- VM runs out of disk
- Deployment goes wrong and breaks production
- Network partition

**Step 4: Eliminate single points of failure**
- Application: run minimum 2 instances behind a load balancer
- Database: RDS Multi-AZ (automatic failover in ~60 seconds)
- Deployment: blue/green deployment so rollback is instant

**Step 5: Automate failure recovery**
- Systemd with `Restart=always` for process crashes
- ASG health checks: replace unhealthy instances automatically
- Circuit breakers in the application layer for downstream dependencies

**Step 6: Measure and iterate**
After each change, measure the SLI against the SLO. Use error budget - if you have consumed 80% of your error budget, freeze risky deployments."

---

## Scenario 3: CI/CD for Microservices

---

### Q17. How do you implement CI/CD for a new microservices-based project?

**Answer:**

"Microservices CI/CD has specific challenges that a monolith does not: independent deploy cadences, service dependencies, and contract testing.

**Repository strategy:**
I would use a mono-repo for small teams (easier to make cross-service changes atomically) or polyrepo for larger teams with clear service ownership. I would avoid the in-between - partial mono-repo is the worst of both worlds.

**Per-service CI pipeline:**
Each service gets its own pipeline trigger (path filters in GitHub Actions):
```yaml
on:
  push:
    paths: ['services/user-service/**']
```

Stages: lint -> unit tests -> integration tests (against a test DB) -> build Docker image -> push to ECR with `git sha` tag.

**Contract testing:**
This is the critical piece most people skip. When Service A calls Service B, you need consumer-driven contract tests (Pact is the common tool). Without this, you discover integration failures in staging, not in CI.

**CD strategy:**
- For staging: automatic deploy on merge to main
- For production: GitOps with ArgoCD. ArgoCD watches a Helm values file in a git repo. To deploy to production, you open a PR changing the image tag. Merge = deploy. This gives you a complete audit trail.

**Kubernetes considerations:**
- Each service gets a Helm chart with configurable replicas, resource limits, and liveness/readiness probes
- Readiness probes prevent traffic to a pod until it is actually ready (this is how you get zero-downtime rolling deploys)
- Resource limits prevent one service from starving another on the same node

**Trade-off to mention:**
"Independent deployability is the goal, but you pay for it with orchestration complexity. I would start simple (single pipeline, deploy all services together) and introduce independent deploys only when release coordination becomes a bottleneck."

---

## Scenario 4: Scaling a Backend Application

---

### Q18. How do you architect a backend (monolith APIs + background workers) to scale out?

**Answer:**

"I think about this in terms of which components have different scaling characteristics.

**Identify the bottlenecks first:**
- Is the API CPU-bound (computation) or I/O-bound (DB queries, external calls)?
- Are background workers CPU-bound (ML inference) or I/O-bound (sending emails)?
- Where is the database - is it the bottleneck?

**API tier:**
- Horizontal scaling behind an ALB. Stateless design is a prerequisite - session state in Redis, not in-process.
- Auto Scaling Group based on CPU or request count metrics.
- CDN (CloudFront) in front for static assets and cacheable API responses.

**Background worker tier:**
- Scale independently from the API. Workers pull from SQS or RabbitMQ.
- SQS-based scaling: CloudWatch alarm on `ApproximateNumberOfMessagesVisible` metric triggers ASG scale-out.
- At HireZapp I did exactly this - Lambda consumers scaled automatically based on queue depth.

**Database tier - the hardest part:**
- Read replicas for read-heavy workloads (most CRUD apps are 80% reads).
- Connection pooling with PgBouncer to avoid exhausting Postgres connection limits at scale.
- Caching layer with Redis: cache expensive, frequently-read, rarely-changed data.
- If write throughput is the bottleneck, consider CQRS (Command Query Responsibility Segregation) - write to primary, read from replica/cache.

**What I would not do prematurely:**
Sharding. Horizontal sharding is complex to implement and even harder to unwind. I would exhaust vertical scaling, read replicas, and caching before going there."

---

## Section D: API and Database Design

---

### Q19. Design a REST API and database schema for an OTP-based auth system.

**Answer:**

**Database schema:**

```sql
CREATE TABLE users (
    id          UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    phone       VARCHAR(15) UNIQUE NOT NULL,  -- E.164 format
    email       VARCHAR(255) UNIQUE,
    created_at  TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    is_active   BOOLEAN NOT NULL DEFAULT TRUE
);

CREATE TABLE otp_requests (
    id          UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    user_id     UUID REFERENCES users(id),
    phone       VARCHAR(15) NOT NULL,          -- denormalized for pre-registration OTPs
    otp_hash    VARCHAR(64) NOT NULL,          -- bcrypt hash, NEVER store plaintext OTP
    purpose     VARCHAR(50) NOT NULL,          -- 'login', 'registration', 'password_reset'
    expires_at  TIMESTAMPTZ NOT NULL,
    attempts    INTEGER NOT NULL DEFAULT 0,
    is_used     BOOLEAN NOT NULL DEFAULT FALSE,
    created_at  TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

CREATE INDEX idx_otp_phone_expires ON otp_requests(phone, expires_at) WHERE NOT is_used;

CREATE TABLE sessions (
    id              UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    user_id         UUID REFERENCES users(id) ON DELETE CASCADE,
    token_hash      VARCHAR(64) NOT NULL UNIQUE,  -- hash of JWT or opaque token
    device_info     JSONB,
    expires_at      TIMESTAMPTZ NOT NULL,
    created_at      TIMESTAMPTZ NOT NULL DEFAULT NOW()
);
```

**API design:**

```
POST /auth/otp/request
Body: { "phone": "+919911569478" }
Response: { "otp_request_id": "uuid", "expires_in": 300 }
Rate limit: 3 requests per phone per 10 minutes

POST /auth/otp/verify
Body: { "otp_request_id": "uuid", "otp": "123456" }
Response: { "access_token": "jwt", "refresh_token": "opaque" }
- Increment attempt counter on failure
- Lock after 5 failed attempts

POST /auth/token/refresh
Authorization: Bearer <refresh_token>
Response: { "access_token": "jwt" }

DELETE /auth/session
Authorization: Bearer <access_token>
Response: 204 No Content
```

**Security decisions to call out:**
- OTP is stored as a bcrypt hash, never plaintext - if the DB leaks, OTPs are not exposed
- `attempts` counter prevents brute force (6-digit OTP has 1 million combinations; without limiting, you can brute force in ~500 requests)
- `expires_at` is short (5 minutes) to reduce the window for interception
- `otp_request_id` in the verify request prevents enumeration of which phone numbers are registered
- Rate limiting at the API gateway layer (not just DB) to prevent DoS

---

## Section E: Debugging Slow Applications

---

### Q20. A three-tier app (JS frontend + REST API + relational DB) is very slow overall. How do you find the problem?

**Answer:**

"I approach performance debugging with the scientific method: measure, hypothesize, test, fix.

**Step 1: Locate the bottleneck tier**
Use browser DevTools Network tab to measure:
- Time to first byte (TTFB): if high, the problem is in the API or DB, not the frontend
- Asset load time: if high, it might be frontend or CDN

Add timing middleware to the API that logs request duration. Then look at the histogram:
- If 99th percentile is 10x the 50th percentile, you have outlier requests (probably missing DB index)
- If all requests are slow, you have a systemic issue (connection pool exhausted, under-provisioned VM)

**Step 2: Profile the API**
Add structured logging that captures:
- Total request duration
- DB query count and total DB time
- External API call durations

The N+1 query problem is the most common culprit. If you are loading a list of orders and then querying each order's user separately, you do N+1 queries. Fix: JOIN or use `select_related` in Django ORM.

**Step 3: Profile the DB**
```sql
-- Enable slow query log
-- MySQL:
SET GLOBAL slow_query_log = 'ON';
SET GLOBAL long_query_time = 0.5;  -- Log queries > 500ms

-- PostgreSQL:
-- pg_stat_statements extension
SELECT query, mean_exec_time, calls
FROM pg_stat_statements
ORDER BY mean_exec_time DESC
LIMIT 20;
```

Then use `EXPLAIN ANALYZE` on the slow queries to check if indexes are being used.

**Step 4: Check infrastructure**
- Is the VM CPU-throttled? (T2/T3 burstable instances exhaust CPU credits)
- Is the DB connection pool full? (requests queue waiting for a connection)
- Is there memory pressure causing swap?

**Step 5: Fix in priority order**
Quick wins first: add missing indexes, fix N+1 queries, add caching for expensive repeated queries. Then address architectural issues if needed."

---

---

# PART 4: BEHAVIORAL / COLLABORATION QUESTIONS

---

### Q21. Tell me about a time you dealt with significant unknowns in a project.

**Answer:**

"When I built the Chasing Cosmos CLI tool, I had never written a tool that needed to work reliably across completely different OS environments - Windows, Linux, and macOS. I did not know how to solve distribution.

I broke the unknown into smaller unknowns: what does 'reliably across environments' actually mean? It means the same binary behavior. Docker solved the environment problem. But then I had the distribution unknown: how does a researcher on Windows install a Docker-based CLI? I researched Windows Package Manager (winget) and found that it could install Docker and then pull and run my container image with one command.

The key thing I learned was to not be paralyzed by the full unknown. I carved off the first 20% I could solve (containerization), solved it, and that opened up the path to the next 20% (distribution). Unknowns compound in the other direction too - solving one often dissolves two others."

---

### Q22. How do you handle a situation where you disagree with a technical decision?

**Answer:**

"I try to separate the decision from the ego. If I disagree, I will raise it once with clear reasoning and the trade-offs I see. At HireZapp, there was a point where we were considering polling instead of SQS for the job automation - simpler to implement but would not scale and would hammer the database. I raised it, explained the scaling concern with specific numbers, and proposed the SQS architecture.

If the decision is made against my recommendation, I commit fully unless it is something that compromises security or causes user harm. Partial commitment is worse than full disagreement - it gives you the worst of both outcomes.

What I want to avoid is being right in a way that damages the team. Technical correctness without effective communication is not actually useful."

---

### Q23. What questions do you have for us?

**Suggested questions to ask One2N:**

1. "What does a typical engagement look like for an SRE consultant at One2N? Are you usually embedded in a client team, or do you work from the outside as an advisor?"

2. "What is the biggest technical challenge your SRE consultants face most frequently with clients? Is it organizational (convincing teams to adopt practices) or technical (implementing them)?"

3. "How does One2N handle the knowledge transfer problem - when an engagement ends, how do you ensure the client team can maintain what you built?"

4. "What does the learning path look like for a junior SRE joining One2N? What would you expect me to be owning independently in 6 months?"

---

---

# QUICK REFERENCE: KEY NUMBERS FROM YOUR RESUME

Use these confidently in answers - they are yours to own.

| Achievement | Metric |
|---|---|
| Response time optimization (HireZapp) | 1.8s to 500ms |
| Email dispatch time reduction | 65% |
| Job applications processed async | 1,100+ per recruiter |
| Research papers accessible (CLI tool) | 100+ million |
| Literature review time reduction | 90% |
| CI/CD: 100% pipelined releases | Zero manual deploys |
| Deployment rollback time (Vercel) | < 10 seconds |
| Concurrent users (Coderr) | 100+ |
| Daily code submissions (Coderr) | 1,000+ |
| API response time (Metagates) | 5s to under 2s |
| Customer data accuracy improvement | 35% |
| Operational efficiency (HireZapp scripts) | 60% |

---

*Last updated: March 2026 | Prepared for One2N SRE Interview*
