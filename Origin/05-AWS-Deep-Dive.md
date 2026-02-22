# 05 — AWS Deep Dive

---

## SECTION 1 — SQS (Simple Queue Service) — CRITICAL (was asked last time)

### What is SQS?

Amazon SQS is a fully managed message queuing service that decouples components of a distributed
application. Producers send messages to a queue; consumers poll and process them asynchronously.
Messages persist in the queue until explicitly deleted by the consumer.

---

### Queue Types

**Standard Queue:**
- Near-unlimited throughput (virtually unlimited TPS)
- At-least-once delivery — a message MAY be delivered more than once. Always design consumers
  to be idempotent (processing the same message twice produces the same result).
- Best-effort ordering — messages are generally delivered in order but NOT guaranteed
- Use when: throughput matters more than strict ordering

**FIFO Queue (First-In-First-Out):**
- Exactly-once processing — no duplicates (uses deduplication ID)
- Strict ordering guaranteed within a message group (MessageGroupId)
- 3000 TPS with batching, 300 TPS without
- Use when: order matters — financial transactions, order processing, event sequencing

---

### Key SQS Concepts

**Visibility Timeout:**
When a consumer receives a message, it becomes invisible to all other consumers for the
visibility timeout duration (default: 30s, max: 12h). If the consumer does not DELETE the
message before the timeout expires, the message becomes visible again and another consumer
can pick it up (this is how retries happen automatically).
Set visibility timeout to slightly longer than your maximum processing time.

**Dead Letter Queue (DLQ):**
A separate queue where messages land after failing `maxReceiveCount` delivery attempts.
- Use for: debugging poison-pill messages, alerting on persistent failures, forensics
- Always set a DLQ on every production queue
- Monitor DLQ depth as a critical alarm

**Long Polling vs Short Polling:**
```
Short polling — returns immediately even if queue is empty.
                Wastes API calls. Costs more. Never use in production.

Long polling  — waits up to WaitTimeSeconds (max 20) for a message to arrive.
                Fewer empty responses. Lower cost. Better latency.
                Always use: WaitTimeSeconds=20
```

**Batch Operations:**
- Send up to 10 messages per API call (SendMessageBatch)
- Receive up to 10 messages per API call (ReceiveMessage MaxNumberOfMessages=10)
- Delete up to 10 messages per API call (DeleteMessageBatch)
- Dramatically reduces API call count and cost

**Message Retention:** Default 4 days. Configurable: 1 minute to 14 days.

**Message Size:** Max 256 KB. For larger payloads: store content in S3, put the S3 key in the
SQS message (the "claim-check" pattern).

---

### SQS vs Lambda — the key distinction (was asked last time, know this cold)

```
SQS is a MESSAGE QUEUE (storage + buffering).
Lambda is a COMPUTE SERVICE (runs your code).
These are two completely different things that work TOGETHER.

SQS alone:
  - Stores messages durably until a consumer processes and deletes them.
  - The consumer can be ANYTHING: EC2 instance, ECS task, Lambda, on-prem server.
  - YOU manage consumer scaling and polling logic.
  - Pull-based: consumers poll the queue continuously.
  - Messages persist if the consumer is down — no messages lost.

Lambda alone:
  - Runs a function in response to an event or trigger.
  - Stateless, auto-scaling compute.
  - Billed per invocation + duration.
  - Push-based when triggered by API Gateway, SNS, EventBridge.

SQS + Lambda TOGETHER (the classic serverless pattern):
  1. Producer puts messages into SQS (decoupled from the consumer).
  2. Lambda event source mapping polls SQS on your behalf (you don't write polling code).
  3. Lambda is invoked with a batch of messages (configurable batchSize).
  4. If Lambda function succeeds -> messages are automatically deleted from the queue.
  5. If Lambda function throws -> messages return to queue after visibility timeout.
  6. After maxReceiveCount failures -> messages go to DLQ.

Why put SQS in FRONT of Lambda instead of invoking Lambda directly?
  - SQS absorbs traffic spikes: Lambda has concurrency limits (default 1000/region).
    SQS buffers the excess and Lambda drains at its own pace.
  - Guaranteed delivery: messages persist in SQS even if Lambda is throttled or errors.
  - Built-in retry logic via visibility timeout.
  - DLQ for unprocessable messages — nothing is silently lost.
  - Decoupling: the producer doesn't care if Lambda is slow, erroring, or being updated.
  - Cost: SQS is very cheap; you pay Lambda only when messages are actually processed.
```

---

### Implementing SQS with boto3 (Python)

```python
import boto3
import json
import logging

logger = logging.getLogger(__name__)
sqs = boto3.client('sqs', region_name='us-east-1')
QUEUE_URL = 'https://sqs.us-east-1.amazonaws.com/123456789012/my-queue'

# Send a single message
def send_message(body: dict) -> str:
    response = sqs.send_message(
        QueueUrl=QUEUE_URL,
        MessageBody=json.dumps(body)
    )
    return response['MessageId']

# Send a batch (up to 10 messages)
def send_batch(messages: list[dict]):
    entries = [
        {'Id': str(i), 'MessageBody': json.dumps(msg)}
        for i, msg in enumerate(messages)
    ]
    response = sqs.send_message_batch(QueueUrl=QUEUE_URL, Entries=entries)
    if response.get('Failed'):
        logger.error(f"Failed to send: {response['Failed']}")

# Receive messages (always use long polling)
def receive_messages(max_count: int = 10) -> list:
    response = sqs.receive_message(
        QueueUrl=QUEUE_URL,
        MaxNumberOfMessages=max_count,
        WaitTimeSeconds=20,        # LONG POLLING — always do this
        VisibilityTimeout=60,      # set >= your max processing time
        MessageAttributeNames=['All']
    )
    return response.get('Messages', [])

# Delete after successful processing
def delete_message(receipt_handle: str):
    sqs.delete_message(QueueUrl=QUEUE_URL, ReceiptHandle=receipt_handle)

# Full worker loop
def run_worker():
    logger.info("SQS worker started...")
    while True:
        messages = receive_messages()
        for msg in messages:
            try:
                data = json.loads(msg['Body'])
                process(data)
                delete_message(msg['ReceiptHandle'])   # DELETE only on success
            except Exception as e:
                logger.error(f"Processing failed: {e}")
                # DO NOT delete — message reappears after VisibilityTimeout
                # After maxReceiveCount attempts it moves to the DLQ automatically
```

---

## SECTION 2 — Lambda (Serverless Compute)

**Q: What is AWS Lambda? Key limits?**

A: Lambda is a serverless compute service. You provide code (or a container image); AWS manages
the underlying infrastructure, OS patching, scaling, and availability.
Billed per invocation + duration (1ms granularity). Zero cost when idle.

```
Key limits:
  Max execution timeout:  15 minutes
  Memory:                 128 MB to 10 GB (CPU scales linearly with memory)
  Package size:           50 MB zipped, 250 MB unzipped
  Container image:        up to 10 GB
  Concurrency:            1000 per account per region (soft limit, can be raised)
  /tmp storage:           512 MB to 10 GB (ephemeral, not shared between invocations)
  Environment variables:  4 KB total
  Payload (sync):         6 MB request, 6 MB response
  Payload (async):        256 KB
```

---

**Q: What is a cold start and how do you mitigate it?**

A: The first invocation (or after a period of inactivity) requires Lambda to:
1. Provision a new micro-VM (Firecracker)
2. Download and unzip the deployment package
3. Start the language runtime (JVM, Python interpreter, Node.js)
4. Run your initialization code (global scope: imports, DB connections, model loading)

Cold start duration varies by runtime: Go/Python/Node ~100ms, Java/.NET ~1-5 seconds.

Mitigation strategies:
```
1. Provisioned Concurrency  — pre-warm N execution environments (costs money even when idle)
2. Keep-warm pings          — EventBridge rule every 5 min to trigger the function
3. Use efficient runtimes   — Go, Python, Node over Java/C# for latency-sensitive functions
4. Minimise package size    — only import what you need; use layers for shared dependencies
5. Initialize outside handler — DB connections, loaded models in global scope
   (they persist across warm invocations in the same execution environment)
```

---

**Q: Lambda execution model — warm vs cold invocations**

```python
import boto3
import psycopg2

# GLOBAL SCOPE: runs once on cold start, reused on warm invocations
# This is a major optimization — DB connection is reused across invocations
db_conn = psycopg2.connect(
    host=os.environ['DB_HOST'],
    database=os.environ['DB_NAME'],
    user=os.environ['DB_USER'],
    password=os.environ['DB_PASSWORD']
)

def handler(event, context):
    # This runs on EVERY invocation
    # db_conn is already established if this is a warm invocation
    cursor = db_conn.cursor()
    cursor.execute("SELECT COUNT(*) FROM orders")
    count = cursor.fetchone()[0]
    return {'statusCode': 200, 'body': str(count)}
```

---

**Q: Lambda vs EC2 vs ECS/Fargate — when to use each?**

```
Lambda:
  - Short-lived tasks (under 15 minutes)
  - Event-driven, sporadic, unpredictable workloads
  - Zero idle cost
  - No need for persistent connections (or use RDS Proxy)
  - Examples: image resize on S3 upload, API handler, nightly data cleanup,
              processing SQS messages, sending emails on a trigger

EC2:
  - Long-running processes (daemon services)
  - Full OS control, custom kernel, specific hardware (GPU, FPGA)
  - Predictable, sustained load — Reserved Instances are very cheap at scale
  - Workloads that need persistent local state or large /tmp
  - Examples: ML training, video transcoding, legacy applications, game servers

ECS/Fargate:
  - Containerised long-running services
  - More control than Lambda, less operational overhead than EC2
  - Fargate: serverless containers (no EC2 instances to manage or patch)
  - Examples: microservices, REST APIs, background workers, web apps
```

---

**Q: What triggers Lambda?**

```
Synchronous (waits for response):
  - API Gateway, ALB, Lambda Function URL
  - Cognito, CloudFront (Lambda@Edge)

Asynchronous (fire and forget):
  - S3 (object events), SNS, EventBridge, CloudWatch Events
  - Lambda retries async failures 2 times by default before sending to DLQ/destination

Stream/Queue (event source mapping — Lambda polls):
  - SQS, Kinesis Data Streams, DynamoDB Streams, Kafka (MSK)
  - Lambda manages the polling, batching, and retries
```

---

## SECTION 3 — EC2

**Q: EC2 instance families — what does each letter mean?**

```
t  — Burstable (T3, T3a, T4g). Uses CPU credits. Cheap for dev/test, variable workloads.
m  — General purpose (M5, M6i, M7g). Balanced CPU:memory. Good for app servers, databases.
c  — Compute optimised (C5, C6g, C7g). High CPU:memory ratio. Good for HPC, batch, gaming.
r  — Memory optimised (R5, R6g). Large RAM:CPU ratio. Good for in-memory DBs, analytics.
i  — Storage optimised (I3, I4i). High local NVMe IOPS. Good for NoSQL, data warehousing.
d  — Dense storage (D3). Large HDD capacity. Hadoop, distributed file systems.
g  — GPU (G4ad, G5). ML inference, video encoding, 3D rendering.
p  — High-performance GPU (P3, P4). ML training, HPC, CUDA.
a  — ARM-based Graviton (A1). Cost-optimised for scale-out workloads.
x  — Extra-large memory (X1e). In-memory databases (SAP HANA), analytics.
```

---

**Q: On-Demand vs Reserved vs Spot — when to use each?**

```
On-Demand:
  - Pay per second/hour. No commitment. Highest unit cost.
  - Use for: unpredictable workloads, short-term experiments, new projects.

Reserved Instances (1 or 3 year commitment):
  - Up to 72% discount over On-Demand.
  - Standard RI: locked to instance family/size/region. Maximum discount.
  - Convertible RI: can change attributes. Less discount.
  - Use for: steady-state production databases, always-on services.

Spot Instances:
  - Up to 90% discount. AWS can interrupt with 2-minute warning.
  - Instances can be terminated when AWS needs capacity back.
  - Use for: batch jobs, ML training, CI/CD build agents, fault-tolerant stateless processing.
  - NOT for: databases, anything that cannot tolerate interruption.

Savings Plans:
  - Commit to a $/hour spend (not a specific instance type).
  - Compute Savings Plans: most flexible (any instance family, region, OS).
  - More flexible than Reserved Instances with similar discounts.
```

---

**Q: What is an EC2 placement group?**

```
Cluster placement group:
  - Packs instances close together within a single AZ.
  - Provides low-latency, high-bandwidth networking (up to 100 Gbps).
  - Risk: if the rack fails, ALL instances in the group can fail.
  - Use for: HPC, tightly coupled parallel workloads (MPI), Kafka brokers.

Spread placement group:
  - Each instance is placed on separate physical hardware (different rack/power/network).
  - Max 7 instances per AZ per spread group.
  - Maximum fault isolation.
  - Use for: small number of critical instances that must not fail together.

Partition placement group:
  - Divides instances into logical partitions; each partition has its own set of racks.
  - Up to 7 partitions per AZ. Hundreds of instances per group.
  - Applications can see which partition each instance is in.
  - Use for: Hadoop, Cassandra, Kafka — where rack-awareness matters.
```

---

## SECTION 4 — S3

**Q: S3 storage classes — when to use each?**

```
Standard:
  - Frequent access. 3 AZs. 99.99% availability. 99.999999999% durability.
  - Most expensive storage cost. No retrieval fee.
  - Use for: active data, frequently read files, static website assets.

Standard-IA (Infrequent Access):
  - Lower storage cost than Standard. Has a per-GB retrieval fee.
  - 3 AZs. 99.9% availability. Minimum 30-day storage charge.
  - Use for: backups, disaster recovery files accessed occasionally.

One Zone-IA:
  - Like Standard-IA but stored in only ONE AZ. 20% cheaper.
  - If that AZ is destroyed, data is LOST.
  - Use for: reproducible data you can recreate (thumbnails, transcoded video).

Glacier Instant Retrieval:
  - Archive storage. Millisecond retrieval (same as Standard).
  - Minimum 90-day storage. Lower cost than Standard-IA.
  - Use for: data accessed once per quarter.

Glacier Flexible Retrieval:
  - Archive. Minutes to hours retrieval (Expedited: 1-5 min, Standard: 3-5 hrs).
  - Minimum 90-day storage. Much cheaper.
  - Use for: long-term backups, compliance archives accessed rarely.

Glacier Deep Archive:
  - Cheapest storage class. 12-48 hour retrieval.
  - Minimum 180-day storage.
  - Use for: 7-10 year compliance archives, data you almost never access.

Intelligent-Tiering:
  - Automatically moves objects between tiers based on access patterns.
  - Monitoring fee per object. No retrieval fees.
  - Use for: data with unknown or changing access patterns.
```

---

**Q: How do you secure an S3 bucket?**

```
1. Block Public Access settings — enable all 4 settings to prevent any public exposure.
   This is a safety net against misconfigured bucket policies or ACLs.

2. Bucket Policy — IAM JSON policy attached to the bucket.
   Can restrict access to specific IAM principals, VPC endpoints, IP ranges, MFA.

3. Encryption:
   - SSE-S3:   AWS manages keys. AES-256. Default for new buckets.
   - SSE-KMS:  Customer Managed Keys (CMK) in AWS KMS. Full audit trail in CloudTrail.
               Key rotation. Can restrict who can decrypt.
   - SSE-C:    Customer provides the encryption key. AWS performs encryption but never stores key.
   - CSE:      Client-side encryption before upload.

4. VPC Endpoint — access S3 from your VPC without traffic going through the public internet.
   Use endpoint policy to restrict which buckets are accessible.

5. CloudTrail + S3 Access Logs — audit every API call (who accessed what, when, from where).

6. S3 Object Lock — WORM (Write Once Read Many) for compliance. Two modes:
   - Compliance: nobody (not even root) can delete before retention period expires.
   - Governance: only users with special IAM permission can override.

7. MFA Delete — require MFA to permanently delete objects or change versioning state.
```

---

**Q: What is an S3 presigned URL?**

A: A time-limited URL that grants temporary access to a private S3 object. Signed using your
IAM credentials. The requester does not need any AWS credentials.

```python
import boto3

s3 = boto3.client('s3')

# Generate presigned URL for download
download_url = s3.generate_presigned_url(
    'get_object',
    Params={'Bucket': 'my-bucket', 'Key': 'reports/q4-2024.pdf'},
    ExpiresIn=3600    # expires in 1 hour
)

# Generate presigned URL for direct upload (PUT)
upload_url = s3.generate_presigned_url(
    'put_object',
    Params={
        'Bucket': 'my-bucket',
        'Key': 'uploads/user-123/avatar.jpg',
        'ContentType': 'image/jpeg'
    },
    ExpiresIn=300    # 5 minutes to upload
)
```

Use cases:
- Allow users to download private files directly from S3 (no proxying through your server)
- Allow clients to upload directly to S3 (bypass your server bandwidth)
- Share private reports with external partners temporarily

---

## SECTION 5 — VPC & Networking

**Q: Explain all VPC components.**

```
VPC (Virtual Private Cloud):
  - Isolated virtual network in a single AWS region.
  - Defined by a CIDR block (e.g., 10.0.0.0/16 = 65,536 IPs).
  - You can have up to 5 VPCs per region (soft limit).

Subnet:
  - A subdivision of the VPC CIDR in a single Availability Zone.
  - Public subnet: has a route to an Internet Gateway. Resources can have public IPs.
  - Private subnet: no route to Internet Gateway. Resources only have private IPs.
  - Each subnet CIDR must be within the VPC CIDR.

Internet Gateway (IGW):
  - Allows resources in PUBLIC subnets to communicate with the internet.
  - Performs NAT for instances with public IPs.
  - Horizontally scaled, redundant, highly available.

NAT Gateway:
  - Allows resources in PRIVATE subnets to initiate OUTBOUND internet connections.
  - Inbound connections from the internet are blocked (one-way).
  - Must be placed in a PUBLIC subnet.
  - Managed service. Highly available within an AZ. Costs ~$0.045/hr + data.
  - Use a NAT Gateway per AZ for high availability.

Route Table:
  - A set of routing rules (destination CIDR -> target).
  - Each subnet is associated with exactly one route table.
  - Public subnet route table: 0.0.0.0/0 -> Internet Gateway
  - Private subnet route table: 0.0.0.0/0 -> NAT Gateway

Security Group:
  - STATEFUL virtual firewall applied at the network interface level (EC2, RDS, Lambda, etc.)
  - Only ALLOW rules. There is no explicit deny.
  - All rules are evaluated (not order-dependent).
  - Return traffic is automatically allowed (stateful).

Network ACL (NACL):
  - STATELESS firewall applied at the subnet level.
  - Both ALLOW and DENY rules.
  - Rules are evaluated in order (lowest number first). First match wins.
  - Must explicitly allow BOTH inbound AND outbound traffic (including ephemeral ports).
  - Use for subnet-level blocks (e.g., block a malicious IP range).

VPC Peering:
  - Direct network connection between two VPCs (same or different accounts/regions).
  - NOT transitive: if A peers B and B peers C, A cannot reach C via B.
  - No overlapping CIDR blocks allowed.

Transit Gateway:
  - Hub-and-spoke model connecting many VPCs and on-premises networks.
  - Transitive routing. Simplifies complex multi-VPC architectures.
  - Supports route tables, attachments, and multicast.

VPC Endpoint:
  - Private connectivity to AWS services without internet traversal.
  - Interface endpoint: uses AWS PrivateLink. Has a private IP in your VPC.
    Used for: SQS, SNS, Secrets Manager, ECR, most AWS services.
  - Gateway endpoint: only for S3 and DynamoDB. Free. Uses route table entries.
```

---

**Q: Security Group vs Network ACL — know this perfectly.**

```
Security Group:
  - Level: Instance / ENI (Elastic Network Interface)
  - State: Stateful — if you allow inbound port 443, return traffic is automatically allowed
  - Rules: ALLOW only. No explicit DENY rules.
  - Evaluation: ALL rules checked, most permissive wins
  - Applies to: EC2, RDS, Lambda in VPC, ECS tasks, load balancers
  - Default: deny all inbound, allow all outbound

Network ACL:
  - Level: Subnet
  - State: Stateless — must explicitly allow BOTH directions
  - Rules: ALLOW and DENY rules
  - Evaluation: Rules in order (lowest number first). First match wins. Stops evaluating.
  - Applies to: all resources in the subnet
  - Default: allow all inbound and outbound (default NACL)
            deny all inbound and outbound (custom NACL — must add rules)

When to use NACL vs Security Group:
  - Security Group: for most access control (always use these first)
  - NACL: when you need to BLOCK specific IPs at the subnet level
          (e.g., block a known attacker IP — you can't do this with SG)
```

---

## SECTION 6 — IAM

**Q: IAM core concepts — Users, Groups, Roles, Policies.**

```
User:
  - A permanent identity for a person or application.
  - Has long-term credentials: password and/or access keys (Access Key ID + Secret).
  - Best practice: only for human console access. NOT for applications/services.

Group:
  - A collection of IAM users.
  - Policies attached to a group apply to all users in the group.
  - Cannot be nested (no groups within groups).

Role:
  - A set of permissions that can be ASSUMED temporarily.
  - Has no permanent credentials — AWS issues temporary security tokens (STS).
  - Best practice for: EC2 instances, Lambda functions, ECS tasks, CI/CD pipelines,
                       cross-account access, federated users (SSO).
  - Why roles over access keys: credentials rotate automatically, no key to leak,
                                principle of least privilege, full CloudTrail audit.

Policy:
  - A JSON document that defines what actions are ALLOWED or DENIED on which RESOURCES.
  - AWS Managed: maintained by AWS. Updated when new features are added.
  - Customer Managed: you write and maintain. More precise control.
  - Inline: embedded directly in a user/group/role. Tight 1:1 relationship.
  - Resource-based: attached to a resource (S3 bucket policy, SQS queue policy, KMS key policy).
```

---

**Q: How does an EC2 instance access AWS services securely (no hardcoded credentials)?**

A: Attach an IAM Role to the EC2 instance via an Instance Profile. The AWS SDK automatically
retrieves temporary credentials from the Instance Metadata Service (IMDS):
`http://169.254.169.254/latest/meta-data/iam/security-credentials/<role-name>`

These temporary credentials (AccessKeyId, SecretAccessKey, SessionToken) are automatically
rotated by AWS before they expire. Your application never stores long-term credentials.

```python
import boto3

# The SDK automatically fetches credentials from:
# 1. Environment variables (AWS_ACCESS_KEY_ID, AWS_SECRET_ACCESS_KEY)
# 2. ~/.aws/credentials file
# 3. EC2 Instance Metadata Service (IMDS) — for EC2 with IAM role
# 4. ECS task role / Lambda execution role
# No code needed — just call boto3 and it figures out credentials automatically
s3 = boto3.client('s3')    # works on EC2 with a role, no keys in code
```

---

**Q: What is the principle of least privilege?**

A: Grant only the minimum permissions required to perform a specific task. Nothing more.

```json
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Sid": "AllowReadFromSpecificBucket",
      "Effect": "Allow",
      "Action": [
        "s3:GetObject",
        "s3:GetObjectVersion"
      ],
      "Resource": "arn:aws:s3:::my-specific-bucket/*"
    }
  ]
}
```

NOT `"Action": "s3:*"` on `"Resource": "*"`. Be specific about both actions and resources.

---

## SECTION 7 — CloudWatch

**Q: CloudWatch core concepts.**

```
Metrics:
  - Time-series data points identified by namespace, metric name, and dimensions.
  - AWS publishes metrics automatically: EC2 (CPU, network, disk), Lambda (invocations,
    errors, duration, throttles, concurrency), SQS (queue depth, age of oldest message),
    RDS (connections, read/write IOPS, free storage), ALB (request count, error rate, latency).
  - Custom metrics: publish your own business metrics via PutMetricData API.
    Example: orders per minute, active users, payment failures.
  - Resolution: standard (1-minute granularity) or high-resolution (1-second, extra cost).

Logs:
  - Log Groups: logical container for log streams (e.g., /aws/lambda/my-function).
  - Log Streams: sequence of log events from a single source.
  - Retention: 1 day to 10 years, or never expire. Set retention to control costs!
  - Logs Insights: query language for log analysis. Like SQL for logs.
  - Subscription Filters: stream logs to Lambda, Kinesis, Elasticsearch in real-time.

Alarms:
  - Trigger on: a metric threshold, anomaly detection, or a math expression.
  - States: OK, ALARM, INSUFFICIENT_DATA.
  - Actions: SNS notification, Auto Scaling policy, EC2 action (stop/terminate/reboot).
  - Composite alarms: AND/OR logic combining multiple alarms.

Dashboards:
  - Visualise metrics from across services, regions, and accounts in one view.
  - Share with stakeholders. Auto-refresh. Support cross-account data.

EventBridge (formerly CloudWatch Events):
  - Schedule Lambda/Step Functions on cron or rate expressions.
  - React to events from AWS services, SaaS apps, or custom sources.
  - More powerful and flexible than the old CloudWatch Events.
```

---

**Q: What SQS metrics should you alarm on?**

```
ApproximateNumberOfMessagesVisible:
  - Queue depth. If this grows unboundedly, consumers are too slow or broken.
  - Alarm: > X for 15 minutes -> scale up consumers or alert.

ApproximateAgeOfOldestMessage:
  - How old is the oldest unprocessed message.
  - Alarm: > your SLA for message processing latency.

NumberOfMessagesSent / NumberOfMessagesDeleted:
  - Throughput metrics. Useful for dashboards and capacity planning.

ApproximateNumberOfMessagesNotVisible:
  - Messages currently being processed (in-flight).

DLQ ApproximateNumberOfMessagesVisible:
  - CRITICAL: any message in your DLQ means processing is failing.
  - Alarm: > 0 -> immediate alert.
```

---

## SECTION 8 — SNS vs SQS vs EventBridge

```
SNS (Simple Notification Service):
  Purpose: Pub/Sub messaging. One message published to ONE topic reaches MANY subscribers.
  Model: Push-based. SNS pushes to subscribers (SQS, Lambda, HTTP, email, SMS, mobile push).
  Persistence: No. If a subscriber is unavailable, the message is lost (for HTTP endpoints).
               SQS subscribers are durable (message queued for later delivery).
  Use for: fan-out (one event -> multiple parallel processors), notifications, alerts.
  Pattern: SNS topic -> multiple SQS queues (fan-out + durable buffering).

SQS (Simple Queue Service):
  Purpose: Message queuing. Durable buffer between producers and consumers.
  Model: Pull-based. Consumers poll the queue.
  Persistence: Yes. Messages persist until deleted (up to 14 days).
  Use for: decoupling services, buffering traffic spikes, reliable async processing.

EventBridge:
  Purpose: Serverless event bus. Routes events from AWS services, SaaS, and custom apps.
  Model: Push-based. Routes events to targets based on content-based filtering rules.
  Persistence: Limited retry for some targets.
  Features: Rich JSON content-based filtering, schema registry, event replay, pipes.
  Use for: complex event routing, replacing CloudWatch Events, SaaS integrations.
           Better than SNS for routing to many different targets with conditional logic.

When to use which:
  SQS alone    — decoupling, buffering, reliable 1:1 async processing
  SNS alone    — simple fan-out notifications (email, SMS, push)
  SNS + SQS    — durable fan-out (one event processed by multiple systems reliably)
  EventBridge  — complex routing, AWS service integrations, scheduled tasks, SaaS events
```

---

## SECTION 9 — RDS & Databases

**Q: RDS key concepts.**

```
Multi-AZ Deployment:
  - Synchronous standby replica in a different AZ.
  - Automatic failover in ~1-2 minutes if primary fails (DNS flips to standby).
  - Standby is passive — does NOT serve read traffic.
  - For: high availability, not read scaling.

Read Replicas:
  - Asynchronous copies of the primary. Can be in same or different regions.
  - Serve read traffic (reporting queries, analytics).
  - Can be promoted to independent primary DB (for disaster recovery).
  - Up to 5 read replicas for MySQL/PostgreSQL. 15 for Aurora.

Aurora:
  - AWS cloud-native MySQL and PostgreSQL compatible engine.
  - 5x faster than standard MySQL, 3x faster than standard PostgreSQL (AWS claim).
  - Storage auto-scales from 10 GB to 128 TB in 10 GB increments.
  - Distributed storage: 6 copies of data across 3 AZs automatically.
  - Aurora Serverless v2: auto-scales compute up and down to 0 ACUs.
  - Aurora Global Database: cross-region replication with < 1 second latency.

RDS Proxy:
  - Connection pooler in front of RDS/Aurora.
  - Critical for Lambda: Lambda functions open a new DB connection on every cold start.
    Without RDS Proxy, 1000 concurrent Lambda invocations = 1000 DB connections.
    RDS Proxy pools and reuses connections, protecting the DB from connection exhaustion.
```

---

**Q: ElastiCache — Redis vs Memcached.**

```
Redis:
  - Data structures: strings, hashes, lists, sets, sorted sets, bitmaps, streams.
  - Persistence: RDB snapshots and AOF (append-only file).
  - Replication: primary + read replicas.
  - Clustering: Redis Cluster for sharding.
  - Pub/Sub: built-in message broker.
  - Lua scripting, transactions.
  - Use for: sessions, leaderboards (sorted sets), rate limiting, distributed locks,
             pub/sub, caching with complex data structures, job queues.

Memcached:
  - Simple key-value only. No persistence. No replication.
  - Multi-threaded (better for very high throughput simple caching).
  - Auto-discovery for cluster nodes.
  - Use when: you only need simple string caching and need maximum throughput.

Choose Redis unless you specifically need Memcached's multi-threaded simplicity.
```

---

## SECTION 10 — ECS vs EKS

```
ECS (Elastic Container Service):
  - AWS-native container orchestrator.
  - Simpler to set up and operate than EKS.
  - Tight native AWS integrations: IAM task roles, CloudWatch, ALB target groups,
    Service Discovery, Parameter Store, Secrets Manager.
  - Less operational overhead.
  - Task Definition = like a pod spec (image, CPU, memory, env vars, volumes).
  - Service = like a Deployment (desired count, LB integration, rolling updates).
  - Launch types:
      EC2: you manage the EC2 instances (cheaper at scale, more control).
      Fargate: serverless containers — you only define CPU/memory, no EC2 to manage.
  - Good for: teams new to containers, AWS-centric organisations, simpler architectures.

EKS (Elastic Kubernetes Service):
  - Managed Kubernetes control plane. AWS manages etcd, kube-apiserver, upgrades.
  - Industry standard — the same YAML manifests work on GKE, AKS, on-prem.
  - More complex, steeper learning curve, but more powerful and portable.
  - Access to full Kubernetes ecosystem: Helm, Argo, Istio, Karpenter, Crossplane.
  - Better for: multi-cloud strategies, teams with existing K8s expertise,
               advanced networking (service mesh), complex workloads.

When to choose ECS: simpler ops, all-in on AWS, smaller team, fewer services.
When to choose EKS: multi-cloud/portability, complex routing, Kubernetes ecosystem needed.
```
