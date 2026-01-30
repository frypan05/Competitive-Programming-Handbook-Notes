# Load Balancers — In-depth Guide for Beginners
This file (`load-balancers.md`) explains AWS Elastic Load Balancers (ELB) in depth for a fresher. I’ll cover concepts, types, components, real-world scenarios, configuration examples, troubleshooting, and recommended monitoring. Read it as a practical handbook you can return to while building and operating services.

---

## Quick summary — what a load balancer does
A load balancer accepts incoming traffic and distributes it across multiple backend targets (EC2 instances, IP addresses, or Lambda functions). It:
- Improves availability by removing unhealthy targets.
- Improves scalability by distributing load across many targets.
- Enables routing logic (host/path-based routing), TLS termination, and sticky sessions when needed.

---

## ELB family — types and when to use each
- Application Load Balancer (ALB)
  - Layer 7 (HTTP/HTTPS)
  - Use for web apps, microservices, host/path routing, header-based rules, WebSockets.
  - Integrates with AWS WAF, supports content-based routing.
- Network Load Balancer (NLB)
  - Layer 4 (TCP/UDP)
  - Use for high-performance, low-latency TCP/UDP workloads, preserving client IP, and when you need static IPs per AZ.
  - Can handle millions of requests per second.
- Classic Load Balancer (CLB)
  - Legacy; supports basic Layer 4 and Layer 7 features. Avoid for new designs; prefer ALB or NLB.

---

## Key components & concepts
- Listener: port and protocol (e.g., `HTTP :80` or `HTTPS :443`) that the load balancer accepts.
- Target Group: a set of targets (instances, IPs, or Lambdas) registered with health check settings. ALB and NLB route to target groups.
- Health Checks: periodic checks (HTTP path, TCP connect, gRPC) that determine target health. Unhealthy targets are removed from rotation automatically.
- Listener Rules (ALB): evaluate host/path/headers and forward to different target groups.
- Sticky Sessions (Session Affinity): client requests are sent to the same target for the session lifetime (ALB supports cookie-based stickiness).
- TLS Termination: decrypt TLS on the load balancer so backends can use HTTP (simplifies cert management).
- Connection Draining / Deregistration Delay: when a target is removed, existing connections are allowed to finish before terminating.

---

## Target types and registration
- `instance` target type: register EC2 instance IDs; traffic uses instance’s port.
- `ip` target type: register specific IP addresses (useful for ECS awsvpc mode, on-prem, or cross-VPC targets).
- `lambda` target type: ALB can invoke Lambda functions for HTTP requests.

---

## Health checks — best practices
- Use application-level checks (HTTP 200 on a `/health` endpoint) rather than only EC2 status checks. This ensures the application stack (web server, app, DB connectivity) is healthy.
- Keep the health-check path lightweight and fast (no DB-heavy checks unless required).
- Tune thresholds (healthy/unhealthy thresholds, interval) to balance fast recovery vs false positives.

---

## Common scenarios & step-by-step examples

1) Simple web app with path-based routing (ALB)
- Problem: You host two services:
  - `www.example.com` → static front-end (S3 or EC2)
  - `www.example.com/api` → backend API service (EC2/ECS)
- Solution:
  - Create ALB with a listener on `:80` (or `:443` with TLS).
  - Create two target groups: `tg-frontend` and `tg-api`.
  - Add listener rule: path `/api/*` → `tg-api`; default → `tg-frontend`.
- Benefit: Clean separation of traffic, independent scaling and health checks.

2) Blue/Green deployment with ALB target groups
- Problem: Deploy a new version with zero downtime and quick rollback.
- Solution:
  - Create `blue` target group (current version) and `green` target group (new version).
  - Launch new instances (or containers) and register with `green`.
  - Shift traffic by changing listener rule weights or swapping default target group to `green`.
  - If issues happen, switch back to `blue`.
- Benefit: Safe deployments and fast rollbacks.

3) Low-latency TCP service (NLB)
- Problem: You run a game server or a database proxy requiring high throughput and preserving the client IP.
- Solution:
  - Use NLB in front of the service with TCP listener on required port.
  - Configure target group with health checks (TCP connect).
  - NLB supports static IPs and handles millions of concurrent connections.

4) Internal service in a VPC using NLB and IP targets
- Problem: You need to load balance services across EC2s in private subnets and expose a single static endpoint to other VPCs or on-prem.
- Solution:
  - Create an internal NLB with `ip` target type (register private IPs).
  - Share NLB's static IP or use cross-account access / VPC peering as needed.

---

## Example: ALB listener rule (conceptual)
Use this conceptual snippet to illustrate a listener rule configuration. (This is an example; adapt to your IaC or CLI workflow.)
```/dev/null/load-balancer-alb-rule.example#L1-8
# Conceptual: Listener rule pseudocode (ALB)
If request.host == "www.example.com" and request.path starts_with "/api" then
  forward to target-group "tg-api"
else
  forward to target-group "tg-frontend"
```

---

## Example: NLB use-case (conceptual)
```/dev/null/load-balancer-nlb.example#L1-6
# Conceptual: NLB configuration for TCP game server
- NLB listener: TCP :25565
- Target group: TCP, targets = [10.0.1.12:25565, 10.0.2.15:25565]
- Health check: TCP on port 25565, interval 10s, healthy threshold 3
```

---

## Integration with Auto Scaling Groups (ASG)
- Register the ASG with a target group so new instances are automatically added and removed.
- Use ELB health checks in the ASG so EC2 instances failing ALB health checks are replaced.
- When scaling in, the load balancer honors deregistration delay so requests are drained before termination.

Scenario: ASG + ALB for web app
- ASG min=2, desired=4, max=10. ALB routes traffic and performs HTTP health checks on `/health`.
- When load increases, ASG scales out. ALB health checks ensure only ready instances serve traffic.

---

## TLS, certificates, and termination
- Prefer terminating TLS at ALB/NLB with TLS listener (ALB supports certificates via AWS Certificate Manager — ACM).
- Benefits of TLS termination on ALB:
  - Simplified certificate management (use ACM).
  - Offload CPU-heavy TLS work from backend servers.
- If end-to-end encryption is required, enable TLS between LB and backend or use mTLS as appropriate.

---

## Sticky sessions and stateful applications
- Sticky sessions bind a client to a target using cookies (ALB) or application-specific cookies.
- Use sticky sessions only when necessary (e.g., legacy apps that store session state on instance). Prefer stateless services with shared session stores (Redis, DynamoDB) to scale better.

---

## Metrics to monitor (CloudWatch)
- ALB metrics:
  - `RequestCount`, `HTTPCode_Target_5XX_Count`, `TargetResponseTime`, `TargetConnectionErrorCount`, `ActiveConnectionCount`.
- NLB metrics:
  - `NewFlowCount`, `ActiveFlowCount`, `TCP_Client_Reset_Count`, `TCP_Target_Reset_Count`.
- Target group metrics:
  - `HealthyHostCount`, `UnHealthyHostCount`.
- Alarms to set:
  - High 5xx rate or increasing target response time.
  - Healthy host count dropping below expected threshold.
  - Sudden spike in error rates or connection resets.

---

## Troubleshooting checklist
- 5xx errors:
  - Check target application logs.
  - Inspect `TargetResponseTime` — slow backends may time out.
- Targets report as unhealthy:
  - Verify health-check path and port.
  - Ensure security groups and NACLs allow health-check traffic.
- Sticky sessions not working:
  - Check that the load balancer has stickiness enabled and cookie settings match app requirements.
- TLS handshake failures:
  - Validate certificate chain, TLS policy, and ciphers.
- No traffic reaches instances:
  - Verify listener and target group configuration.
  - Ensure instance security groups allow inbound from the LB (ALB/NLB uses different source IP behavior).

---

## Security considerations
- Use security groups to restrict access. For public ALB:
  - Allow inbound 80/443 from 0.0.0.0/0 (if public).
  - Backends should only allow inbound traffic from the load balancer security group (not 0.0.0.0/0).
- Use AWS WAF with ALB to block common web threats and rate-limit abusive clients.
- Use IAM least-privilege for automating LB management.

---

## Cost considerations
- ALB: billed per hour + LCU (Load Balancer Capacity Units) for processed traffic/features.
- NLB: billed per hour + per GB processed; static IPs may have additional charges.
- Design with right-sizing in mind: avoid unnecessarily large LBs; leverage caching and edge layers (CloudFront) to reduce LB load.

---

## Best practices (summary)
- Prefer ALB for HTTP/HTTPS traffic and NLB for TCP/UDP/high-performance needs.
- Use application-level health checks and lightweight `/health` endpoints.
- Put LBs in multiple Availability Zones and distribute targets evenly.
- Keep backends stateless; use stickiness only if necessary.
- Terminate TLS at the LB unless strict end-to-end encryption is required.
- Automate with Infrastructure as Code (CloudFormation/Terraform).
- Monitor health and error metrics with CloudWatch and set alerts early.
