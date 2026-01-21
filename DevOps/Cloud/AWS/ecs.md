# Amazon ECS (Elastic Container Service) - Complete Guide

## Table of Contents
1. [Introduction](#introduction)
2. [Core Concepts](#core-concepts)
3. [Launch Types](#launch-types)
4. [Task Definitions](#task-definitions)
5. [Services and Tasks](#services-and-tasks)
6. [Networking](#networking)
7. [Storage](#storage)
8. [Security](#security)
9. [Load Balancing](#load-balancing)
10. [Auto Scaling](#auto-scaling)
11. [Monitoring and Logging](#monitoring-and-logging)
12. [Best Practices](#best-practices)
13. [Common Interview Questions](#common-interview-questions)

---

## Introduction

**Amazon ECS** is a fully managed container orchestration service that enables you to run, stop, and manage Docker containers on a cluster. It eliminates the need to install and operate your own container orchestration software.

### Key Features
- **Fully Managed**: No control plane to manage
- **AWS Integration**: Native integration with AWS services
- **Two Launch Types**: EC2 and Fargate
- **High Performance**: Optimized for scale
- **Secure**: IAM integration, VPC isolation, secrets management
- **Cost-Effective**: Pay only for resources used

### Why ECS?
- No additional charge (pay only for underlying resources)
- Deep AWS integration (ALB, CloudWatch, IAM, VPC)
- Simpler than self-managed Kubernetes
- Reliable and battle-tested by AWS
- Supports both Linux and Windows containers

### ECS vs Other Services
| Feature | ECS | EKS | Fargate | Lambda |
|---------|-----|-----|---------|--------|
| Type | Container orchestration | Kubernetes | Serverless containers | Serverless functions |
| Control | High | Highest | Low | Lowest |
| Management | Managed | Managed K8s | Fully managed | Fully managed |
| Cost | EC2 costs | EC2 + EKS fee | Per task | Per invocation |
| Use case | AWS-native apps | K8s workloads | Serverless containers | Event-driven |

---

## Core Concepts

### 1. **Cluster**
Logical grouping of tasks or services
- Regional construct
- Can contain EC2 instances or Fargate tasks
- Can mix EC2 and Fargate launch types
- Free (pay only for resources)

**Example:**
```bash
# Create cluster
aws ecs create-cluster --cluster-name my-cluster
```

### 2. **Task Definition**
Blueprint for your application (like Dockerfile)
- JSON format
- Defines containers to run
- Specifies resource requirements (CPU, memory)
- Defines networking, IAM roles, logging
- Immutable (create new version for changes)
- Can have multiple containers per task

**Key Components:**
- Container definitions
- Task role (IAM permissions)
- Task execution role (ECS agent permissions)
- Network mode
- Volumes
- CPU and memory requirements

### 3. **Task**
Running instance of a task definition
- Represents one or more containers
- Ephemeral (runs and stops)
- Scheduled or long-running
- Can run standalone or as part of service

### 4. **Service**
Maintains desired number of tasks running
- Ensures specified number of tasks always running
- Replaces failed tasks automatically
- Integrates with load balancers
- Enables rolling updates
- Supports auto scaling

### 5. **Container Instance** (EC2 Launch Type)
EC2 instance running ECS container agent
- Registers itself with cluster
- Reports resource availability
- Runs tasks assigned by ECS scheduler

### 6. **ECS Agent**
Software running on container instances
- Communicates with ECS service
- Starts and stops tasks
- Monitors running tasks
- Pre-installed on ECS-optimized AMIs

---

## Launch Types

### 1. **EC2 Launch Type**

You manage the underlying EC2 instances.

**Characteristics:**
- **Control**: Full control over instance type, AMI, networking
- **Cost**: Pay for EC2 instances (even if no containers running)
- **Management**: You handle patching, scaling, monitoring
- **Flexibility**: Can use Spot, Reserved Instances, Savings Plans

**Architecture:**
```
Cluster → EC2 Instances → ECS Agent → Docker → Containers
```

**When to Use:**
- Need specific instance types
- Cost optimization with RIs/Spot
- Consistent baseline workload
- Need GPU, custom kernels, or special hardware
- Large workloads (more cost-effective at scale)

**Setup:**
1. Create cluster
2. Launch EC2 instances with ECS-optimized AMI
3. Install ECS agent (pre-installed in optimized AMI)
4. Instances auto-register with cluster
5. Deploy tasks/services

**Container Instance Requirements:**
- ECS-optimized AMI (or ECS agent installed)
- IAM role for ECS agent
- Security group allowing container communication
- Sufficient resources (CPU, memory, storage)

### 2. **Fargate Launch Type**

AWS manages the infrastructure (serverless containers).

**Characteristics:**
- **Serverless**: No EC2 instances to manage
- **Cost**: Pay per task (per vCPU and memory per second)
- **Management**: AWS handles infrastructure
- **Scaling**: Automatic, no capacity planning
- **Isolation**: Each task in its own isolated environment

**Architecture:**
```
Cluster → Fargate → Containers (Infrastructure abstracted)
```

**When to Use:**
- Don't want to manage servers
- Variable/unpredictable workloads
- Microservices with independent scaling
- Batch jobs
- Small to medium workloads
- Quick deployments without capacity planning

**Fargate Platform Versions:**
- **1.4.0**: Latest features (recommended)
- Platform versions handle patching, updates

**Task Size (CPU and Memory):**
Must choose valid combinations:
```
0.25 vCPU: 0.5 GB, 1 GB, 2 GB
0.5 vCPU:  1 GB to 4 GB (1 GB increments)
1 vCPU:    2 GB to 8 GB (1 GB increments)
2 vCPU:    4 GB to 16 GB (1 GB increments)
4 vCPU:    8 GB to 30 GB (1 GB increments)
8 vCPU:    16 GB to 60 GB (4 GB increments)
16 vCPU:   32 GB to 120 GB (8 GB increments)
```

### EC2 vs Fargate Comparison

| Aspect | EC2 | Fargate |
|--------|-----|---------|
| Management | Self-managed | Fully managed |
| Pricing | Instance-based | Task-based |
| Scaling | Manual instance scaling | Automatic |
| Cost (small) | Higher | Lower |
| Cost (large) | Lower | Higher |
| Flexibility | High | Limited |
| Startup time | Slower | Faster |
| Spot support | Yes | Fargate Spot |
| Persistent storage | EBS, EFS | EFS only |

---

## Task Definitions

### Structure

```json
{
  "family": "my-app",
  "networkMode": "awsvpc",
  "requiresCompatibilities": ["FARGATE"],
  "cpu": "256",
  "memory": "512",
  "taskRoleArn": "arn:aws:iam::123456789012:role/MyTaskRole",
  "executionRoleArn": "arn:aws:iam::123456789012:role/MyExecutionRole",
  "containerDefinitions": [
    {
      "name": "web-app",
      "image": "nginx:latest",
      "portMappings": [
        {
          "containerPort": 80,
          "protocol": "tcp"
        }
      ],
      "essential": true,
      "environment": [
        {
          "name": "ENV_VAR",
          "value": "value"
        }
      ],
      "secrets": [
        {
          "name": "DB_PASSWORD",
          "valueFrom": "arn:aws:secretsmanager:region:account:secret:name"
        }
      ],
      "logConfiguration": {
        "logDriver": "awslogs",
        "options": {
          "awslogs-group": "/ecs/my-app",
          "awslogs-region": "us-east-1",
          "awslogs-stream-prefix": "web"
        }
      },
      "healthCheck": {
        "command": ["CMD-SHELL", "curl -f http://localhost/ || exit 1"],
        "interval": 30,
        "timeout": 5,
        "retries": 3,
        "startPeriod": 60
      }
    }
  ]
}
```

### Key Components

#### 1. **Family**
Name for task definition versions
- Unique identifier
- Multiple revisions (family:1, family:2, etc.)

#### 2. **Network Mode**

**awsvpc** (Recommended, required for Fargate)
- Each task gets own ENI
- Own private IP address
- Security groups at task level
- Better isolation

**bridge** (Default for EC2)
- Docker bridge network
- Port mapping required
- Multiple tasks share instance's network

**host** (EC2 only)
- Direct host network
- No port mapping
- Container ports must not conflict

**none**
- No external connectivity

#### 3. **CPU and Memory**

**Task Level** (Required for Fargate)
- Total resources for all containers
- Specified as: "256" (.25 vCPU), "512" (.5 vCPU), "1024" (1 vCPU)

**Container Level** (Optional for EC2, Required for Fargate)
- Resources for specific container
- Can set hard/soft limits
- Soft limit: Normal allocation
- Hard limit: Maximum allowed

#### 4. **IAM Roles**

**Task Role**
- Permissions for application
- What containers can do
- Access AWS services (S3, DynamoDB, etc.)

**Task Execution Role**
- Permissions for ECS agent
- Pull images from ECR
- Fetch secrets from Secrets Manager
- Send logs to CloudWatch

```json
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": [
        "ecr:GetAuthorizationToken",
        "ecr:BatchCheckLayerAvailability",
        "ecr:GetDownloadUrlForLayer",
        "ecr:BatchGetImage",
        "logs:CreateLogStream",
        "logs:PutLogEvents"
      ],
      "Resource": "*"
    }
  ]
}
```

#### 5. **Container Definitions**

**Essential Containers**
- `essential: true` → If dies, all containers in task stopped
- `essential: false` → Other containers continue running

**Port Mappings**
- **awsvpc mode**: Only containerPort needed
- **bridge mode**: hostPort and containerPort
- Protocol: tcp, udp

**Environment Variables**
- Plaintext: `environment`
- Secrets: `secrets` (from Secrets Manager or SSM Parameter Store)

**Health Checks**
- Command to run
- Interval: Check frequency
- Timeout: Max time to wait
- Retries: Failures before unhealthy
- Start period: Grace period for startup

**Resource Limits**
- CPU shares (relative weight)
- Memory (MB): Hard/soft limits
- GPU: Number of GPUs

#### 6. **Volumes**

**Bind Mounts** (EC2)
- Mount host directory into container

**Docker Volumes** (EC2)
- Managed by Docker

**EFS Volumes**
- Shared filesystem
- Works with both EC2 and Fargate
- Persistent across tasks

**Example:**
```json
"volumes": [
  {
    "name": "my-efs",
    "efsVolumeConfiguration": {
      "fileSystemId": "fs-12345678",
      "transitEncryption": "ENABLED",
      "authorizationConfig": {
        "iam": "ENABLED"
      }
    }
  }
]
```

---

## Services and Tasks

### Services

Long-running applications that maintain desired count.

**Key Features:**
- Desired count: Number of tasks to run
- Minimum healthy percent: During deployment
- Maximum percent: Extra tasks during deployment
- Health checks: Integrate with ALB/NLB
- Service discovery: Register in AWS Cloud Map
- Auto scaling: Scale based on metrics

**Deployment Types:**

#### 1. **Rolling Update** (Default)
- Replace old tasks with new gradually
- Control with min/max healthy percent
- Minimizes downtime

**Configuration:**
```
Desired: 4
Minimum healthy: 50%
Maximum: 200%

Process:
1. Stop 2 tasks (50% = 2 remain)
2. Start 2 new tasks (4 total)
3. Wait for healthy
4. Stop remaining 2 old
5. Start 2 more new
```

#### 2. **Blue/Green Deployment**
- Requires CodeDeploy
- Creates new version (green)
- Shifts traffic gradually
- Can rollback instantly
- Zero downtime

**Traffic Shifting:**
- All-at-once
- Linear: Equal increments over time
- Canary: Small percentage, then remaining

#### 3. **External Deployment**
- Use third-party tools
- Custom deployment logic

**Deployment Circuit Breaker**
- Automatically rollback failed deployments
- Monitors health checks
- Prevents cascading failures

### Tasks (Standalone)

One-time or scheduled tasks.

**Types:**

**Run Task**
- One-time execution
- Batch jobs
- Manual operations

**Scheduled Tasks**
- Cron-based scheduling
- Uses EventBridge Rules
- Example: Nightly data processing

**Example:**
```bash
# Run one-time task
aws ecs run-task \
  --cluster my-cluster \
  --task-definition my-task:1 \
  --count 1 \
  --launch-type FARGATE \
  --network-configuration "awsvpcConfiguration={subnets=[subnet-xxx],securityGroups=[sg-xxx]}"
```

---

## Networking

### Network Modes

#### 1. **awsvpc** (Recommended)
- Each task gets ENI
- Own private IP
- Security groups at task level
- Required for Fargate
- Best isolation

**Benefits:**
- Task-level security groups
- VPC Flow Logs per task
- Better monitoring
- PrivateLink support

**Considerations:**
- ENI limits per instance
- VPC IP exhaustion

#### 2. **bridge** (EC2 Default)
- Docker bridge network
- Port mapping: hostPort:containerPort
- Multiple tasks share instance network
- Less isolation

#### 3. **host** (EC2 Only)
- Use host network directly
- No port mapping
- Container ports cannot conflict
- Best performance (no NAT)

### VPC Configuration

**Public Subnet + Public IP**
- Direct internet access
- Assign public IP to task
- For internet-facing services

**Private Subnet + NAT Gateway**
- Outbound internet via NAT
- No inbound from internet
- Recommended for most workloads
- More secure

**Private Subnet + VPC Endpoints**
- No internet required
- Access AWS services via PrivateLink
- Most secure
- Lower data transfer costs

### Service Discovery

**AWS Cloud Map Integration**
- DNS-based service discovery
- Automatic registration/deregistration
- Namespace: DNS domain
- Service: Maps to ECS service

**Types:**
- API-based: Query Cloud Map API
- DNS-based: Standard DNS queries

**Example:**
```
Service: api-service
Namespace: local
DNS: api-service.local

Tasks registered automatically:
- 10.0.1.5:8080
- 10.0.1.6:8080
```

**Benefits:**
- No hard-coded endpoints
- Automatic failover
- Service mesh compatibility

---

## Storage

### Ephemeral Storage

**Task Storage (Fargate)**
- 20 GB default ephemeral storage
- Can configure up to 200 GB
- Lost when task stops
- For temporary files, caching

### Persistent Storage

#### **EFS (Elastic File System)**
- NFS-based shared filesystem
- Multiple tasks can mount simultaneously
- Data persists across task restarts
- Works with EC2 and Fargate
- Supports encryption

**Use Cases:**
- Shared content (CMS, media files)
- Persistent application data
- Configuration files
- Logs aggregation

**Configuration:**
```json
"volumes": [
  {
    "name": "efs-volume",
    "efsVolumeConfiguration": {
      "fileSystemId": "fs-12345678",
      "rootDirectory": "/data",
      "transitEncryption": "ENABLED",
      "authorizationConfig": {
        "accessPointId": "fsap-12345678",
        "iam": "ENABLED"
      }
    }
  }
]
```

#### **EBS Volumes** (EC2 Only)
- Block storage attached to EC2 instance
- Not shared between tasks
- Higher performance than EFS
- Use bind mounts to access

#### **Docker Volumes** (EC2 Only)
- Managed by Docker daemon
- Can use volume drivers

### Bind Mounts (EC2 Only)

Mount host directories into containers:
```json
"mountPoints": [
  {
    "sourceVolume": "my-volume",
    "containerPath": "/data"
  }
]
```

---

## Security

### 1. **IAM Roles**

**Task Role**
- Application permissions
- Access to AWS services
- S3, DynamoDB, SQS, etc.
- Best practice: Least privilege

**Task Execution Role**
- ECS agent permissions
- Pull ECR images
- Send CloudWatch logs
- Retrieve secrets

**EC2 Instance Role** (EC2 Launch Type)
- ECS agent communication
- Pull images
- Access CloudWatch

### 2. **Secrets Management**

**AWS Secrets Manager**
- Automatic rotation
- Fine-grained access control
- Audit trail
- Versioning

**SSM Parameter Store**
- Simpler than Secrets Manager
- No automatic rotation
- Lower cost
- Integrated with IAM

**Configuration:**
```json
"secrets": [
  {
    "name": "DB_PASSWORD",
    "valueFrom": "arn:aws:secretsmanager:region:account:secret:db-password-xxx"
  },
  {
    "name": "API_KEY",
    "valueFrom": "arn:aws:ssm:region:account:parameter/api-key"
  }
]
```

**Benefits:**
- No secrets in code/images
- Centralized management
- Rotation without redeployment
- Audit access

### 3. **Security Groups**

**awsvpc Mode:**
- Security groups at task level
- Granular control
- Different rules per service

**bridge/host Mode:**
- Security groups at instance level
- Less granular

**Best Practices:**
- Separate security groups per service
- Allow only required ports
- Use security group references (not CIDR)
- Restrict egress traffic

### 4. **Network Isolation**

**Private Subnets**
- No public IPs
- Outbound via NAT
- More secure

**VPC Endpoints**
- Private connection to AWS services
- No internet required
- ECR, S3, CloudWatch, Secrets Manager

### 5. **Container Security**

**Image Scanning**
- ECR integrated scanning
- Finds vulnerabilities
- Scan on push
- Continuous scanning

**Read-only Root Filesystem**
```json
"readonlyRootFilesystem": true
```

**Privileged Mode**
- Avoid unless necessary
- Gives container elevated privileges
```json
"privileged": false
```

**User**
- Run as non-root user
```json
"user": "1000:1000"
```

### 6. **Compliance and Auditing**

**CloudTrail**
- Log all API calls
- Who did what, when
- Security audits

**VPC Flow Logs**
- Network traffic logs
- Security analysis
- Troubleshooting

**CloudWatch Logs**
- Application logs
- Container stdout/stderr
- Search and analysis

---

## Load Balancing

### Application Load Balancer (ALB)

**Features:**
- Layer 7 (HTTP/HTTPS)
- Path-based routing
- Host-based routing
- Dynamic port mapping (bridge mode)
- Health checks
- WebSocket support

**Target Types:**
- **IP** (awsvpc mode): Task IP addresses
- **Instance** (bridge mode): EC2 instance IPs

**Dynamic Port Mapping** (EC2 bridge mode)
- Container uses ephemeral host port
- ALB automatically discovers port
- Multiple tasks per instance

**Configuration:**
```json
"portMappings": [
  {
    "containerPort": 80,
    "protocol": "tcp"
  }
]
```

**Health Checks:**
- Path: /health
- Interval: 30 seconds
- Timeout: 5 seconds
- Healthy threshold: 2
- Unhealthy threshold: 2

### Network Load Balancer (NLB)

**Features:**
- Layer 4 (TCP/UDP)
- Ultra-high performance
- Static IP support
- Preserve source IP
- Long-lived connections

**Use Cases:**
- Non-HTTP protocols
- Extreme performance needs
- Static IP requirements

### Service Integration

**Load Balancer Configuration:**
```json
"loadBalancers": [
  {
    "targetGroupArn": "arn:aws:elasticloadbalancing:...",
    "containerName": "web-app",
    "containerPort": 80
  }
]
```

**How it Works:**
1. ECS service creates tasks
2. Tasks register with target group
3. ALB/NLB distributes traffic
4. Health checks monitor task health
5. Failed tasks deregistered and replaced

**Multiple Target Groups:**
- Multiple containers can register
- Different ports
- Different protocols

---

## Auto Scaling

### Service Auto Scaling

Automatically adjust desired task count based on metrics.

**Scaling Policies:**

#### 1. **Target Tracking**
Maintain target metric value
```json
{
  "TargetValue": 75.0,
  "PredefinedMetricSpecification": {
    "PredefinedMetricType": "ECSServiceAverageCPUUtilization"
  }
}
```

**Predefined Metrics:**
- ECSServiceAverageCPUUtilization
- ECSServiceAverageMemoryUtilization
- ALBRequestCountPerTarget

**Custom Metrics:**
- Any CloudWatch metric
- Application-specific metrics

#### 2. **Step Scaling**
Scale based on CloudWatch alarms
```
CPU > 80%: Add 2 tasks
CPU > 90%: Add 4 tasks
CPU < 30%: Remove 1 task
```

#### 3. **Scheduled Scaling**
Predictable patterns
```
Weekdays 9 AM: Scale to 10 tasks
Weekdays 6 PM: Scale to 2 tasks
```

### Cluster Auto Scaling (EC2)

**Capacity Providers**
- Manage EC2 instance scaling
- ASG-based scaling
- Target tracking for cluster capacity

**Managed Scaling:**
```json
{
  "Status": "ENABLED",
  "TargetCapacity": 100,
  "MinimumScalingStepSize": 1,
  "MaximumScalingStepSize": 10
}
```

**How it Works:**
1. ECS monitors CapacityProviderReservation metric
2. When reservation high, scale out ASG
3. When reservation low, scale in ASG
4. Maintains target capacity percentage

### Fargate Auto Scaling

No cluster scaling needed!
- Only service auto scaling
- AWS handles infrastructure

---

## Monitoring and Logging

### CloudWatch Metrics

**Service Metrics:**
- CPUUtilization
- MemoryUtilization
- DesiredTaskCount
- RunningTaskCount
- PendingTaskCount

**Task Metrics:**
- Per-task CPU and memory
- Network metrics
- Storage metrics

**Cluster Metrics:**
- CPU/memory reservation
- Running tasks count

### CloudWatch Logs

**Configuration:**
```json
"logConfiguration": {
  "logDriver": "awslogs",
  "options": {
    "awslogs-group": "/ecs/my-app",
    "awslogs-region": "us-east-1",
    "awslogs-stream-prefix": "ecs",
    "awslogs-create-group": "true"
  }
}
```

**Log Drivers:**
- **awslogs**: CloudWatch Logs (most common)
- **json-file**: Local JSON files
- **syslog**: Syslog daemon
- **fluentd**: Fluentd logging
- **splunk**: Splunk logging

### Container Insights

Enhanced monitoring for containers
- Additional metrics
- Performance insights
- Resource utilization maps
- Automatic dashboards

**Enable:**
```bash
aws ecs update-cluster-settings \
  --cluster my-cluster \
  --settings name=containerInsights,value=enabled
```

### AWS X-Ray

Distributed tracing
- Track requests through microservices
- Identify bottlenecks
- Debug and analyze

**Configuration:**
- Add X-Ray daemon as sidecar
- Instrument application code
- View service map

### CloudWatch Alarms

**Common Alarms:**
- High CPU utilization
- High memory utilization
- Service deployment failures
- Task stopped unexpectedly
- Health check failures

---

## Best Practices

### 1. **Design and Architecture**

**Microservices:**
- One service per container
- Loose coupling
- Independent deployment
- Service discovery

**Container Design:**
- Single responsibility
- Stateless when possible
- Small images
- No secrets in images
- Use official base images

**Multi-Container Tasks:**
- Sidecar pattern: Logging, monitoring
- Ambassador pattern: Proxy connections
- Adapter pattern: Normalize output

### 2. **Resource Management**

**Right-Sizing:**
- Start with estimates
- Monitor actual usage
- Adjust based on metrics
- Over-provision slightly for spikes

**Resource Limits:**
- Always set CPU and memory limits
- Prevent resource starvation
- Hard limit prevents OOM

**Fargate Sizing:**
- Choose smallest size that works
- Scale horizontally, not vertically
- Consider task granularity

### 3. **High Availability**

**Multi-AZ Deployment:**
- Deploy tasks across AZs
- Automatic failover
- Resilience to AZ failures

**Health Checks:**
- Implement proper health endpoints
- Check dependencies (DB, cache)
- Graceful degradation

**Desired Count:**
- Minimum 2 tasks per service
- Distribute across AZs
- Consider peak load

### 4. **Security**

**Least Privilege:**
- Minimal IAM permissions
- Task roles per service
- No wildcard permissions

**Network Segmentation:**
- Private subnets
- Security groups per service
- VPC endpoints

**Image Security:**
- Scan images regularly
- Use minimal base images
- Keep images updated
- No secrets in images

**Secrets:**
- Use Secrets Manager/SSM
- Rotate secrets regularly
- Audit secret access

### 5. **Performance**

**Networking:**
- Use awsvpc mode
- Place compute near data
- Use VPC endpoints
- Consider network latency

**Storage:**
- Use EFS for shared data
- Ephemeral for temporary
- Optimize I/O patterns

**Caching:**
- Implement caching layers
- Use ElastiCache
- CDN for static content

### 6. **Cost Optimization**

**Fargate vs EC2:**
- Fargate: Variable workloads
- EC2: Steady, large workloads
- Mix both: Baseline on EC2, burst on Fargate

**Fargate Spot:**
- Up to 70% discount
- For fault-tolerant workloads
- Batch processing, testing

**EC2 Cost Optimization:**
- Reserved Instances
- Savings Plans
- Spot Instances
- Right-size instances

**Resource Utilization:**
- Pack tasks efficiently
- Monitor and eliminate waste
- Use auto scaling

### 7. **Deployment**

**Blue/Green:**
- Zero downtime
- Easy rollback
- Test in production-like environment

**Rolling Updates:**
- Configure min/max healthy
- Monitor deployment
- Set deployment timeouts

**Circuit Breaker:**
- Enable for critical services
- Prevent bad deployments
- Automatic rollback

### 8. **Monitoring**

**Comprehensive Monitoring:**
- Enable Container Insights
- CloudWatch metrics and logs
- X-Ray for tracing
- Application-specific metrics

**Alerting:**
- Set meaningful thresholds
- Avoid alert fatigue
- Escalation procedures

---

## Common Interview Questions

### Basic Level

**Q1: What is Amazon ECS?**
A: Amazon ECS is a fully managed container orchestration service that allows you to run, stop, and manage Docker containers on a cluster. It handles container scheduling, placement, and scaling without you managing the orchestration infrastructure.

**Q2: What are the two launch types in ECS?**
A:
- **EC2 Launch Type**: You manage EC2 instances that host containers. More control, better for large-scale workloads.
- **Fargate Launch Type**: AWS manages infrastructure (serverless). No server management, pay per task.

**Q3: What is a Task Definition?**
A: A Task Definition is a JSON blueprint that describes how to run a Docker container. It includes:
- Container image
- CPU and memory requirements
- Port mappings
- Environment variables
- IAM roles
- Networking configuration
- Logging settings

**Q4: What is the difference between a Task and a Service?**
A:
- **Task**: Running instance of a task definition. One-time or scheduled execution.
- **Service**: Maintains desired number of tasks running. Long-running applications with automatic recovery and load balancing.

**Q5: What is the ECS Container Agent?**
A: Software that runs on EC2 instances (EC2 launch type) to communicate with ECS service. It:
- Registers instance with cluster
- Reports resource availability
- Starts and stops containers
- Monitors running tasks

### Intermediate Level

**Q6: Explain the different network modes in ECS.**
A:
- **awsvpc**: Each task gets own ENI and private IP. Task-level security groups. Required for Fargate.
- **bridge**: Default for EC2. Uses Docker bridge network. Port mapping required.
- **host**: Uses host network directly. No port mapping. Best performance but less isolation.
- **none**: No network connectivity.

**Q7: What are IAM roles in ECS and why do we need two different roles?**
A:
- **Task Role**: Permissions for the application running in container (e.g., access S3, DynamoDB)
- **Task Execution Role**: Permissions for ECS agent to pull images, send logs, fetch secrets
They're separate because they serve different purposes - one for the app, one for the infrastructure.

**Q8: How does service auto scaling work in ECS?**
A: ECS Service Auto Scaling adjusts the desired task count based on metrics:
- **Target Tracking**: Maintains target metric (e.g., 70% CPU)
- **Step Scaling**: Different scaling actions at different thresholds
- **Scheduled Scaling**: Scale at specific times
Works with CloudWatch metrics and alarms.

**Q9: What is dynamic port mapping and when is it used?**
A: In EC2 bridge mode, dynamic port mapping allows:
- Container uses fixed port internally
- Maps to ephemeral host port (32768-65535)
- ALB automatically discovers the port
- Multiple tasks of same type can run on single instance
Not needed in awsvpc mode since each task has own IP.

**Q10: How do you implement service discovery in ECS?**
A: Using AWS Cloud Map:
- Create namespace (DNS domain)
- ECS service automatically registers tasks
- Other services query via DNS or API
- Automatic deregistration when task stops
Enables microservices to find each other without hard-coded endpoints.

### Advanced Level

**Q11: Compare ECS EC2 vs Fargate for a large-scale application. When would you choose each?**
A:
**Choose EC2 when:**
- Large, steady workload (RIs make it cheaper)
- Need specific instance types or features
- Require GPU, custom AMI, or special hardware
- Cost optimization at scale
- Need persistent storage beyond EFS

**Choose Fargate when:**
- Variable, unpredictable workload
- Want zero infrastructure management
- Microservices with independent scaling
- Small to medium workload
- Rapid deployment without capacity planning
- Strong isolation requirements

**Best practice**: Use both - EC2 for baseline, Fargate for bursts.

**Q12: Design a highly available microservices architecture using ECS.**
A:
```
Architecture:
1. Multi-region deployment (Route 53 for failover)
2. Per region:
   - ECS cluster spanning 3 AZs
   - Each service: min 2 tasks across AZs
   - Application Load Balancer distributing traffic
   - Private subnets + NAT Gateway
   - RDS Multi-AZ for database
   - ElastiCache Multi-AZ for caching
   - EFS for shared storage
3. Service discovery via Cloud Map
4. Auto scaling per service
5. Blue/green deployments via CodeDeploy
6. Container Insights + CloudWatch alarms
7. Secrets Manager for credentials
8. VPC endpoints for AWS services

Key principles:
- No single point of failure
- Graceful degradation
- Auto recovery
- Zero-downtime deployments
```

**Q13: How do you handle secrets and sensitive data in ECS?**
A:
**Best Practices:**
1. **Never** hardcode in images or environment variables
2. Use AWS Secrets Manager or SSM Parameter Store
3. Reference secrets in task definition using `secrets` parameter
4. Grant Task Execution Role permission to read secrets
5. Enable encryption at rest and in transit
6. Implement secret rotation
7. Audit access via CloudTrail
8. Use IAM policies for fine-grained access

**Implementation:**
```json
"secrets": [{
  "name": "DB_PASSWORD",
  "valueFrom": "arn:aws:secretsmanager:region:account:secret:name"
}]
```

Secrets automatically injected as environment variables at runtime.

**Q14: Explain ECS task placement strategies and constraints.**
A:
**Placement Strategies** (how to distribute tasks):
- **spread**: Distribute across AZs or instances (high availability)
- **binpack**: Pack tasks on fewest instances (cost optimization)
- **random**: Random placement

**Placement Constraints** (where tasks can run):
- **distinctInstance**: Each task on different instance
- **memberOf**: Use cluster query language for custom rules

**Example:**
```json
{
  "placementStrategy": [
    { "type": "spread", "field": "availability-zone" },
    { "type": "binpack", "field": "memory" }
  ],
  "placementConstraints": [
    { "type": "memberOf", "expression": "attribute:ecs.instance-type =~ t3.*" }
  ]
}
```

Only applies to EC2 launch type (Fargate handles automatically).

**Q15: How do you implement blue/green deployments in ECS?**
A:
**Using AWS CodeDeploy:**
1. Create two target groups (blue and green)
2. ALB listens on ports
3. Deploy new version to green environment
4. CodeDeploy gradually shifts traffic
5. Monitor health and metrics
6. Rollback if issues detected

**Traffic Shifting Options:**
- **Canary**: Small % first (10%), then all
- **Linear**: Equal increments (10% every 10 min)
- **All-at-once**: Immediate switch

**Benefits:**
- Zero downtime
- Easy rollback
- Test in production
- Gradual validation

**Automated Rollback Triggers:**
- CloudWatch alarms
- Deployment failure
- Health check failures

**Q16: What are capacity providers and how do they work?**
A: Capacity providers manage infrastructure for ECS tasks:

**Fargate Capacity Providers:**
- FARGATE: Standard Fargate
- FARGATE_SPOT: Fargate Spot (70% discount, can be interrupted)

**EC2 Capacity Providers:**
- Link to Auto Scaling Group
- Managed scaling enabled
- Target capacity percentage (e.g., 100%)
- ECS automatically scales ASG based on task demand

**Strategy:**
```json
{
  "capacityProviders": ["FARGATE", "FARGATE_SPOT"],
  "defaultCapacityProviderStrategy": [
    { "capacityProvider": "FARGATE_SPOT", "weight": 1, "base": 0 },
    { "capacityProvider": "FARGATE", "weight": 1, "base": 2 }
  ]
}
```
This runs 2 tasks on Fargate (base), then splits additional tasks 50/50 between Fargate and Spot.

**Q17: How do you troubleshoot an ECS task that keeps failing?**
A:
**Troubleshooting Steps:**

1. **Check Service Events:**
```bash
aws ecs describe-services --cluster my-cluster --services my-service
```
Look for recent events and errors

2. **Check Task Stopped Reason:**
```bash
aws ecs describe-tasks --cluster my-cluster --tasks task-id
```
Check `stoppedReason` and `stopCode`

3. **Common Issues:**
- **Insufficient resources**: Not enough CPU/memory on cluster
- **Image pull errors**: ECR permissions, image doesn't exist
- **Health check failures**: Container unhealthy
- **Port conflicts**: Port already in use (bridge mode)
- **ENI limit**: Too many tasks (awsvpc mode)
- **Security group**: Blocking required traffic

4. **Check CloudWatch Logs:**
Review container logs for application errors

5. **Check Task Execution Role:**
Permissions to pull images and write logs

6. **Verify Network:**
Security groups, route tables, NAT gateway

7. **Test Locally:**
Run same image locally with same config

**Q18: How do you optimize ECS costs?**
A:
**Strategies:**

1. **Right-size tasks**: Don't over-provision CPU/memory
2. **Use Fargate Spot**: 70% discount for fault-tolerant workloads
3. **EC2 Savings Plans**: For predictable workloads
4. **EC2 Spot Instances**: For batch processing
5. **Auto scaling**: Match capacity to demand
6. **Efficient packing**: binpack placement strategy
7. **Remove unused resources**: Stopped tasks, old images
8. **Monitor utilization**: Identify underutilized resources
9. **Use smaller images**: Faster pull, less storage
10. **VPC endpoints**: Reduce data transfer costs
11. **Reserved capacity**: For baseline workload
12. **Mix launch types**: EC2 baseline + Fargate burst

**Cost Monitoring:**
- Enable cost allocation tags
- Use Cost Explorer
- Set budget alarms

**Q19: Explain the relationship between ECS and other AWS services.**
A:
**Core Integrations:**
- **ECR**: Store and retrieve container images
- **VPC**: Network isolation and security
- **IAM**: Authentication and authorization
- **CloudWatch**: Monitoring, logging, alarms
- **ALB/NLB**: Load balancing and traffic distribution
- **Auto Scaling**: Automatic capacity management

**Enhanced Integrations:**
- **CloudFormation**: Infrastructure as Code
- **CodePipeline/CodeDeploy**: CI/CD
- **Secrets Manager/SSM**: Secrets management
- **EFS**: Shared persistent storage
- **Cloud Map**: Service discovery
- **X-Ray**: Distributed tracing
- **App Mesh**: Service mesh
- **CloudTrail**: Audit logging
- **EventBridge**: Event-driven automation

**Data Services:**
- **RDS**: Databases
- **DynamoDB**: NoSQL
- **ElastiCache**: Caching
- **S3**: Object storage
- **SQS/SNS**: Messaging

**Q20: What is the difference between ECS and EKS?**
A:
| Feature | ECS | EKS |
|---------|-----|-----|
| Orchestrator | AWS proprietary | Kubernetes |
| Learning curve | Lower | Higher |
| AWS integration | Deep, native | Good |
| Complexity | Simpler | More complex |
| Portability | AWS-specific | Multi-cloud |
| Control plane cost | Free | $0.10/hr per cluster |
| Ecosystem | AWS-focused | Huge (CNCF) |
| Configuration | Task definitions | YAML manifests |
| Use case | AWS-native apps | K8s workloads |

**Choose ECS when:**
- AWS-native applications
- Want simplicity
- Deep AWS integration needed
- Lower operational overhead
- Team unfamiliar with K8s

**Choose EKS when:**
- Need Kubernetes features
- Multi-cloud portability
- Existing K8s expertise
- Large K8s ecosystem needed
- Helm charts, operators, etc.

---

## Summary Cheat Sheet

### Key Concepts
- **Cluster**: Logical grouping of resources
- **Task Definition**: Blueprint (JSON) for containers
- **Task**: Running instance of task definition
- **Service**: Maintains desired count of tasks
- **Container Instance**: EC2 instance in cluster (EC2 launch type)

### Launch Types
- **EC2**: You manage instances, more control, cost-effective at scale
- **Fargate**: AWS manages infra, serverless, easier to use

### Network Modes
- **awsvpc**: Task gets ENI (required for Fargate)
- **bridge**: Docker bridge with port mapping
- **host**: Host network, best performance
- **none**: No connectivity

### IAM Roles
- **Task Role**: Application permissions
- **Task Execution Role**: ECS agent permissions
- **Instance Role**: EC2 instance permissions (EC2 launch type)

### Deployment Types
- **Rolling**: Gradual replacement
- **Blue/Green**: New environment, traffic shift
- **External**: Custom tooling

### Auto Scaling
- **Service Auto Scaling**: Adjust task count
- **Cluster Auto Scaling**: Adjust EC2 instances (via capacity providers)

### Interview Tips
1. Understand task definitions structure
2. Know when to use EC2 vs Fargate
3. Explain network modes and their use cases
4. Understand IAM roles (task vs execution)
5. Know auto scaling strategies
6. Explain service discovery
7. Understand deployment strategies
8. Know cost optimization techniques
9. Understand integration with ALB
10. Practice designing architectures

---

*Last Updated: 2025*
*Based on AWS Official Documentation and Best Practices*