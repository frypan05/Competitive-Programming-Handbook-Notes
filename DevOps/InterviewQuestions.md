# DevOps Interview Prep

## Scenario-Based Questions

1. **Conditional Jenkins Pipeline**
   - Design a Jenkins pipeline with ten stages.
   - Execute only a subset of stages based on user input (add conditions for each stage).
   - **Answer:** Use a scripted or declarative pipeline with a shared parameter map (e.g., `params.runBuild`, `params.runTests`) and wrap each of the ten stages inside `when { expression { params.runBuild } }` blocks so only the selected stages execute; encapsulate stage lists in functions for reusability and ensure default values prevent empty runs.

2. **Ansible Concepts**
   - Difference between an Ansible **playbook** and an **Ansible role**.
   - Scenarios where roles are preferred vs. when to use standalone playbooks.
   - **Answer:** Playbooks are ordered task files you run directly, whereas roles provide reusable structure (`tasks/`, `templates/`, `vars/`) for modular automation; choose roles when multiple teams or environments need the same logic and standalone playbooks when doing quick, one-off orchestration.

3. **Docker Home Path**
   - Identify the Docker data directory (e.g., `/var/lib/docker`).
   - **Answer:** On Linux hosts the default root is `/var/lib/docker`, but verify with `docker info | grep "Docker Root Dir"` or override via `daemon.json` if disks are moved.

4. **Troubleshooting Docker Disk Space in Jenkins**
   - Jenkins pipeline triggers Docker builds on a remote agent.
   - Pipeline began failing due to a “space issue,” yet the server has free disk space.
   - Outline steps to troubleshoot hidden or per-user disk usage (e.g., Docker layers, workspace cleanup).
   - **Answer:** Check the agent’s filesystem with `df -h` and per-directory usage via `du -sh /var/lib/docker/*` and Jenkins workspaces; prune unused images/containers (`docker system prune -a`), wipe abandoned workspaces, review per-user quotas, and confirm the Jenkins user’s home partition isn’t full even if the root disk looks healthy.

5. **Sample Dockerfile Requirement**
   - Create a Dockerfile for a Linux base image running a web server (Nginx/Tomcat) to serve a static website.
   - **Answer:** Use `FROM nginx:alpine`, copy static assets to `/usr/share/nginx/html`, expose port 80, and rely on the base image’s default `CMD ["nginx","-g","daemon off;"]`; for Tomcat swap to `FROM tomcat:9-jdk17` and drop artifacts into `/usr/local/tomcat/webapps/ROOT`.

6. **Dockerfile Instructions**
   - Explain the difference between `ENTRYPOINT` and `CMD`.
   - **Answer:** `ENTRYPOINT` defines the immutable executable that always runs (ideal for container’s main process), while `CMD` supplies default arguments that can be overridden at runtime with `docker run`; combining both lets you keep behavior predictable yet configurable.

7. **Kubernetes Architecture**
   - Describe control plane vs. data plane components and their responsibilities.
   - **Answer:** The control plane (API server, etcd, controller manager, scheduler) stores cluster state and makes scheduling decisions, while the data plane (kubelet, kube-proxy, container runtime on worker nodes) actually runs Pods and handles networking/routing for application traffic.

---

## Interview Questions

### General & Experience
- Please introduce yourself.
  - **Answer:** Give a concise summary covering current role, years of experience, key DevOps skills (CI/CD, cloud, automation), and highlight a recent impactful project.
- Explain your internship experience.
  - **Answer:** Outline the environment you supported, the stack you touched (AWS, Jenkins, Kubernetes, etc.), and quantify deliverables such as pipelines built or environments automated.
- What roles and responsibilities did you handle as a Cloud / DevOps Intern?
  - **Answer:** Mention owning CI/CD jobs, IaC modules, environment monitoring, incident response support, and collaboration with developers on release readiness.
- Certificate ≠ Experience — Exams teach you what to click, but Hands-on teaches what breaks. Certifications look shiny, until you hit your first production outage. You’ll never learn why a pod crashes, why disks fill up overnight, or why alerts don’t fire from an exam guide; these lessons come from the long nights spent on debugging. Practice proves competence. If your resume is full of logos but your terminal’s history is empty, it’s time to switch saddles. Because in DevOps, uptime isn’t saved by certificates it’s saved by the engineer who don’t hesitate to get their hands dirty.
  - **Answer:** Emphasize that certificates validate theory but interviewers want war stories—describe a real outage you solved, the diagnostics you ran, and how that hands-on grit matters more than badges.

### AWS Fundamentals
- How do you securely spin up an EC2 instance?
  - **Answer:** Create it inside a private subnet, attach a security group with least-privilege inbound rules, associate an IAM role, use an encrypted AMI/EBS volume, and access it via Systems Manager Session Manager or a bastion with key pairs.
- What security measures do you take while launching EC2?
  - **Answer:** Enforce MFA for console access, restrict key pair distribution, enable detailed monitoring, tag resources for audits, use security groups/NACLs, patch AMIs, and enable GuardDuty/Inspector for continuous checks.
- What is IAM and the principle of least privilege?
  - **Answer:** IAM manages identities and permissions in AWS; least privilege means granting only the minimum actions and resources required, typically via scoped policies and roles.
- What is a VPC?
  - **Answer:** A VPC is a logically isolated virtual network in AWS where you define IP ranges, subnets, routing, and security controls for your resources.
- Which components exist inside a VPC?
  - **Answer:** Subnets, route tables, internet/NAT gateways, NACLs, security groups, endpoints, DHCP options sets, and peering/VPN attachments.
- Difference between VPC CIDR and Subnet CIDR.
  - **Answer:** The VPC CIDR is the overall IP range (e.g., 10.0.0.0/16) while subnet CIDRs are smaller slices (e.g., 10.0.1.0/24) carved from it for specific availability zones or tiers.
- What are Security Groups?
  - **Answer:** Stateful virtual firewalls attached to ENIs that control inbound and outbound traffic via allow rules.
- Difference between inbound and outbound rules.
  - **Answer:** Inbound rules govern traffic entering an instance, outbound rules govern traffic leaving; security groups evaluate both but allow return traffic automatically if initiated.
- If inbound port 22 is allowed, what happens to outbound traffic?
  - **Answer:** Outbound is still controlled by outbound rules; if you allow all egress it works, otherwise you must explicitly permit required destination ports/IPs—even though return SSH traffic is automatically allowed.
- What is an Auto Scaling Group?
  - **Answer:** An ASG maintains desired instance counts using launch templates/configurations, scaling policies, health checks, and lifecycle hooks to match demand.
- What is CloudWatch and how did you use it?
  - **Answer:** CloudWatch collects metrics, logs, and alarms; use it to graph CPU/memory (via agent), create anomaly alarms, trigger Lambda remediations, and centralize application logs.
- What is Route 53?
  - **Answer:** AWS’s managed DNS service providing domain registration, health checks, routing policies (simple, weighted, latency), and private zones for VPCs.

### Load Balancing & Networking
- What is an Application Load Balancer (ALB)?
  - **Answer:** A layer-7 load balancer that routes HTTP/HTTPS based on host/path, supports WAF, SSL termination, and integrates with target groups for microservices.
- What are health checks in an ALB?
  - **Answer:** Periodic HTTP/HTTPS/TCP probes to each target’s endpoint; only healthy targets receive traffic, so configure endpoints that reflect real app readiness.
- Difference between ALB and NLB.
  - **Answer:** ALB operates at Layer 7 with advanced routing, while NLB is Layer 4 for ultra-low latency, static IPs, TCP/UDP passthrough, and millions of connections.

### Kubernetes
- What are the key components of Kubernetes?
  - **Answer:** API server, etcd, scheduler, controller manager on the control plane plus kubelet, kube-proxy, container runtime, and node components on workers.
- What is a Pod?
  - **Answer:** The smallest deployable unit encapsulating one or more containers sharing network namespace and storage volumes.
- Difference between a Pod and a Container.
  - **Answer:** A container is a single runtime process, whereas a Pod can host multiple tightly coupled containers with shared resources and is what Kubernetes schedules.
- What is a Service in Kubernetes?
  - **Answer:** An abstraction exposing a stable virtual IP and DNS name that load balances traffic to a set of Pods selected via labels.
- Types of Kubernetes Services.
  - **Answer:** ClusterIP (internal), NodePort, LoadBalancer, ExternalName, and headless services for direct DNS-based discovery.
- What is Ingress?
  - **Answer:** An API object defining layer-7 routing rules (hosts/paths) to expose HTTP/S services externally via a single entry point.
- What is an Ingress Controller?
  - **Answer:** The implementation (e.g., NGINX, Traefik, ALB) that watches Ingress resources and programs the underlying proxy/load balancer accordingly.
- Have you written Kubernetes YAML files?
  - **Answer:** Yes—authored Deployments, Services, ConfigMaps, Secrets, and Helm charts, validating them with `kubectl apply --dry-run=client`.
- How do you deploy applications to Kubernetes?
  - **Answer:** Use declarative YAML applied through `kubectl`, Helm charts in CI/CD, or GitOps tools like Argo CD to sync repositories to clusters.
- What happens when a Pod goes into `CrashLoopBackOff`?
  - **Answer:** Kubernetes restarts the failing container with exponential backoff due to repeated crashes; the root cause lies in the container process exiting unexpectedly.
- How do you debug a failing Pod?
  - **Answer:** Inspect `kubectl describe pod`, check logs via `kubectl logs`, exec for runtime checks, review events, verify config/secrets, and check node resources.

### Docker
- How do you list running containers?
  - **Answer:** Execute `docker ps` (or `docker ps -a` for all) and optionally filter with `--filter status=running`.
- How do you build and run Docker images?
  - **Answer:** Build with `docker build -t repo/app:tag .` and run using `docker run -d --name app -p 8080:80 repo/app:tag`.
- How do you check Docker container logs?
  - **Answer:** Use `docker logs <container>` with `-f` for tailing and `--since` for time filtering.
- How do you exec into a running container?
  - **Answer:** Run `docker exec -it <container> /bin/bash` (or `/bin/sh`) to open an interactive shell.
- What are Docker volumes and networks?
  - **Answer:** Volumes persist data outside container layers, while networks define isolated communication domains (bridge, host, overlay) for containers.
- Difference between `COPY` vs. `ADD`.
  - **Answer:** `COPY` performs straightforward file transfers and is preferred; `ADD` can fetch remote URLs and auto-extract archives, which may introduce side effects.
- Difference between `CMD` vs. `ENTRYPOINT`.
  - **Answer:** `ENTRYPOINT` defines the fixed executable, `CMD` provides default arguments; overriding `CMD` is easier, while `ENTRYPOINT` ensures the base command always runs.
- How do you debug a failing container?
  - **Answer:** Review logs, inspect exit codes (`docker inspect`), run the container interactively, check resource limits, validate environment variables, and compare to working image versions.

### CI/CD & Tooling
- What is CI/CD?
  - **Answer:** Continuous Integration automates build/test on every change, Continuous Delivery/Deployment pushes artifacts to environments; together they shorten feedback loops and reduce release risk.
- Explain your Jenkins pipeline structure.
  - **Answer:** Typically stages follow checkout → static analysis → build → unit/integration tests → container build → security scan → artifact publish → deploy → post-deploy verification with shared libraries for reuse.
- What stages did you use in Jenkins?
  - **Answer:** Source checkout, dependency install, build, test, code quality (SonarQube), security scan (Trivy), Docker build/push, infrastructure provisioning (Terraform), deployment, and notifications.
- How did you implement DevSecOps?
  - **Answer:** Shift-left scans (SAST/DAST), container vulnerability scanning, IaC policy checks, secrets detection in pipelines, enforced approvals, and continuous monitoring with alert hooks.
- What is SonarQube?
  - **Answer:** A code quality platform that performs static analysis for bugs, vulnerabilities, and code smells with quality gates tied into CI.
- What is Trivy?
  - **Answer:** An open-source scanner for container images, file systems, and IaC templates detecting CVEs and misconfigurations.
- How do you push images to ECR?
  - **Answer:** Authenticate via `aws ecr get-login-password`, tag the image with the registry URI, and run `docker push <account>.dkr.ecr.<region>.amazonaws.com/repo:tag`.
- How does Jenkins deploy to Kubernetes?
  - **Answer:** Pipelines use `kubectl`/`helm` with kubeconfig or service account tokens stored in Jenkins credentials, applying manifests or Helm charts after builds.
- Jenkins build is failing — how do you debug?
  - **Answer:** Check console logs, validate environment variables/credentials, reproduce locally, inspect agent resource metrics, and review recent SCM changes affecting stages.
- Difference between GitLab CI and GitHub Actions.
  - **Answer:** GitLab CI uses `.gitlab-ci.yml` with built-in runners and tightly integrated DevOps lifecycle; GitHub Actions leverages YAML workflows triggered by repo events with marketplace actions and hosted/self-hosted runners.
- What is Argo CD?
  - **Answer:** A GitOps controller that continuously syncs Kubernetes clusters with declarative manifests in Git, offering drift detection and automated rollbacks.
- What is GitOps?
  - **Answer:** A paradigm where Git serves as the single source of truth for infrastructure/application manifests and automated controllers reconcile cluster state with repository state.

### Infrastructure as Code
- What is Terraform?
  - **Answer:** An open-source IaC tool using declarative HCL to provision multi-cloud resources via providers, managing lifecycle through state.
- What is Terraform drift?
  - **Answer:** Any divergence between infrastructure state and Terraform configuration/state file caused by manual changes or external processes.
- How do you secure Terraform state files?
  - **Answer:** Store them in encrypted backends (S3 with SSE-KMS), enable least-privilege access, use remote backends, and avoid committing state to version control.
- What is a remote backend?
  - **Answer:** A centralized storage location (S3, Terraform Cloud, etc.) where Terraform keeps state to enable collaboration and locking.
- What is state locking?
  - **Answer:** A mechanism (e.g., DynamoDB table with S3 backend) preventing concurrent `terraform apply` operations that could corrupt state.
- Have you used S3 + DynamoDB for Terraform?
  - **Answer:** Yes—S3 stores encrypted state while DynamoDB handles locking via a dedicated table; configure both in the backend block.
- How do you manage secrets securely?
  - **Answer:** Use AWS Secrets Manager/Parameter Store, HashiCorp Vault, or sealed secrets; inject at runtime via environment variables or secret mounts rather than committing plaintext.

### Observability & Monitoring
- Have you worked with Prometheus and Grafana?
  - **Answer:** Yes—deployed Prometheus Operator for metrics scraping and connected Grafana for dashboards/alerts using Alertmanager integrations.
- How did you deploy Prometheus in Kubernetes?
  - **Answer:** Used Helm (kube-prometheus-stack) with custom scrape configs, RBAC service accounts, persistent volumes, and ServiceMonitors for app metrics.
- What is Node Exporter?
  - **Answer:** A Prometheus exporter running on nodes to expose OS-level metrics like CPU, memory, disk, and filesystem stats.
- What monitoring tools have you used?
  - **Answer:** Prometheus/Grafana, CloudWatch, ELK/EFK stacks, Datadog, and New Relic depending on environment requirements.
- What is Performance Insights?
  - **Answer:** An AWS RDS feature that visualizes database load, waits, and SQL bottlenecks to aid tuning.
- Have you worked with log aggregation tools?
  - **Answer:** Yes—configured Fluent Bit/Logstash shipping into Elasticsearch/OpenSearch and used CloudWatch Logs Insights for centralized querying.

### Linux & OS
- How do you check disk usage in Linux?
  - **Answer:** Run `df -h` for filesystem usage, `du -sh *` to pinpoint large directories, and `ncdu` for interactive exploration.
- How do you find which process is using a port?
  - **Answer:** Use `sudo lsof -i :PORT`, `ss -ltnp`, or `netstat -tulpn` to map ports to PIDs.
- What is a zombie process?
  - **Answer:** A defunct process whose execution is complete but whose parent hasn’t read its exit status, leaving an entry in the process table.
- Difference between hard link and soft link.
  - **Answer:** Hard links reference the same inode and survive original file deletion (within same filesystem), while soft links are pointers to paths and break if the target is removed.
- Do you have scripting knowledge?
  - **Answer:** Yes—write Bash/Python scripts for automation, log parsing, and pipeline glue code, following modular and idempotent practices.

### Git & Troubleshooting
- What is Git?
  - **Answer:** A distributed version control system that tracks changes, enables branching/merging, and supports collaborative development workflows.
- What are Git branches?
  - **Answer:** Lightweight pointers to commits representing independent lines of development, enabling feature isolation and experimentation.
- How does Git merge work?
  - **Answer:** Combines changes from one branch into another, creating merge commits or fast-forwards after reconciling differences; conflicts require manual resolution.
- Application is down — how will you troubleshoot?
  - **Answer:** Start with user impact, check monitoring/alerts, inspect recent deployments, verify infrastructure health, review logs/metrics, and roll back or hotfix as needed.
- What checks will you do for Docker vs. EC2 vs. Kubernetes?
  - **Answer:** Docker: container status/logs/resources; EC2: instance health, OS services, network/security groups; Kubernetes: pod events, deployments, services, nodes.
- How do you debug network and port issues?
  - **Answer:** Validate security groups/firewalls, run `telnet`/`nc`/`curl`, inspect DNS, use `traceroute`, check kube-proxy/iptables or VPC flow logs depending on environment.
- How do you troubleshoot CI/CD failures?
  - **Answer:** Review pipeline logs, confirm environment variables/secrets, reproduce locally, check agent capacity, verify dependency availability, and bisect recent code changes.

---

### AWS Deep Dive
- What is NACL in AWS? What is the difference between security group and NACL?
  - **Answer:** NACL (Network Access Control List) is a stateless firewall operating at the subnet level with numbered allow/deny rules for inbound and outbound traffic; Security Groups are stateful, instance-level firewalls with only allow rules that automatically permit return traffic. NACLs evaluate rules in order and apply to all instances in a subnet, while SGs are attached per ENI and require explicit outbound rules only for new connections.
- What is an EC2 instance store? How does EC2 instance store differ from EBS volume?
  - **Answer:** Instance store provides ephemeral block storage physically attached to the host, offering high IOPS but data is lost on stop/termination; EBS volumes are network-attached persistent storage that survive instance lifecycle events and support snapshots, encryption, and volume types (gp3, io2, etc.).
- How many types of storage classes in S3?
  - **Answer:** S3 Standard, S3 Intelligent-Tiering, S3 Standard-IA, S3 One Zone-IA, S3 Glacier Instant Retrieval, S3 Glacier Flexible Retrieval, S3 Glacier Deep Archive, and S3 Outposts; choose based on access frequency and retrieval time requirements.
- What is the use of m5.2xlarge instance type?
  - **Answer:** m5.2xlarge is a general-purpose instance with 8 vCPUs and 32 GiB RAM balanced for compute/memory workloads like web servers, app servers, small databases, and development environments.
- How many types of EC2 instance types in AWS?
  - **Answer:** AWS offers seven main families: General Purpose (T, M), Compute Optimized (C), Memory Optimized (R, X, High Memory), Accelerated Computing (P, G, Inf, Trn), Storage Optimized (I, D, H), and HPC Optimized (Hpc); each family has multiple generations and sizes.
- What is CDN in AWS?
  - **Answer:** Amazon CloudFront is AWS's CDN that caches content at global edge locations to reduce latency, offload origin traffic, and deliver static/dynamic content, streaming media, and APIs with SSL/TLS support and WAF integration.
- What type of database is DynamoDB? Is it relational or not?
  - **Answer:** DynamoDB is a fully managed NoSQL key-value and document database offering single-digit millisecond performance, automatic scaling, and flexible schemas—not relational, so no joins or complex queries like SQL databases.
- What type of DB is PostgreSQL?
  - **Answer:** PostgreSQL is an open-source relational database (RDBMS) supporting ACID transactions, complex queries, joins, foreign keys, and advanced features like JSONB, full-text search, and extensions.

### Jenkins Deep Dive
- How do you store secrets in Jenkins?
  - **Answer:** Use Jenkins Credentials plugin to store secrets (username/password, SSH keys, tokens) encrypted at rest; reference them in pipelines via `credentials()` or `withCredentials` blocks, and integrate external vaults like HashiCorp Vault or AWS Secrets Manager for enhanced security.
- How to implement/setup SonarQube in your Jenkins pipeline?
  - **Answer:** Install SonarQube Scanner plugin, configure SonarQube server in Jenkins global settings with authentication token, add a pipeline stage calling `withSonarQubeEnv('SonarQube') { sh 'mvn sonar:sonar' }`, and optionally use Quality Gates with `waitForQualityGate()` to fail builds on threshold violations.

### Docker & Kubernetes Deep Dive
- What is the difference between CMD & ENTRYPOINT in Dockerfile?
  - **Answer:** `ENTRYPOINT` defines the fixed executable that always runs (ideal for container's main process), while `CMD` supplies default arguments that can be overridden at runtime with `docker run`; combining both lets you keep behavior predictable yet configurable.
- What is the purpose of a base image?
  - **Answer:** A base image provides the foundational OS layer and runtime dependencies (e.g., `alpine`, `ubuntu`, `python:3.11`) that subsequent Dockerfile instructions build upon, enabling consistency, smaller layer counts, and reusability across applications.
- What is the K8s architecture? Components of master node & worker node.
  - **Answer:** Master (control plane) runs API server (entry point), etcd (state store), scheduler (Pod placement), and controller manager (reconciliation loops); worker nodes run kubelet (Pod lifecycle agent), kube-proxy (network rules), and container runtime (Docker/containerd) to execute workloads.
- If pod is CrashLoopBackOff, how to debug this?
  - **Answer:** Kubernetes restarts the failing container with exponential backoff due to repeated crashes; debug with `kubectl describe pod` for events, `kubectl logs <pod> --previous` for crash logs, verify image/command, check resource limits, validate ConfigMaps/Secrets, and exec into init containers if applicable.

### Terraform Deep Dive
- How to structure modules/code in Terraform?
  - **Answer:** Organize with a root module calling child modules stored in `modules/` directories; each module contains `main.tf` (resources), `variables.tf` (inputs), `outputs.tf` (exports), and optionally `versions.tf` (provider constraints); use remote modules for shared infrastructure patterns and workspaces or separate state files per environment.
- What is the .tfstate file in Terraform? Where is the .tfstate file stored?
  - **Answer:** The state file maps Terraform configuration to real-world resource IDs and metadata, enabling plan/apply operations; store locally by default (`terraform.tfstate`) or in remote backends (S3 + DynamoDB, Terraform Cloud, Azure Blob) with encryption and locking for team collaboration.
- What does the terraform init command do?
  - **Answer:** `terraform init` initializes the working directory by downloading provider plugins, setting up the backend for state storage, and installing child modules; run it after cloning a repo or changing provider/backend configuration.

### Linux Deep Dive
- Suppose I have a random logs file, I want to wipe out or delete the logs, what will be the one-liner command to delete the logs?
  - **Answer:** To truncate (clear contents) without deleting the file use `> /path/to/logfile.log` or `truncate -s 0 /path/to/logfile.log`; to delete the file entirely run `rm /path/to/logfile.log`; for mass deletion use `find /var/log -name "*.log" -type f -delete`.
- What is the command to check the listening port?
  - **Answer:** Use `sudo ss -ltnp`, `sudo netstat -tulpn`, or `sudo lsof -i -P -n | grep LISTEN` to display all listening TCP/UDP ports with associated process IDs and names.

---


## DevOps Interview Notes

1. **Terraform State File** – Stores mapping between Terraform config and real infrastructure.
2. **State File Storage** – Local or remote (S3 + DynamoDB, Terraform Cloud).
3. **Null Resource** – Run scripts without creating real infrastructure.
4. **CI/CD** – Automates build, test, and deployment pipeline.
5. **EC2 via Terraform** – Uses the `aws_instance` resource.
6. **Commenting a Resource** – Terraform plans to destroy it.
7. **Largest/Smallest Array Problem** – Iterate and compare values.
8. **ENTRYPOINT vs. CMD** – `ENTRYPOINT` is fixed; `CMD` is overrideable.
9. **ADD vs. COPY** – `COPY` is preferred and safer.
10. **Kubernetes Architecture** – Master + worker nodes.
11. **Ansible** – Agentless configuration management tool.
12. **Secrets vs. ConfigMap** – Sensitive vs. non-sensitive data.
13. **Docker Lifecycle** – Build → Run → Stop → Remove.
14. **ReplicaSet** – Maintains desired Pod count.
15. **Single-Node Kubernetes** – Minikube, Kind.
16. **Remove File from Git** – `git rm --cached`.
17. **Check Merged Branch** – `git branch --merged`.
18. **ALB vs. NLB** – Layer 7 vs. Layer 4.
19. **Route 53** – AWS DNS service.
20. **GCP** – Compute Engine, GKE, Storage.
21. **Jenkins Pipelines** – Single vs. multiple pipeline strategies.
22. **Pipeline Issues** – Complexity vs. coordination.
23. **Jenkins Version** – Use latest LTS.
24. **Jenkins + Terraform** – `terraform init`, `plan`, `apply`.
25. **Create 10 EC2 Instances** – Use the `count` meta-argument.
26. **Terminate EC2 Instances** – Reduce `count`.
27. **On-Prem to VPC Connectivity** – VPN / Direct Connect.
28. **Terraform Taint** – Forces resource recreation.
29. **Terraform Refresh** – Syncs state with real infrastructure.
30. **Manual Change** – Terraform reverts drift.
31. **Terraform Module** – Reusable configuration blocks.
32. **CloudTrail** – AWS API auditing service.
33. **Load Balancer & Auto Scaling** – Traffic distribution + scaling.
34. **Databricks** – Spark-based analytics platform.
