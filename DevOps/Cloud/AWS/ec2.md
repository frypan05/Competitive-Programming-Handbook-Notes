# Amazon EC2 (Elastic Compute Cloud) - Complete Guide

## Table of Contents
1. [Introduction](#introduction)
2. [Core Concepts](#core-concepts)
3. [Instance Types](#instance-types)
4. [Storage Options](#storage-options)
5. [Networking](#networking)
6. [Security](#security)
7. [Pricing Models](#pricing-models)
8. [Auto Scaling](#auto-scaling)
9. [Load Balancing](#load-balancing)
10. [Best Practices](#best-practices)
11. [Common Interview Questions](#common-interview-questions)

---
uOEPxrdjIUSVqURKhZzGbSQMz4CH2UJT
## Introduction

**Amazon EC2** is a web service that provides secure, resizable compute capacity in the cloud. It allows you to launch virtual servers (instances) on-demand.

### Key Features
- **Elastic Computing**: Scale capacity up or down within minutes
- **Complete Control**: Root access to instances
- **Flexible Hosting**: Multiple instance types, OS, and configurations
- **Integrated**: Works seamlessly with other AWS services
- **Reliable**: 99.99% SLA availability
- **Secure**: Built on AWS Nitro System

### Why EC2?
- No upfront hardware investment
- Pay only for what you use
- Quick deployment (minutes vs weeks)
- Global infrastructure
- Broad OS support (Linux, Windows, macOS)

---

## Core Concepts

### 1. **Instance**
A virtual server running in AWS cloud. Each instance has:
- vCPUs (virtual CPUs)
- Memory (RAM)
- Storage (EBS or Instance Store)
- Network capacity
- Operating System

### 2. **AMI (Amazon Machine Image)**
A template containing:
- Operating system
- Application server
- Applications
- Configuration settings

**Types of AMIs:**
- **AWS provided**: Amazon Linux, Ubuntu, Windows Server
- **AWS Marketplace**: Pre-configured software from vendors
- **Community AMIs**: Shared by AWS users
- **Custom AMIs**: Created by you from configured instances

**AMI Components:**
- Root volume template (OS and apps)
- Launch permissions (public, private, specific AWS accounts)
- Block device mapping (volumes to attach)

### 3. **Instance Types**
Optimized for different use cases (compute, memory, storage, etc.)

### 4. **Regions and Availability Zones**
- **Region**: Geographic area (e.g., us-east-1, eu-west-1)
- **Availability Zone (AZ)**: Isolated data center within a region
- **Edge Locations**: For CloudFront CDN

**Best Practice**: Deploy across multiple AZs for high availability

### 5. **Security Groups**
Virtual firewalls controlling inbound/outbound traffic
- Stateful (return traffic automatically allowed)
- Default: All inbound denied, all outbound allowed
- Rules based on: Protocol, Port, Source/Destination

### 6. **Key Pairs**
SSH keys for secure instance access
- **Public key**: AWS stores on instance
- **Private key**: You download and store securely
- Required for SSH/RDP access

### 7. **Elastic IP (EIP)**
Static IPv4 address for dynamic cloud computing
- Remains associated with your account
- Can be remapped to different instances
- Charged when not associated with running instance

### 8. **Instance Metadata**
Data about your instance accessible from within:
```bash
# Get instance ID
curl http://169.254.169.254/latest/meta-data/instance-id

# Get public IP
curl http://169.254.169.254/latest/meta-data/public-ipv4

# Get IAM role credentials
curl http://169.254.169.254/latest/meta-data/iam/security-credentials/role-name
```

### 9. **User Data**
Scripts executed at instance launch
```bash
#!/bin/bash
yum update -y
yum install httpd -y
systemctl start httpd
systemctl enable httpd
echo "<h1>Hello from EC2</h1>" > /var/www/html/index.html
```

---

## Instance Types

EC2 instances follow naming convention: **Family.Generation.Size**
Example: `t3.medium` = T family, 3rd generation, medium size

### Instance Families

#### 1. **General Purpose (T, M, A)**
Balanced compute, memory, and networking
- **T3/T3a**: Burstable performance (credits system)
  - Use case: Web servers, dev environments
  - T3: Intel | T3a: AMD (cheaper)
- **M6i/M6a**: Consistent performance
  - Use case: Application servers, small databases
  - M6i: Intel | M6a: AMD

**Burstable Performance (T-series):**
- Baseline CPU performance
- Accumulate CPU credits when idle
- Burst above baseline using credits
- Unlimited mode available (pay for extra usage)

#### 2. **Compute Optimized (C)**
High-performance processors
- **C6i/C6a**: Latest generation
- **C7g**: AWS Graviton3 (ARM-based)
- Use cases:
  - Batch processing
  - High-performance web servers
  - Scientific modeling
  - Game servers
  - Ad serving engines

#### 3. **Memory Optimized (R, X, Z)**
Large memory for in-memory applications
- **R6i**: Memory-intensive apps
- **X2iedn**: Lowest cost per GiB (up to 64 TB RAM)
- **Z1d**: High compute + high memory + NVMe SSD
- Use cases:
  - In-memory databases (Redis, Memcached)
  - Real-time big data analytics
  - SAP HANA
  - High-performance databases

#### 4. **Storage Optimized (I, D, H)**
High sequential read/write to local storage
- **I3/I3en**: NVMe SSD, high IOPS
- **D3**: Dense HDD storage (48 TB)
- **H1**: HDD, high throughput
- Use cases:
  - NoSQL databases (Cassandra, MongoDB)
  - Data warehousing
  - Distributed file systems (HDFS)
  - Log processing

#### 5. **Accelerated Computing (P, G, F, Inf)**
Hardware accelerators (GPUs, FPGAs)
- **P4**: NVIDIA A100 GPUs (ML training)
- **G5**: NVIDIA A10G GPUs (graphics, ML inference)
- **F1**: Field Programmable Gate Arrays (FPGAs)
- **Inf1**: AWS Inferentia chips (ML inference)
- Use cases:
  - Machine learning training/inference
  - Video encoding
  - Genomics
  - Financial modeling
  - 3D rendering

#### 6. **Mac Instances**
- **mac1**: Intel-based Mac mini
- **mac2**: Apple M1 (ARM-based)
- Use cases:
  - iOS/macOS app development
  - Xcode builds
  - Apple platform testing

### Instance Sizes
Within each family: nano, micro, small, medium, large, xlarge, 2xlarge, 4xlarge, etc.
- Each size doubles resources (approximately)
- Example: t3.medium (2 vCPU, 4 GB) → t3.large (2 vCPU, 8 GB)

---

## Storage Options

### 1. **EBS (Elastic Block Store)**
Network-attached block storage that persists independently

#### EBS Volume Types:

##### **SSD-backed (for random I/O)**

**gp3 (General Purpose SSD)**
- Latest generation
- Baseline: 3,000 IOPS, 125 MB/s
- Max: 16,000 IOPS, 1,000 MB/s
- Size: 1 GB - 16 TB
- Use case: Virtual desktops, dev/test, boot volumes
- **Cost-effective**: Cheaper than gp2

**gp2 (General Purpose SSD)**
- Previous generation
- IOPS scales with size: 3 IOPS/GB
- Baseline: 100-16,000 IOPS
- Burst up to 3,000 IOPS (for volumes < 1 TB)
- Use case: General workloads

**io2/io2 Block Express (Provisioned IOPS)**
- For I/O-intensive workloads
- io2: Up to 64,000 IOPS, 1,000 MB/s
- io2 Block Express: Up to 256,000 IOPS, 4,000 MB/s
- 99.999% durability
- Use case: Large databases (MongoDB, Cassandra), critical applications

**io1 (Provisioned IOPS)**
- Previous generation
- Up to 64,000 IOPS
- 99.9% durability

##### **HDD-backed (for sequential I/O)**

**st1 (Throughput Optimized HDD)**
- Low-cost HDD
- Max: 500 IOPS, 500 MB/s throughput
- Size: 125 GB - 16 TB
- Use case: Big data, data warehouses, log processing
- Cannot be boot volume

**sc1 (Cold HDD)**
- Lowest cost HDD
- Max: 250 IOPS, 250 MB/s throughput
- Use case: Infrequently accessed data, archive
- Cannot be boot volume

#### EBS Features:

**Snapshots**
- Point-in-time backups stored in S3
- Incremental (only changed blocks)
- Can create AMI from snapshot
- Can copy across regions
- Can share with other AWS accounts

**Encryption**
- AES-256 encryption
- Encrypted at rest and in transit
- Minimal performance impact
- Uses AWS KMS

**Multi-Attach (io2 only)**
- Attach single volume to multiple instances
- Same AZ only
- Use case: Clustered applications

**Performance**
- IOPS: Input/Output Operations Per Second
- Throughput: MB/s data transfer
- Latency: Single-digit milliseconds

### 2. **Instance Store**
Physical disks attached to host computer
- **Ephemeral**: Data lost on instance stop/terminate
- **Very high IOPS**: Millions of IOPS possible
- **No additional cost**: Included with instance
- Use case: Temporary data, buffers, caches, scratch data

**Difference: EBS vs Instance Store**
| Feature | EBS | Instance Store |
|---------|-----|----------------|
| Persistence | Yes | No (ephemeral) |
| Detachable | Yes | No |
| Snapshots | Yes | No |
| Performance | High | Very High |
| Use case | Persistent data | Temporary data |

### 3. **EFS (Elastic File System)**
Managed NFS (Network File System)
- **Shared storage**: Multiple instances can mount
- **Elastic**: Grows/shrinks automatically
- **Highly available**: Multi-AZ by default
- Use case: Content management, web serving, shared data
- More expensive than EBS

### 4. **FSx**
Managed file systems
- **FSx for Windows**: Windows native file system (SMB)
- **FSx for Lustre**: High-performance computing (HPC)

---

## Networking

### 1. **VPC (Virtual Private Cloud)**
Isolated virtual network in AWS
- Define IP address range (CIDR block)
- Create subnets (public/private)
- Configure route tables
- Set up internet/NAT gateways

### 2. **Subnets**
- **Public Subnet**: Has route to Internet Gateway
- **Private Subnet**: No direct internet access
- Each subnet in one AZ

### 3. **ENI (Elastic Network Interface)**
Virtual network card
- Can attach/detach from instances
- Can have multiple ENIs per instance
- Private IP, Elastic IP, MAC address, Security Groups
- Use case: Network failover, dual-homed instances

### 4. **Enhanced Networking**
- **SR-IOV**: Single Root I/O Virtualization
- Higher bandwidth, lower latency
- Higher packets per second (PPS)
- Two types:
  - **ENA (Elastic Network Adapter)**: Up to 100 Gbps
  - **Intel 82599 VF**: Up to 10 Gbps (older)

### 5. **Placement Groups**

**Cluster Placement Group**
- Instances packed close together in single AZ
- Low latency (10 Gbps network)
- Use case: HPC applications
- Risk: Single point of failure

**Spread Placement Group**
- Instances on different hardware
- Across multiple AZs
- Max 7 instances per AZ per group
- Use case: Critical applications requiring high availability

**Partition Placement Group**
- Instances divided into partitions
- Each partition on separate racks
- Up to 7 partitions per AZ
- Use case: Large distributed systems (Hadoop, Cassandra, Kafka)

### 6. **IP Addressing**

**Private IP**
- Internal AWS communication
- Retained when instance stopped

**Public IP**
- Internet communication
- Changes when instance stopped (unless Elastic IP)

**Elastic IP**
- Static public IPv4
- Charged when not associated with running instance

**IPv6**
- Available for internet-facing instances
- All IPv6 addresses are public

---

## Security

### 1. **Security Groups**
Virtual firewalls at instance level

**Characteristics:**
- **Stateful**: Return traffic automatically allowed
- **Allow rules only**: Cannot create deny rules
- **Multiple security groups**: Can attach up to 5 per instance
- **All rules evaluated**: Most permissive rule wins

**Common Rules:**
```
SSH (22) from 0.0.0.0/0 (anywhere)
HTTP (80) from 0.0.0.0/0
HTTPS (443) from 0.0.0.0/0
RDP (3389) from your IP
Custom TCP from another security group
```

**Best Practices:**
- Least privilege principle
- Separate security groups for different tiers
- Reference security groups instead of IP ranges
- Regular audit of rules

### 2. **Network ACLs (NACLs)**
Firewall at subnet level

**Characteristics:**
- **Stateless**: Must allow both inbound and outbound
- **Numbered rules**: Evaluated in order (lowest first)
- **Allow and Deny rules**: Can explicitly deny
- **Default**: Allows all inbound/outbound

**Security Groups vs NACLs:**
| Feature | Security Groups | NACLs |
|---------|----------------|-------|
| Level | Instance | Subnet |
| State | Stateful | Stateless |
| Rules | Allow only | Allow and Deny |
| Rule processing | All rules | In order |
| Applies to | Specified instances | All instances in subnet |

### 3. **IAM Roles**
Grant permissions to EC2 instances

**Benefits:**
- No credentials in code
- Automatic credential rotation
- Easy permission management

**Common Use Cases:**
- Access S3 buckets
- Query DynamoDB
- Publish to SNS/SQS
- Read/write CloudWatch logs

**Example:**
```json
{
  "Version": "2012-10-17",
  "Statement": [{
    "Effect": "Allow",
    "Action": [
      "s3:GetObject",
      "s3:PutObject"
    ],
    "Resource": "arn:aws:s3:::my-bucket/*"
  }]
}
```

### 4. **Key Pairs**
- Public key cryptography (RSA)
- AWS stores public key
- You store private key
- Never share private key
- Can create multiple key pairs

### 5. **AWS Systems Manager (SSM)**
- Manage instances without SSH keys
- Session Manager: Browser-based shell
- Requires IAM role with SSM permissions
- All sessions logged to CloudWatch/S3

### 6. **Security Best Practices**
- Enable CloudTrail logging
- Use IAM roles instead of access keys
- Encrypt EBS volumes
- Regular patching and updates
- Implement least privilege
- Use bastion hosts for private instances
- Enable VPC Flow Logs
- Regular security audits

---

## Pricing Models

### 1. **On-Demand**
Pay by the second (minimum 60 seconds)

**Characteristics:**
- No upfront payment
- No long-term commitment
- Pay fixed rate
- Can stop/start anytime

**Use case:**
- Unpredictable workloads
- Short-term projects
- Testing/development
- First-time applications

**Pricing:** Highest cost per hour

### 2. **Reserved Instances (RI)**
1 or 3-year commitment for significant discount (up to 72%)

**Types:**
- **Standard RI**: Highest discount, can't change instance type
- **Convertible RI**: Lower discount, can change instance type
- **Scheduled RI**: Reserve for specific time windows

**Payment Options:**
- **All Upfront**: Highest discount
- **Partial Upfront**: Medium discount
- **No Upfront**: Lowest discount

**Scope:**
- **Regional**: Flexible AZ within region
- **Zonal**: Specific AZ, capacity reservation

**Use case:**
- Steady-state workloads
- Predictable usage
- Applications requiring reserved capacity

### 3. **Savings Plans**
Flexible pricing model (up to 72% discount)

**Types:**
- **Compute Savings Plans**: Most flexible (EC2, Lambda, Fargate)
- **EC2 Instance Savings Plans**: Specific instance family in region

**Benefits:**
- Flexibility to change instance size, OS, tenancy
- Applies automatically to usage
- 1 or 3-year commitment

### 4. **Spot Instances**
Bid for unused EC2 capacity (up to 90% discount)

**Characteristics:**
- Can be interrupted with 2-minute warning
- Price fluctuates based on supply/demand
- Best for flexible start/end times

**Spot Request Types:**
- **One-time**: Request once, terminated when interrupted
- **Persistent**: Request relaunched after interruption

**Spot Block**: Reserve 1-6 hours without interruption

**Spot Fleet**: Collection of spot and on-demand instances
- Define target capacity
- Multiple instance types
- Allocation strategy: lowestPrice, diversified, capacityOptimized

**Use case:**
- Batch processing
- Data analysis
- CI/CD pipelines
- Stateless web servers
- Image/video processing
- Non-critical workloads

**Cannot use for:**
- Databases
- Critical jobs
- Services requiring high availability

### 5. **Dedicated Hosts**
Physical server dedicated to you

**Characteristics:**
- Visibility into sockets, cores
- Can use existing server-bound licenses
- Most expensive option

**Use case:**
- Regulatory requirements
- Licensing restrictions (per-socket, per-core)

### 6. **Dedicated Instances**
Instances run on dedicated hardware
- May share hardware with other instances in your account
- No visibility into physical server
- More expensive than regular instances

**Dedicated Hosts vs Dedicated Instances:**
| Feature | Dedicated Hosts | Dedicated Instances |
|---------|-----------------|---------------------|
| Visibility | Socket/core level | No visibility |
| Placement control | Yes | No |
| License | BYOL supported | Limited |
| Billing | Per host | Per instance |
| Price | Most expensive | Expensive |

### 7. **Capacity Reservations**
Reserve capacity in specific AZ
- No discount (pay on-demand rate)
- Can combine with RIs/Savings Plans for discount
- Use case: Disaster recovery, capacity assurance

---

## Auto Scaling

### What is Auto Scaling?
Automatically adjust compute capacity based on demand
- Scale out (add instances) during demand spikes
- Scale in (remove instances) during low demand
- Ensures right number of instances available

### Components

#### 1. **Launch Template/Configuration**
Blueprint for instances
- AMI
- Instance type
- Key pair
- Security groups
- User data script
- IAM role
- EBS volumes

**Launch Template vs Launch Configuration:**
- Launch Template: Newer, versioned, more features
- Launch Configuration: Legacy, immutable

#### 2. **Auto Scaling Group (ASG)**
Collection of EC2 instances

**Settings:**
- **Minimum capacity**: Never go below
- **Desired capacity**: Target number
- **Maximum capacity**: Never exceed

**Example:**
- Min: 2, Desired: 4, Max: 10
- Normal: 4 instances
- High demand: Scale to 10
- Low demand: Scale to 2

#### 3. **Scaling Policies**

**Types:**

**a) Target Tracking**
Maintain specific metric value
- Example: Keep CPU at 50%
- Easiest to set up

```
Target metric: Average CPU Utilization
Target value: 50%
```

**b) Step Scaling**
Scale based on CloudWatch alarms
- Add/remove specific number of instances
- Different steps for different thresholds

```
CPU > 80%: Add 2 instances
CPU > 90%: Add 4 instances
CPU < 30%: Remove 1 instance
```

**c) Simple Scaling**
Single scaling adjustment
- After scaling, wait for cooldown period

**d) Scheduled Scaling**
Predictable traffic patterns
```
Every Monday 9 AM: Scale to 20 instances
Every Friday 6 PM: Scale to 5 instances
```

**e) Predictive Scaling**
Uses ML to predict future traffic
- Proactively scales before traffic spike
- Based on historical patterns

#### 4. **Scaling Cooldown**
Default: 300 seconds (5 minutes)
- Prevents rapid scaling actions
- Wait for previous scaling to take effect

### Health Checks
- **EC2 Status Check**: Instance running
- **ELB Health Check**: Application responding
- Unhealthy instances terminated and replaced

### Lifecycle Hooks
Perform actions before launch/terminate
- Install software
- Backup data
- Log events
- States: Pending, InService, Terminating, Terminated

### Termination Policies
Which instance to terminate first?
- Default: Oldest launch template, then closest to billing hour
- Options: OldestInstance, NewestInstance, OldestLaunchConfiguration

### Best Practices
- Use multiple AZs for high availability
- Enable detailed monitoring (1-min intervals)
- Use ELB health checks for applications
- Set appropriate cooldown periods
- Monitor scaling activities
- Test scaling policies

---

## Load Balancing

### What is Load Balancing?
Distribute incoming traffic across multiple targets

### Types of Load Balancers

#### 1. **Application Load Balancer (ALB)**
Layer 7 (HTTP/HTTPS) load balancing

**Features:**
- **Path-based routing**: /api → API servers, /images → Image servers
- **Host-based routing**: api.example.com → API, www.example.com → Web
- **HTTP/2 and WebSocket support**
- **Server Name Indication (SNI)**: Multiple SSL certificates
- **Query string routing**: ?platform=mobile
- **Fixed response**: Return static content
- **Redirect**: HTTP to HTTPS

**Target Types:**
- EC2 instances
- IP addresses
- Lambda functions
- Containers (ECS)

**Use case:**
- Microservices
- Container-based applications
- Web applications with complex routing

#### 2. **Network Load Balancer (NLB)**
Layer 4 (TCP/UDP) load balancing

**Features:**
- **Ultra-high performance**: Millions of requests/sec
- **Low latency**: ~100 microseconds
- **Static IP**: One per AZ
- **Elastic IP support**
- **Preserve source IP**
- **TLS termination**
- **TCP/UDP/TLS protocols**

**Use case:**
- Extreme performance required
- Static IP needed
- TCP/UDP traffic
- Gaming, IoT

#### 3. **Gateway Load Balancer (GWLB)**
Layer 3 (Network layer) load balancing

**Features:**
- Deploy third-party virtual appliances
- Transparent network gateway
- GENEVE protocol (port 6081)

**Use case:**
- Firewalls
- Intrusion detection/prevention
- Deep packet inspection

#### 4. **Classic Load Balancer (CLB)**
Legacy (Layer 4 and Layer 7)
- Not recommended for new applications
- Use ALB or NLB instead

### Key Concepts

#### Cross-Zone Load Balancing
- **Enabled**: Distribute evenly across all targets in all AZs
- **Disabled**: Distribute evenly only within same AZ
- ALB: Enabled by default (no charge)
- NLB: Disabled by default (charged if enabled)

#### Connection Draining (Deregistration Delay)
Time to complete in-flight requests before deregistering
- Default: 300 seconds
- Range: 0-3600 seconds

#### Sticky Sessions (Session Affinity)
Route same client to same target
- **Duration-based**: Cookie expires after duration
- **Application-based**: Application generates cookie
- Use case: User session data stored locally

#### SSL/TLS
- **SSL Certificate**: Upload to ACM or IAM
- **SNI**: Multiple certificates, different domains
- **SSL Policies**: Security policies for cipher suites

#### Health Checks
- Protocol: HTTP, HTTPS, TCP
- Port: Target port or override
- Path: /health, /ping
- Interval: 30 seconds (default)
- Timeout: 5 seconds
- Healthy threshold: 5 consecutive successes
- Unhealthy threshold: 2 consecutive failures

### ALB vs NLB

| Feature | ALB | NLB |
|---------|-----|-----|
| Layer | 7 (HTTP/HTTPS) | 4 (TCP/UDP) |
| Performance | High | Ultra-high |
| Latency | ~ms | ~100 μs |
| Static IP | No | Yes |
| Elastic IP | No | Yes |
| Routing | Advanced | Simple |
| WebSocket | Yes | Yes |
| Preserve source IP | Via X-Forwarded-For | Yes |
| Use case | Web apps | Extreme performance |

---

## Best Practices

### 1. **Security**
- Use IAM roles, never embed credentials
- Enable encryption for EBS volumes
- Use security groups with least privilege
- Regular patching and updates
- Enable CloudTrail and VPC Flow Logs
- Use bastion hosts for private instances
- Implement defense in depth

### 2. **High Availability**
- Deploy across multiple AZs
- Use Auto Scaling Groups
- Use Load Balancers
- Regular backups (AMIs, EBS snapshots)
- Test disaster recovery procedures

### 3. **Performance**
- Choose right instance type for workload
- Use placement groups for HPC
- Enable enhanced networking
- Use SSD volumes for I/O intensive apps
- Monitor with CloudWatch
- Use CloudFront for content delivery

### 4. **Cost Optimization**
- Use Reserved Instances for steady workloads
- Use Spot Instances for flexible workloads
- Right-size instances (don't over-provision)
- Stop non-production instances off-hours
- Delete unused EBS volumes
- Release unused Elastic IPs
- Use Auto Scaling to match demand
- Monitor with Cost Explorer

### 5. **Monitoring**
- Enable detailed monitoring
- Create CloudWatch alarms
- Use CloudWatch Logs for application logs
- Set up SNS notifications
- Monitor key metrics:
  - CPU Utilization
  - Network In/Out
  - Disk Read/Write
  - Status Checks

### 6. **Backup and Recovery**
- Regular EBS snapshots
- Create AMIs of configured instances
- Cross-region snapshot copies
- Automate backups with AWS Backup
- Document recovery procedures
- Test restore processes

### 7. **Tagging**
- Consistent tagging strategy
- Tags: Environment, Owner, Project, Cost Center
- Use tags for cost allocation
- Use tags for automation

---

## Common Interview Questions

### Basic Level

**Q1: What is EC2?**
A: Amazon EC2 is a web service that provides resizable compute capacity (virtual servers) in the cloud. It allows you to launch instances on-demand with complete control over the operating system and configuration.

**Q2: What is an AMI?**
A: Amazon Machine Image is a template containing the OS, application server, and applications needed to launch an instance. It's a snapshot that can be used to launch one or multiple instances.

**Q3: What is the difference between stopping and terminating an instance?**
A: 
- **Stop**: Instance is shut down, EBS volumes retained, can restart later, no compute charges (only storage)
- **Terminate**: Instance deleted permanently, root EBS volume deleted (unless configured otherwise), cannot restart

**Q4: What are Security Groups?**
A: Virtual firewalls controlling inbound and outbound traffic at the instance level. They are stateful, meaning return traffic is automatically allowed. Only allow rules can be specified.

**Q5: What is the difference between public, private, and Elastic IP?**
A:
- **Private IP**: Internal AWS communication, retained when stopped
- **Public IP**: Internet communication, changes when stopped
- **Elastic IP**: Static public IP, retained until released, charged when not associated

### Intermediate Level

**Q6: Explain EBS volume types and when to use each.**
A:
- **gp3/gp2**: General purpose, boot volumes, dev/test (gp3 is newer and cheaper)
- **io2/io1**: High IOPS, databases, critical applications
- **st1**: Throughput optimized, big data, data warehouses
- **sc1**: Cold storage, infrequent access, archives

**Q7: What is the difference between EBS and Instance Store?**
A:
- **EBS**: Network-attached, persistent, can detach/reattach, snapshot support
- **Instance Store**: Physically attached, ephemeral (data lost on stop), very high IOPS, no additional cost

**Q8: Explain Auto Scaling components.**
A:
- **Launch Template**: Blueprint for instances (AMI, type, security groups, etc.)
- **Auto Scaling Group**: Manages instance collection with min/desired/max capacity
- **Scaling Policies**: Rules for when to scale (target tracking, step, scheduled)

**Q9: What are Placement Groups?**
A:
- **Cluster**: Low latency, high network throughput, single AZ
- **Spread**: Different hardware, high availability, max 7 per AZ
- **Partition**: Large distributed systems, separate racks

**Q10: Difference between Security Groups and NACLs?**
A:
- **Security Groups**: Instance level, stateful, allow rules only, all rules evaluated
- **NACLs**: Subnet level, stateless, allow and deny rules, rules processed in order

### Advanced Level

**Q11: How does burstable performance work in T-series instances?**
A: T-series instances earn CPU credits when operating below baseline. Credits can be spent to burst above baseline when needed. If credits run out, performance drops to baseline. Unlimited mode available to burst beyond credits (with additional charges).

**Q12: Explain the differences between Reserved Instances and Savings Plans.**
A:
- **Reserved Instances**: Specific instance type in specific region/AZ, less flexible
- **Savings Plans**: More flexible, can change instance size/OS/tenancy, applies to compute usage (EC2, Lambda, Fargate)
Both offer similar discounts (up to 72%) for 1 or 3-year commitment.

**Q13: How do Spot Instances work and what are the best practices?**
A: Spot Instances let you bid for unused EC2 capacity at up to 90% discount. They can be interrupted with 2-minute warning when AWS needs capacity back. Best practices:
- Use Spot Fleet with diversified strategy
- Implement graceful shutdown handling
- Use for stateless, flexible workloads
- Combine with On-Demand for baseline capacity
- Monitor Spot interruption notices via metadata

**Q14: Explain Cross-Zone Load Balancing.**
A: When enabled, load balancer distributes traffic evenly across all registered targets in all enabled AZs, regardless of which AZ receives the traffic. When disabled, traffic is distributed only among targets in the same AZ as the load balancer node. ALB has it enabled by default (free), NLB has it disabled (charged if enabled).

**Q15: How would you design a highly available and scalable web application on EC2?**
A: Architecture:
1. **Multi-AZ deployment**: Instances in at least 2 AZs
2. **Application Load Balancer**: Distribute traffic across instances
3. **Auto Scaling Group**: Automatically adjust capacity based on demand
4. **RDS Multi-AZ**: Database high availability
5. **ElastiCache**: Session storage and caching
6. **CloudFront**: CDN for static content
7. **S3**: Store static assets
8. **Route 53**: DNS with health checks and failover
9. **Security**: Security groups, NACLs, WAF
10. **Monitoring**: CloudWatch, alarms, dashboards

**Q16: What is Enhanced Networking and when should you use it?**
A: Enhanced Networking uses SR-IOV (Single Root I/O Virtualization) to provide:
- Higher bandwidth (up to 100 Gbps with ENA)
- Higher PPS (packets per second)
- Lower inter-instance latency
Use for: HPC applications, distributed computing, high-performance databases. Most modern instance types support it by default.

**Q17: Explain the EC2 instance launch process.**
A: 
1. Choose AMI (OS and software)
2. Select instance type (CPU, memory, network)
3. Configure instance (VPC, subnet, IAM role, user data)
4. Add storage (EBS volumes, instance store)
5. Add tags (key-value pairs for organization)
6. Configure security group (firewall rules)
7. Select key pair (SSH access)
8. Launch and review

Behind the scenes:
- AWS provisions resources on physical host
- AMI is copied to instance
- Networking is configured
- User data scripts execute
- Instance enters running state

**Q18: How do you optimize EC2 costs?**
A:
1. **Right-sizing**: Match instance type to workload
2. **Reserved Instances/Savings Plans**: For predictable workloads (up to 72% discount)
3. **Spot Instances**: For flexible workloads (up to 90% discount)
4. **Auto Scaling**: Match capacity to demand
5. **Stop non-production**: Off-hours shutdown
6. **Use newer generations**: Better price/performance
7. **Delete unused resources**: EBS volumes, snapshots, Elastic IPs
8. **Use CloudWatch**: Monitor utilization
9. **Consider Graviton instances**: ARM-based, up to 40% better price/performance

**Q19: Explain EC2 instance metadata and user data.**
A:
- **Instance Metadata**: Information about instance accessible via HTTP endpoint (169.254.169.254)
  - Instance ID, type, IP, security groups, IAM role credentials
  - Retrieved from within instance
  - No authentication required
- **User Data**: Scripts passed to instance at launch
  - Runs only at first boot (unless configured otherwise)
  - Used for automation (install software, configure settings)
  - Max 16 KB
  - Runs as root

**Q20: What strategies would you use for disaster recovery on EC2?**
A:
1. **Backup and Restore**: Regular EBS snapshots, AMIs, cross-region copies
2. **Pilot Light**: Minimal version running, scale up when needed
3. **Warm Standby**: Scaled-down version running, scale up for production
4. **Multi-Site Active/Active**: Full production capacity in multiple regions

Considerations:
- RTO (Recovery Time Objective): How quickly to recover
- RPO (Recovery Point Objective): How much data loss acceptable
- Cost vs. availability trade-off
- Automate with AWS Backup, Lambda, CloudFormation
- Regular testing of DR procedures

---

## Additional Resources

### AWS Documentation
- [EC2 User Guide](https://docs.aws.amazon.com/ec2/)
- [EC2 FAQs](https://aws.amazon.com/ec2/faqs/)
- [Instance Types](https://aws.amazon.com/ec2/instance-types/)

### AWS Services to Know
- **CloudWatch**: Monitoring and observability
- **Systems Manager**: Instance management without SSH
- **CloudFormation**: Infrastructure as Code
- **AWS Backup**: Centralized backup
- **Cost Explorer**: Cost analysis and optimization

### Key Metrics to Monitor
- CPUUtilization
- NetworkIn/NetworkOut
- DiskReadOps/DiskWriteOps
- StatusCheckFailed
- StatusCheckFailed_Instance
- StatusCheckFailed_System

### Common CLI Commands
```bash
# Launch instance
aws ec2 run-instances --image-id ami-xxx --instance-type t3.micro --key-name my-key

# Describe instances
aws ec2 describe-instances

# Stop instance
aws ec2 stop-instances --instance-ids i-xxx

# Start instance
aws ec2 start-instances --instance-ids i-xxx

# Terminate instance
aws ec2 terminate-instances --instance-ids i-xxx

# Create snapshot
aws ec2 create-snapshot --volume-id vol-xxx --description "My backup"

# Create AMI
aws ec2 create-image --instance-id i-xxx --name "My AMI"
```

---

## Summary Cheat Sheet

### Instance Types (Remember: FCGMPRIX)
- **F**: FPGA
- **C**: Compute Optimized
- **G**: Graphics (GPU)
- **M**: Main Choice (General Purpose)
- **P**: GPU (ML/AI)
- **R**: RAM (Memory Optimized)
- **I**: I/O (Storage Optimized)
- **X**: Xtreme Memory

### EBS Volume Types (Remember: GGIISC)
- **gp3/gp2**: General Purpose SSD
- **io2/io1**: Provisioned IOPS SSD
- **st1**: Throughput Optimized HDD
- **sc1**: Cold HDD

### Pricing Models (Remember: RODS)
- **R**eserved: 1-3 year commitment, up to 72% discount
- **O**n-Demand: Pay per second, most flexible
- **D**edicated: Physical server for you
- **S**pot: Unused capacity, up to 90% discount

### Key Points for Interviews
1. EC2 = Virtual servers in the cloud
2. Security Groups = Stateful, allow rules only
3. EBS = Persistent block storage, network-attached
4. Auto Scaling = Automatic capacity adjustment
5. Load Balancers = Distribute traffic (ALB=Layer 7, NLB=Layer 4)
6. Spot Instances = Cheap but can be interrupted
7. Multi-AZ = High availability
8. IAM Roles = Secure access without credentials
9. User Data = Automation scripts at launch
10. Always use latest generation instances for best price/performance

---

*Last Updated: 2025*
*Based on AWS Official Documentation and Best Practices*
