# 09 — Terraform, Ansible & Infrastructure as Code (IaC)

---

## SECTION 1 — IaC Concepts

**Q: What is Infrastructure as Code? Why is it important?**

A: IaC means managing and provisioning infrastructure (servers, networks, databases, DNS records)
through machine-readable configuration files rather than manual processes or interactive tools.

Benefits:
```
Reproducibility  — the same config deploys identical environments every time.
                   "Works in staging but not prod" becomes impossible with true IaC.

Version Control  — infra changes tracked in Git. Every change has an author, timestamp,
                   commit message, and is reversible with git revert.

Code Review      — infra changes go through PR review like application code.

Collaboration    — the whole team can understand, modify, and improve infrastructure.

Documentation    — the config IS the documentation. No separate "setup guide" to maintain.

Speed            — minutes to spin up a full environment vs hours of manual work.

Consistency      — eliminates config drift between environments.

Disaster Recovery — if you lose everything, run terraform apply and it comes back.
```

---

**Q: Terraform vs Ansible — what is the fundamental difference?**

```
Terraform:
  - Purpose: PROVISIONING infrastructure (create/destroy cloud resources).
  - Model: Declarative — you describe WHAT you want.
             Terraform figures out HOW to achieve it.
  - State: maintains a state file tracking what it has created.
  - Idempotent: running the same config twice = no change if already correct.
  - Providers: AWS, GCP, Azure, K8s, GitHub, Datadog, Cloudflare...
  - Best for: spinning up EC2 instances, VPCs, RDS, S3 buckets, IAM roles, DNS records.

Ansible:
  - Purpose: CONFIGURATION MANAGEMENT and application deployment.
  - Model: Mostly imperative/procedural — ordered list of tasks.
             Each task uses idempotent modules.
  - State: stateless — no state file. Connects to machines and checks/applies.
  - Agentless: connects via SSH (Linux) or WinRM (Windows). No agent needed on targets.
  - Push-based: controller machine pushes config to nodes.
  - Best for: installing packages, configuring services, deploying application files,
              running commands on existing servers.

Together:
  Terraform provisions the EC2 instance.
  Ansible installs and configures the application on it.
  They are complementary, not competing.
```

---

## SECTION 2 — Terraform

**Q: Explain the Terraform workflow.**

```
terraform init    — download provider plugins and modules.
                    Run this first and whenever you add new providers/modules.
                    Creates .terraform/ directory and .terraform.lock.hcl.

terraform plan    — show what Terraform WOULD do if you applied.
                    Compares desired config to current state.
                    Outputs: + (create), ~ (modify), - (destroy), -/+ (replace).
                    Always review the plan before applying. Shows exact changes.
                    Use: terraform plan -out=tfplan  to save for later apply.

terraform apply   — apply the changes shown in the plan.
                    Asks for confirmation by default (type 'yes').
                    terraform apply -auto-approve  skips confirmation (for CI/CD).
                    terraform apply tfplan          apply a saved plan file.

terraform destroy — destroy all resources managed by this configuration.
                    Use carefully. terraform destroy -target=aws_instance.web  for targeted.

terraform validate — check configuration syntax and internal consistency.
terraform fmt      — format .tf files according to canonical HCL style.
terraform output   — show values of output variables.
terraform show     — show human-readable state or plan.
```

---

**Q: What is Terraform state and why is it critical?**

A: Terraform stores the current state of managed infrastructure in a JSON state file
(`terraform.tfstate`). The state file maps your HCL resources to real-world resource IDs.

It is used to:
- Calculate diffs between desired configuration and actual infrastructure
- Track resource dependencies and ordering
- Store metadata and computed attributes (e.g., assigned IP addresses, auto-generated names)

**Remote state (always use in teams):**
```hcl
# Store state in S3 with DynamoDB locking
terraform {
  backend "s3" {
    bucket         = "my-terraform-state-prod"
    key            = "services/api/terraform.tfstate"
    region         = "us-east-1"
    encrypt        = true                          # encrypt at rest with KMS
    dynamodb_table = "terraform-state-locks"       # prevents concurrent apply
  }
}
```

**Why NOT commit local state to Git:**
- State contains sensitive values (passwords, private keys) in plaintext
- Concurrent applies by two team members will corrupt state
- Git is not a locking mechanism

**State manipulation commands:**
```bash
terraform state list                          # list all resources in state
terraform state show aws_instance.web         # inspect a specific resource
terraform state mv old_name new_name          # rename a resource in state
terraform state rm aws_instance.old           # remove resource from state (doesn't destroy it)
terraform import aws_s3_bucket.existing my-bucket  # import existing resource into state
terraform state pull                          # download remote state to stdout
terraform state push terraform.tfstate        # push local state to remote (use carefully)
```

---

### Terraform Example — Full AWS VPC + ECS + ALB

```hcl
# versions.tf
terraform {
  required_version = ">= 1.5.0"

  required_providers {
    aws = {
      source  = "hashicorp/aws"
      version = "~> 5.0"
    }
  }

  backend "s3" {
    bucket         = "my-terraform-state"
    key            = "prod/main.tfstate"
    region         = "us-east-1"
    encrypt        = true
    dynamodb_table = "terraform-locks"
  }
}

provider "aws" {
  region = var.aws_region

  default_tags {
    tags = {
      Environment = var.environment
      ManagedBy   = "terraform"
      Project     = var.project_name
    }
  }
}

# variables.tf
variable "environment" {
  description = "Deployment environment"
  type        = string
  validation {
    condition     = contains(["dev", "staging", "prod"], var.environment)
    error_message = "environment must be one of: dev, staging, prod"
  }
}

variable "aws_region" {
  type    = string
  default = "us-east-1"
}

variable "project_name" {
  type    = string
  default = "myapp"
}

variable "app_image" {
  description = "Docker image for the application"
  type        = string
}

variable "app_port" {
  type    = number
  default = 8080
}

variable "desired_count" {
  description = "Number of ECS tasks to run"
  type        = number
  default     = 2
}

# main.tf

# VPC using community module
module "vpc" {
  source  = "terraform-aws-modules/vpc/aws"
  version = "5.1.2"

  name = "${var.project_name}-${var.environment}"
  cidr = "10.0.0.0/16"

  azs             = ["${var.aws_region}a", "${var.aws_region}b", "${var.aws_region}c"]
  private_subnets = ["10.0.1.0/24", "10.0.2.0/24", "10.0.3.0/24"]
  public_subnets  = ["10.0.101.0/24", "10.0.102.0/24", "10.0.103.0/24"]

  enable_nat_gateway   = true
  single_nat_gateway   = var.environment != "prod"  # multi-AZ NAT in prod only
  enable_dns_hostnames = true
  enable_dns_support   = true
}

# Security Groups
resource "aws_security_group" "alb" {
  name        = "${var.project_name}-${var.environment}-alb"
  description = "Security group for the Application Load Balancer"
  vpc_id      = module.vpc.vpc_id

  ingress {
    description = "HTTP from internet"
    from_port   = 80
    to_port     = 80
    protocol    = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }

  ingress {
    description = "HTTPS from internet"
    from_port   = 443
    to_port     = 443
    protocol    = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }

  egress {
    from_port   = 0
    to_port     = 0
    protocol    = "-1"
    cidr_blocks = ["0.0.0.0/0"]
  }
}

resource "aws_security_group" "ecs_tasks" {
  name        = "${var.project_name}-${var.environment}-ecs"
  description = "Security group for ECS tasks — only allow traffic from ALB"
  vpc_id      = module.vpc.vpc_id

  ingress {
    description     = "From ALB only"
    from_port       = var.app_port
    to_port         = var.app_port
    protocol        = "tcp"
    security_groups = [aws_security_group.alb.id]
  }

  egress {
    from_port   = 0
    to_port     = 0
    protocol    = "-1"
    cidr_blocks = ["0.0.0.0/0"]
  }
}

# Application Load Balancer
resource "aws_lb" "app" {
  name               = "${var.project_name}-${var.environment}"
  internal           = false
  load_balancer_type = "application"
  security_groups    = [aws_security_group.alb.id]
  subnets            = module.vpc.public_subnets

  enable_deletion_protection = var.environment == "prod"

  access_logs {
    bucket  = aws_s3_bucket.alb_logs.bucket
    enabled = true
  }
}

resource "aws_lb_target_group" "app" {
  name        = "${var.project_name}-${var.environment}"
  port        = var.app_port
  protocol    = "HTTP"
  vpc_id      = module.vpc.vpc_id
  target_type = "ip"    # required for Fargate

  health_check {
    enabled             = true
    healthy_threshold   = 2
    unhealthy_threshold = 3
    timeout             = 5
    interval            = 30
    path                = "/health"
    matcher             = "200"
  }

  deregistration_delay = 30
}

resource "aws_lb_listener" "https" {
  load_balancer_arn = aws_lb.app.arn
  port              = 443
  protocol          = "HTTPS"
  ssl_policy        = "ELBSecurityPolicy-TLS13-1-2-2021-06"
  certificate_arn   = aws_acm_certificate.app.arn

  default_action {
    type             = "forward"
    target_group_arn = aws_lb_target_group.app.arn
  }
}

resource "aws_lb_listener" "http_redirect" {
  load_balancer_arn = aws_lb.app.arn
  port              = 80
  protocol          = "HTTP"

  default_action {
    type = "redirect"
    redirect {
      port        = "443"
      protocol    = "HTTPS"
      status_code = "HTTP_301"
    }
  }
}

# ECS Cluster
resource "aws_ecs_cluster" "main" {
  name = "${var.project_name}-${var.environment}"

  setting {
    name  = "containerInsights"
    value = "enabled"
  }
}

# IAM Role for ECS task execution
resource "aws_iam_role" "ecs_task_execution" {
  name = "${var.project_name}-${var.environment}-ecs-execution"

  assume_role_policy = jsonencode({
    Version = "2012-10-17"
    Statement = [{
      Action    = "sts:AssumeRole"
      Effect    = "Allow"
      Principal = { Service = "ecs-tasks.amazonaws.com" }
    }]
  })
}

resource "aws_iam_role_policy_attachment" "ecs_task_execution" {
  role       = aws_iam_role.ecs_task_execution.name
  policy_arn = "arn:aws:iam::aws:policy/service-role/AmazonECSTaskExecutionRolePolicy"
}

# ECS Task Definition
resource "aws_ecs_task_definition" "app" {
  family                   = "${var.project_name}-${var.environment}"
  network_mode             = "awsvpc"
  requires_compatibilities = ["FARGATE"]
  cpu                      = 256
  memory                   = 512
  execution_role_arn       = aws_iam_role.ecs_task_execution.arn

  container_definitions = jsonencode([{
    name  = var.project_name
    image = var.app_image

    portMappings = [{
      containerPort = var.app_port
      protocol      = "tcp"
    }]

    environment = [
      { name = "ENVIRONMENT", value = var.environment },
      { name = "PORT",        value = tostring(var.app_port) }
    ]

    secrets = [
      { name = "DB_PASSWORD", valueFrom = aws_ssm_parameter.db_password.arn }
    ]

    logConfiguration = {
      logDriver = "awslogs"
      options = {
        "awslogs-group"         = aws_cloudwatch_log_group.app.name
        "awslogs-region"        = var.aws_region
        "awslogs-stream-prefix" = "ecs"
      }
    }

    healthCheck = {
      command     = ["CMD-SHELL", "curl -f http://localhost:${var.app_port}/health || exit 1"]
      interval    = 30
      timeout     = 5
      retries     = 3
      startPeriod = 60
    }
  }])
}

# ECS Service
resource "aws_ecs_service" "app" {
  name            = var.project_name
  cluster         = aws_ecs_cluster.main.id
  task_definition = aws_ecs_task_definition.app.arn
  desired_count   = var.desired_count
  launch_type     = "FARGATE"

  network_configuration {
    subnets          = module.vpc.private_subnets
    security_groups  = [aws_security_group.ecs_tasks.id]
    assign_public_ip = false
  }

  load_balancer {
    target_group_arn = aws_lb_target_group.app.arn
    container_name   = var.project_name
    container_port   = var.app_port
  }

  deployment_minimum_healthy_percent = 100
  deployment_maximum_percent         = 200

  deployment_circuit_breaker {
    enable   = true
    rollback = true    # auto-rollback on deployment failure
  }

  depends_on = [aws_lb_listener.https]
}

# CloudWatch Log Group
resource "aws_cloudwatch_log_group" "app" {
  name              = "/ecs/${var.project_name}-${var.environment}"
  retention_in_days = var.environment == "prod" ? 90 : 7
}

# SSM Parameter for secret
resource "aws_ssm_parameter" "db_password" {
  name  = "/${var.environment}/${var.project_name}/db_password"
  type  = "SecureString"
  value = "changeme"  # set via: terraform apply -var='db_password=actual_secret'

  lifecycle {
    ignore_changes = [value]   # don't overwrite after initial creation
  }
}

# outputs.tf
output "alb_dns_name" {
  description = "DNS name of the Application Load Balancer"
  value       = aws_lb.app.dns_name
}

output "alb_arn" {
  value = aws_lb.app.arn
}

output "ecs_cluster_name" {
  value = aws_ecs_cluster.main.name
}

output "vpc_id" {
  value = module.vpc.vpc_id
}

output "private_subnets" {
  value = module.vpc.private_subnets
}
```

---

**Q: What are Terraform modules?**

A: Self-contained, reusable packages of Terraform configuration. You call a module and pass
variables; it creates resources. Modules enforce consistency and DRY (Don't Repeat Yourself).

```hcl
# Calling a local module
module "api_service" {
  source = "./modules/ecs-service"

  name          = "api"
  environment   = var.environment
  image         = "registry.company.com/api:${var.image_tag}"
  desired_count = 3
  cpu           = 512
  memory        = 1024
  port          = 8080
  vpc_id        = module.vpc.vpc_id
  subnets       = module.vpc.private_subnets
}

# Calling a registry module
module "vpc" {
  source  = "terraform-aws-modules/vpc/aws"
  version = "5.1.2"
  # ...
}
```

---

**Q: What is the difference between `count` and `for_each`?**

```hcl
# count — creates N identical resources, referenced by index
resource "aws_instance" "web" {
  count         = 3
  ami           = "ami-12345"
  instance_type = "t3.micro"
  tags          = { Name = "web-${count.index}" }
}
# References: aws_instance.web[0], aws_instance.web[1], aws_instance.web[2]
# Problem: if you remove index 1, Terraform destroys [1] and [2] and recreates [2] as [1].

# for_each — creates resources from a map or set, referenced by key (stable)
resource "aws_iam_user" "engineers" {
  for_each = toset(["alice", "bob", "charlie"])
  name     = each.value
}
# References: aws_iam_user.engineers["alice"], etc.
# Adding/removing "bob" only affects "bob" — alice and charlie are untouched.
# PREFER for_each for named resources. Use count only for truly identical resources.

# for_each with a map (gives you both key and value)
variable "buckets" {
  default = {
    raw       = "us-east-1"
    processed = "us-west-2"
    archive   = "eu-west-1"
  }
}

resource "aws_s3_bucket" "data" {
  for_each = var.buckets
  bucket   = "${var.project}-${each.key}-${var.environment}"
  # each.key = "raw", each.value = "us-east-1"
}
```

---

**Q: What are data sources in Terraform?**

```hcl
# Data sources read existing infrastructure without managing it
# Useful for: referencing resources not managed by this config, dynamic lookups

# Look up the latest Amazon Linux 2 AMI automatically
data "aws_ami" "amazon_linux" {
  most_recent = true
  owners      = ["amazon"]
  filter {
    name   = "name"
    values = ["amzn2-ami-hvm-*-x86_64-gp2"]
  }
}

resource "aws_instance" "web" {
  ami           = data.aws_ami.amazon_linux.id   # always up-to-date
  instance_type = "t3.micro"
}

# Look up existing VPC by tag
data "aws_vpc" "existing" {
  tags = { Environment = "production" }
}

# Look up Route53 hosted zone
data "aws_route53_zone" "main" {
  name         = "example.com"
  private_zone = false
}

# Fetch secret from AWS Secrets Manager
data "aws_secretsmanager_secret_version" "db" {
  secret_id = "prod/myapp/database"
}
locals {
  db_creds = jsondecode(data.aws_secretsmanager_secret_version.db.secret_string)
}
```

---

**Q: How do you handle sensitive data in Terraform?**

```hcl
# 1. Mark variables as sensitive — values are redacted in plan/apply output
variable "db_password" {
  type      = string
  sensitive = true
}

# 2. Mark outputs as sensitive
output "connection_string" {
  value     = "postgresql://user:${var.db_password}@${aws_db_instance.main.address}/mydb"
  sensitive = true
}

# 3. Use AWS SSM Parameter Store or Secrets Manager
data "aws_ssm_parameter" "db_password" {
  name            = "/prod/myapp/db_password"
  with_decryption = true
}

# 4. Pass secrets via environment variables (not in .tfvars files)
# export TF_VAR_db_password="mysecret"
# terraform apply

# 5. Remote state is encrypted (S3 with encrypt=true + KMS key)

# NEVER commit these to Git:
# *.tfvars containing secrets
# terraform.tfstate
# .terraform/
```

---

**Q: What happens if Terraform state gets out of sync with real infrastructure?**

```bash
# Scenario: Someone deleted a resource manually in AWS console.
# Terraform still thinks it exists. Apply would fail.

# Solution 1: terraform refresh — update state from real infrastructure
terraform refresh

# Solution 2: Remove from state (leaves resource alone, Terraform forgets it)
terraform state rm aws_instance.web

# Solution 3: Import existing resource into state
terraform import aws_instance.web i-1234567890abcdef0

# Solution 4: Targeted destroy + recreate
terraform destroy -target=aws_instance.web
terraform apply -target=aws_instance.web
```

---

**Q: What is `terraform taint` / `terraform apply -replace`?**

A: Force the recreation of a specific resource on the next apply, even if no config change.
`terraform taint` was the old command. Since Terraform 0.15.2, use `-replace`:

```bash
terraform apply -replace="aws_instance.web"
# Terraform will destroy and recreate aws_instance.web
# Useful when: an EC2 instance is degraded, AMI needs to be refreshed, etc.
```

---

**Q: What are lifecycle meta-arguments?**

```hcl
resource "aws_instance" "web" {
  ami           = data.aws_ami.amazon_linux.id
  instance_type = "t3.micro"

  lifecycle {
    # Create the new resource BEFORE destroying the old one
    # Prevents downtime during replacement
    create_before_destroy = true

    # Never destroy this resource (e.g., production database)
    prevent_destroy = true

    # Ignore changes to these attributes after initial creation
    # (e.g., if auto-scaling changes the instance count externally)
    ignore_changes = [
      ami,
      tags["LastModified"],
    ]

    # Trigger replacement if this value changes
    replace_triggered_by = [
      aws_launch_template.web.latest_version
    ]
  }
}
```

---

## SECTION 3 — Ansible

**Q: What is Ansible? Key concepts.**

```
Ansible is an agentless configuration management and automation tool.
Connects to target hosts via SSH (Linux) or WinRM (Windows).
No software needed on managed nodes (only Python and SSH).

Control Node:  the machine running Ansible (your laptop, CI server, etc.)
Managed Node:  a server being configured by Ansible
Inventory:     list of managed nodes (static file or dynamic from AWS/GCP/Azure)
Playbook:      YAML file defining a set of tasks to execute on hosts
Task:          a single unit of work (install a package, copy a file, start a service)
Module:        reusable unit that performs a task (apt, copy, service, template, etc.)
Role:          a reusable, structured collection of tasks, handlers, templates, variables
Handler:       a task triggered by a notify — runs once at the end of a play
Fact:          information gathered about a host (OS, IP, memory, hostname, etc.)
Variable:      value used in playbooks/templates. Overridable at multiple levels.
```

---

### Full Playbook Example — Configure a web server

```yaml
---
# site.yml
- name: Configure application servers
  hosts: webservers
  become: yes             # run as root (sudo)
  gather_facts: yes       # collect host facts first

  vars:
    app_port: 8080
    app_version: "2.1.0"
    nginx_worker_processes: "auto"

  vars_files:
    - vars/secrets.yml    # encrypted with ansible-vault

  pre_tasks:
    - name: Update apt cache
      ansible.builtin.apt:
        update_cache: yes
        cache_valid_time: 3600   # only update if cache is older than 1 hour

  tasks:
    - name: Install required packages
      ansible.builtin.apt:
        name:
          - nginx
          - python3
          - python3-pip
          - curl
          - git
        state: present

    - name: Create application user
      ansible.builtin.user:
        name: appuser
        system: yes
        shell: /bin/false
        create_home: no
        comment: "Application service account"

    - name: Create application directories
      ansible.builtin.file:
        path: "{{ item }}"
        state: directory
        owner: appuser
        group: appuser
        mode: '0755'
      loop:
        - /opt/myapp
        - /opt/myapp/logs
        - /opt/myapp/config

    - name: Deploy application binary
      ansible.builtin.copy:
        src: "files/myapp-{{ app_version }}"
        dest: /opt/myapp/myapp
        owner: appuser
        group: appuser
        mode: '0755'
      notify: Restart myapp

    - name: Deploy application config from template
      ansible.builtin.template:
        src: templates/app.conf.j2
        dest: /opt/myapp/config/app.conf
        owner: appuser
        group: appuser
        mode: '0640'
      notify: Restart myapp

    - name: Deploy nginx config
      ansible.builtin.template:
        src: templates/nginx.conf.j2
        dest: /etc/nginx/sites-available/myapp
        owner: root
        group: root
        mode: '0644'
      notify: Reload nginx

    - name: Enable nginx site
      ansible.builtin.file:
        src: /etc/nginx/sites-available/myapp
        dest: /etc/nginx/sites-enabled/myapp
        state: link
      notify: Reload nginx

    - name: Deploy systemd service unit
      ansible.builtin.template:
        src: templates/myapp.service.j2
        dest: /etc/systemd/system/myapp.service
        owner: root
        group: root
        mode: '0644'
      notify:
        - Reload systemd
        - Restart myapp

    - name: Enable and start myapp service
      ansible.builtin.systemd:
        name: myapp
        state: started
        enabled: yes
        daemon_reload: yes

    - name: Ensure nginx is started and enabled
      ansible.builtin.service:
        name: nginx
        state: started
        enabled: yes

    - name: Wait for application to be ready
      ansible.builtin.uri:
        url: "http://localhost:{{ app_port }}/health"
        status_code: 200
      retries: 10
      delay: 3
      register: health_check
      until: health_check.status == 200

    - name: Open application port in firewall
      ansible.builtin.ufw:
        rule: allow
        port: "{{ app_port }}"
        proto: tcp

  handlers:
    - name: Reload nginx
      ansible.builtin.service:
        name: nginx
        state: reloaded

    - name: Reload systemd
      ansible.builtin.systemd:
        daemon_reload: yes

    - name: Restart myapp
      ansible.builtin.service:
        name: myapp
        state: restarted

  post_tasks:
    - name: Verify deployment
      ansible.builtin.debug:
        msg: "Application deployed successfully on {{ ansible_hostname }} ({{ ansible_default_ipv4.address }})"
```

---

### Inventory examples

```ini
# inventory/production.ini (static inventory)

[webservers]
web1.company.com ansible_user=ubuntu
web2.company.com ansible_user=ubuntu
web3.company.com ansible_user=ubuntu

[databases]
db1.company.com  ansible_user=ec2-user  ansible_port=22
db2.company.com  ansible_user=ec2-user

[loadbalancers]
lb1.company.com

# Variables for the webservers group
[webservers:vars]
ansible_python_interpreter=/usr/bin/python3
ansible_ssh_private_key_file=~/.ssh/prod_key.pem
app_port=8080

# Group of groups
[production:children]
webservers
databases
loadbalancers

[all:vars]
ansible_ssh_common_args='-o StrictHostKeyChecking=no -o ConnectTimeout=10'
```

```yaml
# inventory/hosts.yml (YAML format — cleaner for complex inventories)
all:
  children:
    webservers:
      hosts:
        web1.company.com:
          ansible_user: ubuntu
          app_port: 8080
        web2.company.com:
          ansible_user: ubuntu
          app_port: 8080
      vars:
        ansible_python_interpreter: /usr/bin/python3

    databases:
      hosts:
        db1.company.com:
          ansible_user: ec2-user
          db_port: 5432
```

---

### Ansible Roles structure

```
roles/
  myapp/
    tasks/
      main.yml        # main list of tasks
      install.yml     # tasks for installation
      configure.yml   # tasks for configuration
    handlers/
      main.yml        # handlers
    templates/
      app.conf.j2     # Jinja2 config template
      nginx.conf.j2
    files/
      myapp           # static files to copy
    vars/
      main.yml        # role variables (high priority)
    defaults/
      main.yml        # default variables (lowest priority, easily overridden)
    meta/
      main.yml        # role dependencies
    README.md
```

```yaml
# roles/myapp/defaults/main.yml
app_port: 8080
app_user: appuser
app_dir: /opt/myapp
log_level: INFO
max_connections: 100

# roles/myapp/tasks/main.yml
---
- name: Install dependencies
  import_tasks: install.yml

- name: Configure application
  import_tasks: configure.yml
  when: app_configured | default(false)
```

---

### Jinja2 template example

```jinja2
{# templates/app.conf.j2 #}
[server]
host = 0.0.0.0
port = {{ app_port }}
environment = {{ environment }}
log_level = {{ log_level | default('INFO') }}

[database]
host = {{ db_host }}
port = {{ db_port | default(5432) }}
name = {{ db_name }}
# Password comes from vault
password = {{ db_password }}
pool_size = {{ db_pool_size | default(10) }}

[cache]
{% if redis_enabled | default(false) %}
type = redis
host = {{ redis_host }}
port = {{ redis_port | default(6379) }}
{% else %}
type = memory
{% endif %}

[workers]
{% for worker in workers %}
worker_{{ loop.index }} = {{ worker.host }}:{{ worker.port }}
{% endfor %}
```

---

### Key Ansible modules

```yaml
# File management
- ansible.builtin.copy:         # copy static files from control node to target
- ansible.builtin.template:     # render Jinja2 template and copy to target
- ansible.builtin.file:         # create/delete files, dirs, symlinks, set permissions
- ansible.builtin.lineinfile:   # ensure a line exists in a file
- ansible.builtin.blockinfile:  # insert/replace a block of lines in a file
- ansible.builtin.fetch:        # copy files FROM target TO control node
- ansible.builtin.synchronize:  # rsync-based file sync

# Package management
- ansible.builtin.apt:          # Debian/Ubuntu packages
- ansible.builtin.yum:          # RHEL/CentOS packages
- ansible.builtin.dnf:          # Fedora/newer RHEL packages
- ansible.builtin.pip:          # Python packages
- ansible.builtin.npm:          # Node.js packages

# Service management
- ansible.builtin.service:      # start/stop/restart/enable services
- ansible.builtin.systemd:      # systemd-specific (daemon_reload, etc.)

# User management
- ansible.builtin.user:         # manage user accounts
- ansible.builtin.group:        # manage groups
- ansible.builtin.authorized_key:  # manage SSH authorized keys

# Commands
- ansible.builtin.command:      # run command (no shell — safer, no env vars, no redirects)
- ansible.builtin.shell:        # run through shell (has env, pipes, redirects)
- ansible.builtin.script:       # run a local script on the remote host
- ansible.builtin.raw:          # run raw SSH command (no Python needed on target)

# Cloud
- amazon.aws.ec2_instance:      # manage EC2 instances
- amazon.aws.s3_object:         # manage S3 objects
- amazon.aws.route53:           # manage Route53 records

# Utility
- ansible.builtin.debug:        # print messages and variables
- ansible.builtin.set_fact:     # set variables dynamically
- ansible.builtin.include_tasks: # include a task file conditionally
- ansible.builtin.import_tasks:  # statically import a task file
- ansible.builtin.assert:       # fail if conditions are not met
- ansible.builtin.uri:          # make HTTP requests (health checks, API calls)
- ansible.builtin.wait_for:     # wait for ports, files, or conditions
- ansible.builtin.pause:        # pause execution (wait or prompt for input)
```

---

### Ansible Vault — encrypt sensitive data

```bash
# Encrypt a file
ansible-vault encrypt vars/secrets.yml

# Decrypt a file
ansible-vault decrypt vars/secrets.yml

# Edit an encrypted file
ansible-vault edit vars/secrets.yml

# View without decrypting to disk
ansible-vault view vars/secrets.yml

# Encrypt a single string (to embed in a plaintext YAML file)
ansible-vault encrypt_string 'my_database_password' --name 'db_password'

# Run playbook with vault password
ansible-playbook site.yml --ask-vault-pass
ansible-playbook site.yml --vault-password-file ~/.vault_pass

# Rekey (change the vault password)
ansible-vault rekey vars/secrets.yml
```

---

### Ansible CLI commands

```bash
# Run a playbook
ansible-playbook -i inventory/production.ini site.yml

# Limit to specific hosts or groups
ansible-playbook site.yml -l webservers
ansible-playbook site.yml -l web1.company.com,web2.company.com

# Run only specific tags
ansible-playbook site.yml --tags "nginx,app"
ansible-playbook site.yml --skip-tags "database"

# Dry run (check mode — no changes made)
ansible-playbook site.yml --check
ansible-playbook site.yml --check --diff   # also show file diffs

# Run ad-hoc commands (without a playbook)
ansible webservers -i inventory.ini -m ping
ansible all -m command -a "uptime"
ansible webservers -m apt -a "name=nginx state=latest" --become
ansible webservers -m service -a "name=nginx state=restarted" --become

# Gather facts about hosts
ansible webservers -m setup
ansible webservers -m setup -a "filter=ansible_distribution*"

# List hosts that would be affected
ansible-playbook site.yml --list-hosts

# Increase verbosity (up to -vvvv)
ansible-playbook site.yml -v      # task results
ansible-playbook site.yml -vv     # more detail
ansible-playbook site.yml -vvv    # connection details
```

---

## SECTION 4 — IaC Interview Q&A

**Q: How do you manage multiple environments (dev/staging/prod) in Terraform?**

```
Option 1: Workspaces (simple, same backend)
  terraform workspace new staging
  terraform workspace select staging
  terraform apply -var-file=staging.tfvars
  # State is isolated per workspace: terraform.tfstate.d/staging/

Option 2: Directory per environment (recommended for large setups)
  environments/
    dev/
      main.tf       # calls shared modules
      terraform.tfvars
      backend.tf    # dev-specific state bucket
    staging/
      main.tf
      terraform.tfvars
      backend.tf
    prod/
      main.tf
      terraform.tfvars
      backend.tf
  modules/
    vpc/
    ecs-service/
    rds/
  # Full isolation: separate state, separate variables, separate backend.

Option 3: Terragrunt (wrapper tool)
  Adds DRY principles: define shared config once, override per environment.
```

---

**Q: What is Terraform drift and how do you detect it?**

A: Drift occurs when the actual infrastructure state differs from what Terraform expects.
This happens when someone manually changes resources outside of Terraform.

```bash
# Detect drift
terraform plan      # shows differences between state and real infra
terraform refresh   # update state file to match actual infra (without applying)

# Prevention:
# 1. Enforce IaC-only changes via IAM policies (restrict console access)
# 2. Use OPA/Sentinel policies to block non-Terraform changes
# 3. Run terraform plan in CI/CD and alert on unexpected diffs
# 4. Use AWS Config Rules or similar to detect manual changes
```

---

**Q: How do you test Terraform code?**

```
Unit testing:
  - terraform validate    — check syntax
  - terraform fmt --check — check formatting
  - tflint               — lint for best practices, provider-specific rules
  - checkov / tfsec      — static security analysis

Integration testing:
  - Terratest (Go library) — provision real infra, run assertions, destroy
  - Kitchen-Terraform     — Ruby-based testing framework

Plan testing:
  - OPA (Open Policy Agent) / Conftest — policy-as-code for plan output
  - Sentinel (Terraform Cloud) — policy enforcement
  - terratest plan testing — parse JSON plan output and assert

Example tflint config:
  .tflint.hcl:
    plugin "aws" {
      enabled = true
      version = "0.27.0"
      source  = "github.com/terraform-linters/tflint-ruleset-aws"
    }
    rule "aws_instance_invalid_type" { enabled = true }
    rule "aws_instance_previous_type" { enabled = true }
```

---

**Q: What is Ansible idempotency and why does it matter?**

A: An idempotent operation produces the same result whether run once or many times.
Ansible modules are designed to be idempotent:
- `apt: name=nginx state=present` → installs nginx if not present. Does nothing if already installed.
- `file: path=/opt/app state=directory` → creates the dir if it doesn't exist. No-op if it does.
- `service: name=nginx state=started` → starts nginx if not running. No-op if already running.

This means you can safely run `ansible-playbook` multiple times without breaking things.
It's safe to use in CI/CD pipelines that run on every commit.

Non-idempotent anti-patterns:
```yaml
# BAD — runs the command every time, even if package already installed
- name: Install myapp
  ansible.builtin.command: pip install myapp

# GOOD — idempotent
- name: Install myapp
  ansible.builtin.pip:
    name: myapp
    state: present

# When you MUST use command/shell, use creates/removes for idempotency
- name: Run one-time migration
  ansible.builtin.command: python manage.py migrate
  args:
    creates: /opt/app/.migration_done    # skip if this file exists
  notify: Create migration done marker

- name: Create migration done marker
  ansible.builtin.file:
    path: /opt/app/.migration_done
    state: touch
```

---

**Q: How do you do a rolling update with Ansible?**

```yaml
# site.yml — rolling update with serial
- name: Rolling update of web servers
  hosts: webservers
  become: yes
  serial: 1          # update 1 server at a time
  # serial: "25%"    # update 25% of servers at a time
  # serial: [1, 2, 5]  # 1 first, then 2, then 5 at a time

  tasks:
    - name: Remove from load balancer
      ansible.builtin.uri:
        url: "https://lb.company.com/api/backends/{{ inventory_hostname }}/disable"
        method: POST
      delegate_to: localhost   # run this task on the control node, not the target

    - name: Wait for connections to drain
      ansible.builtin.wait_for:
        timeout: 30

    - name: Deploy new version
      ansible.builtin.copy:
        src: myapp-{{ new_version }}
        dest: /opt/myapp/myapp
        mode: '0755'
      notify: Restart myapp

    - name: Wait for app to be healthy
      ansible.builtin.uri:
        url: "http://{{ ansible_host }}:8080/health"
        status_code: 200
      retries: 10
      delay: 3
      register: result
      until: result.status == 200

    - name: Add back to load balancer
      ansible.builtin.uri:
        url: "https://lb.company.com/api/backends/{{ inventory_hostname }}/enable"
        method: POST
      delegate_to: localhost

  handlers:
    - name: Restart myapp
      ansible.builtin.service:
        name: myapp
        state: restarted
```
```

Now the last file: