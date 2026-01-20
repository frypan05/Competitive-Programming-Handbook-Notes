# DevOps Interview Prep

## Scenario-Based Questions

1. **Conditional Jenkins Pipeline**
   - Design a Jenkins pipeline with ten stages.
   - Execute only a subset of stages based on user input (add conditions for each stage).

2. **Ansible Concepts**
   - Difference between an Ansible **playbook** and an **Ansible role**.
   - Scenarios where roles are preferred vs. when to use standalone playbooks.

3. **Docker Home Path**
   - Identify the Docker data directory (e.g., `/var/lib/docker`).

4. **Troubleshooting Docker Disk Space in Jenkins**
   - Jenkins pipeline triggers Docker builds on a remote agent.
   - Pipeline began failing due to a “space issue,” yet the server has free disk space.
   - Outline steps to troubleshoot hidden or per-user disk usage (e.g., Docker layers, workspace cleanup).

5. **Sample Dockerfile Requirement**
   - Create a Dockerfile for a Linux base image running a web server (Nginx/Tomcat) to serve a static website.

6. **Dockerfile Instructions**
   - Explain the difference between `ENTRYPOINT` and `CMD`.

7. **Kubernetes Architecture**
   - Describe control plane vs. data plane components and their responsibilities.

---

## Interview Questions

### General & Experience
- Please introduce yourself.
- Explain your internship experience.
- What roles and responsibilities did you handle as a Cloud / DevOps Intern?

### AWS Fundamentals
- How do you securely spin up an EC2 instance?
- What security measures do you take while launching EC2?
- What is IAM and the principle of least privilege?
- What is a VPC?
- Which components exist inside a VPC?
- Difference between VPC CIDR and Subnet CIDR.
- What are Security Groups?
- Difference between inbound and outbound rules.
- If inbound port 22 is allowed, what happens to outbound traffic?
- What is an Auto Scaling Group?
- What is CloudWatch and how did you use it?
- What is Route 53?

### Load Balancing & Networking
- What is an Application Load Balancer (ALB)?
- What are health checks in an ALB?
- Difference between ALB and NLB.

### Kubernetes
- What are the key components of Kubernetes?
- What is a Pod?
- Difference between a Pod and a Container.
- What is a Service in Kubernetes?
- Types of Kubernetes Services.
- What is Ingress?
- What is an Ingress Controller?
- Have you written Kubernetes YAML files?
- How do you deploy applications to Kubernetes?
- What happens when a Pod goes into `CrashLoopBackOff`?
- How do you debug a failing Pod?

### Docker
- How do you list running containers?
- How do you build and run Docker images?
- How do you check Docker container logs?
- How do you exec into a running container?
- What are Docker volumes and networks?
- Difference between `COPY` vs. `ADD`.
- Difference between `CMD` vs. `ENTRYPOINT`.
- How do you debug a failing container?

### CI/CD & Tooling
- What is CI/CD?
- Explain your Jenkins pipeline structure.
- What stages did you use in Jenkins?
- How did you implement DevSecOps?
- What is SonarQube?
- What is Trivy?
- How do you push images to ECR?
- How does Jenkins deploy to Kubernetes?
- Jenkins build is failing — how do you debug?
- Difference between GitLab CI and GitHub Actions.
- What is Argo CD?
- What is GitOps?

### Infrastructure as Code
- What is Terraform?
- What is Terraform drift?
- How do you secure Terraform state files?
- What is a remote backend?
- What is state locking?
- Have you used S3 + DynamoDB for Terraform?
- How do you manage secrets securely?

### Observability & Monitoring
- Have you worked with Prometheus and Grafana?
- How did you deploy Prometheus in Kubernetes?
- What is Node Exporter?
- What monitoring tools have you used?
- What is Performance Insights?
- Have you worked with log aggregation tools?

### Linux & OS
- How do you check disk usage in Linux?
- How do you find which process is using a port?
- What is a zombie process?
- Difference between hard link and soft link.
- Do you have scripting knowledge?

### Git & Troubleshooting
- What is Git?
- What are Git branches?
- How does Git merge work?
- Application is down — how will you troubleshoot?
- What checks will you do for Docker vs. EC2 vs. Kubernetes?
- How do you debug network and port issues?
- How do you troubleshoot CI/CD failures?

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
