# Origin DevOps Intern — Interview Prep Master Index

## How to use this guide
Read every file top-to-bottom at least once.
Then re-read and answer every question *out loud* without looking at the answer.
Then do the hands-on tasks in `10-Hands-On-Write-These.md`.

## Files

| # | File | Core Topics |
|---|------|-------------|
| 01 | `01-Linux-Shell-Scripting.md` | Linux internals, signals, filesystem, shell scripting |
| 02 | `02-Docker-Deep-Dive.md` | Dockerfile, image layers, optimisation, networking |
| 03 | `03-Kubernetes.md` | Pods, Services, Deployments, HPA, networking |
| 04 | `04-CI-CD-Jenkins-GitHub-Actions.md` | Pipelines, Jenkinsfile, stages, GitHub Actions YAML |
| 05 | `05-AWS-Deep-Dive.md` | SQS, Lambda, EC2, S3, VPC, IAM, CloudWatch |
| 06 | `06-Networking.md` | TCP/IP, DNS, HTTP, load balancers, firewalls |
| 07 | `07-Python-Bash-Scripting.md` | Automation scripts, cron, common patterns |
| 08 | `08-Monitoring-Logging.md` | Prometheus, Grafana, ELK stack, alerting |
| 09 | `09-Terraform-Ansible-IaC.md` | IaC concepts, Terraform HCL, Ansible playbooks |
| 10 | `10-Hands-On-Write-These.md` | Write Dockerfile, Jenkinsfile, scripts from scratch |

## What the interviewer is testing
- Not just *what* you know, but *why* something works the way it does
- Trade-offs: why choose SQS over Lambda directly, why alpine over ubuntu, etc.
- Depth: can you go from "what is it" to "how does it work internally"
- Hands-on: can you write a Dockerfile, Jenkinsfile, or script on the spot

## Red flags the interviewer watches for
- Saying "I've heard of it" instead of explaining it
- Confusing similar services (SQS vs SNS vs Lambda, CMD vs ENTRYPOINT)
- Not knowing *why* a best practice exists, only *that* it exists
- Memorised answers with no depth — always prepare a "why" and a "trade-off"

## Study order (suggested)
1. Docker (02) — very likely to be asked hands-on
2. AWS (05) — SQS/Lambda deep dive from last time, expect more
3. Linux (01) — fundamentals they may test live
4. CI/CD (04) — write a Jenkinsfile on the spot
5. Kubernetes (03) — may ask you to debug a scenario
6. Networking (06) — conceptual questions
7. Monitoring (08) — dashboard and alerting questions
8. Terraform/Ansible (09) — IaC concepts
9. Python/Bash (07) — scripting questions
10. Hands-On (10) — practice writing everything cold

## Last interview recap (what was asked)
- Resume deep-dive: how did you reduce payload time, which AWS services used
- SQS internals: implementation, difference from Lambda, in-depth service comparison
- Dockerfile: how to reduce runtime and image size
- General DevOps core understanding at depth

## What to expect this time (likely harder)
- Advanced Linux: signals, processes, /proc, inode, syscalls
- Write a Dockerfile or Jenkinsfile from scratch on the spot
- Kubernetes: debug a broken pod scenario, explain networking
- Jenkins/GitHub Actions: explain pipeline stages, write a pipeline
- Python or Bash script: write a real automation script
- Networking: DNS, TCP, HTTP deep questions
- Monitoring: Prometheus queries, Grafana dashboard design
- Terraform: state management, modules, import