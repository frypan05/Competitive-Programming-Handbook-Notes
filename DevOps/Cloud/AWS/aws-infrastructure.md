# AWS Infrastructure Overview
This document gives a beginner-friendly explanation of three core AWS services you’ll commonly use together:
- Auto Scaling Groups (ASGs)
- Amazon CloudFront (CDN)
- Elastic Load Balancers (ELB)

Each section includes plain-language explanations, scenario-based examples, and short command snippets to illustrate common tasks.

---

## 1. Auto Scaling Groups (ASG)

What is an ASG?
- An `Auto Scaling Group` is a logical group of EC2 instances that AWS maintains for you. You define minimum, desired, and maximum numbers of instances and rules that tell AWS when to add or remove instances.
- ASGs keep your application available, replace unhealthy instances, and help you match capacity to demand automatically.

Key components
- `Launch Template` / `Launch Configuration`: Defines AMI, instance type, user data, IAM role, etc. (Use `Launch Template` for new setups.)
- `min/desired/max` capacity: Lower and upper bounds for instances.
- Scaling policies:
  - Target tracking (recommended): e.g., keep average CPU at 40%.
  - Step scaling: scale by steps depending on alarm severity.
  - Scheduled scaling: scale at known times (e.g., business hours).
- Health checks: ASG can use EC2 status checks and ELB health checks. Unhealthy instances are replaced.

Scenario: E-commerce sale event
- Situation: You run an online shop. Normally you need 4 instances, but during a sale you expect traffic spikes.
- ASG setup:
  - `min = 2`, `desired = 4`, `max = 20`
  - Add a target-tracking policy to keep average CPU ≈ 50%
  - Register instances with an Application Load Balancer (ALB) for smooth request distribution
- Result: When traffic spikes, ASG increases instances up to `max`. When traffic falls, instances are scaled down, saving cost.

Quick conceptual CLI example (create launch template + ASG):
```/dev/null/aws-asg-example.sh#L1-20
# Create a launch template (example)
aws ec2 create-launch-template --launch-template-name my-template \
  --version-description "v1" \
  --launch-template-data '{"ImageId":"ami-0123456789abcdef0","InstanceType":"t3.medium","UserData":"..."}'

# Create an autoscaling group
aws autoscaling create-auto-scaling-group --auto-scaling-group-name my-asg \
  --launch-template LaunchTemplateName=my-template,Version=1 \
  --min-size 2 --desired-capacity 4 --max-size 20 \
  --vpc-zone-identifier "subnet-aaa,subnet-bbb" --health-check-type ELB
```

Best practices
- Use `Launch Templates`.
- Spread instances across multiple Availability Zones.
- Use ELB health checks so ASG knows to replace instances failing application-level checks.
- Use lifecycle hooks to drain traffic before termination (graceful shutdown).

---

## 2. Amazon CloudFront (CDN)

What is CloudFront?
- `CloudFront` is AWS’s Content Delivery Network (CDN). It caches your static and dynamic content at edge locations globally to deliver content faster to users.
- It sits in front of your origin (S3, ALB, API Gateway, or custom HTTP server) and serves cached content when possible.

How CloudFront helps
- Reduced latency for global users (content served from the nearest edge location).
- Lower origin load and costs due to caching.
- HTTPS, geo-restrictions, signed URLs/cookies, and integration with WAF for security.

Scenario: Global static site + APIs
- Situation: You host static website assets (images, JS) in an S3 bucket in `us-east-1`, but you have users worldwide.
- Solution: Place CloudFront distribution in front of S3. Configure cache TTLs for images and CSS. Use an ALB backend for APIs and set appropriate cache behavior (e.g., do not cache user-specific API responses).
- Result: Users in Europe or Asia get assets from nearby edge locations, improving perceived performance.

CloudFront quick example: create distribution pointing to S3 (conceptual)
```/dev/null/cloudfront-create-example.sh#L1-12
aws cloudfront create-distribution --origin-domain-name my-bucket.s3.amazonaws.com \
  --default-root-object index.html \
  --default-cache-behavior '{"TargetOriginId":"S3Origin","ViewerProtocolPolicy":"redirect-to-https"}'
```

Best practices
- Cache static assets aggressively with long TTLs and use versioned filenames for cache-busting.
- Use signed URLs/Cookies for private content.
- Keep sensitive, user-specific responses un-cached or use short TTLs.

---

## 3. Elastic Load Balancers (ELB) — high-level

What is an Elastic Load Balancer?
- ELB is a family of managed load balancers that distribute incoming traffic across targets (EC2 instances, IPs, Lambdas).
- Types:
  - Application Load Balancer (ALB) — Layer 7 (HTTP/HTTPS). Good for web apps, host/path routing, WebSockets.
  - Network Load Balancer (NLB) — Layer 4 (TCP/UDP). Good for ultra-low latency, static IPs, TLS passthrough.
  - Classic Load Balancer (CLB) — legacy; prefer ALB/NLB.

When to use which
- Use `ALB` when you need HTTP routing, host/path-based routing, or advanced features (sticky sessions, WAF integration).
- Use `NLB` for TCP services, high throughput, or when you need a static IP per AZ.
- Avoid `CLB` unless maintaining legacy systems.

Scenario: Blue/Green deployment using ALB and ASG
- You have `blue` ASG (current) and `green` ASG (new version).
- Steps:
  1. Create new `green` ASG with updated launch template.
  2. Attach `green` ASG to the same ALB target group.
  3. Gradually shift traffic from `blue` target group to `green` target group using weighted routing or by re-registering instances.
  4. Once healthy, scale down `blue` and remove it.
- Benefit: Almost zero-downtime deployment, ability to roll back quickly.

Note: You requested an in-depth file about load balancers. I'll produce a separate `load-balancers.md` under `devops/cloud/` with deep coverage (ALB rules, target groups, sticky sessions, TLS termination, path-based routing, NLB details, troubleshooting, metrics, and real-world examples). If you want that now, tell me and I’ll provide it next.

---

## Final tips & costs
- ASG, CloudFront, and ELB complement each other: ASG for compute elasticity, ELB for traffic distribution and health checks, CloudFront for global delivery and caching.
- There’s no extra charge for ASG itself; you pay for EC2, ELB, CloudFront data transfer and requests.
- Monitor CloudWatch metrics (CPU, latency, request count, 4xx/5xx rates) and set sensible alarms.
- Use Infrastructure as Code (CloudFormation, Terraform) so environments are reproducible.
