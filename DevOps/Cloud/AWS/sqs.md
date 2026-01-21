# Amazon SQS (Simple Queue Service) - Complete Guide

## Table of Contents
1. [Introduction](#introduction)
2. [Core Concepts](#core-concepts)
3. [Queue Types](#queue-types)
4. [Message Lifecycle](#message-lifecycle)
5. [Message Attributes and Properties](#message-attributes-and-properties)
6. [Polling Mechanisms](#polling-mechanisms)
7. [Dead Letter Queues](#dead-letter-queues)
8. [Security](#security)
9. [Monitoring and Logging](#monitoring-and-logging)
10. [Integration Patterns](#integration-patterns)
11. [Best Practices](#best-practices)
12. [Common Interview Questions](#common-interview-questions)

---

## Introduction

**Amazon SQS** is a fully managed message queuing service that enables you to decouple and scale microservices, distributed systems, and serverless applications. It eliminates the complexity and overhead of managing message-oriented middleware.

### Key Features
- **Fully Managed**: No infrastructure to manage
- **Highly Scalable**: Handle any volume of messages
- **Reliable**: Messages stored redundantly across multiple AZs
- **Secure**: Encryption at rest and in transit
- **Cost-Effective**: Pay only for what you use
- **Two Queue Types**: Standard and FIFO

### Why SQS?
- Decouple application components
- Scale independently
- Handle traffic spikes
- Ensure reliable message delivery
- No message loss
- Simple API
- No upfront costs

### Use Cases
- **Application Decoupling**: Separate frontend from backend
- **Work Queues**: Distribute tasks to workers
- **Buffer Writes**: Batch database writes
- **Event-Driven Architecture**: Trigger workflows
- **Load Leveling**: Smooth out traffic spikes
- **Order Processing**: Handle customer orders
- **Log Processing**: Aggregate and process logs

---

## Core Concepts

### 1. **Queue**
Named buffer that stores messages
- Identified by URL
- Exists in specific region
- Can be standard or FIFO
- Stores messages until consumed
- Retention: 1 minute to 14 days (default 4 days)

**Queue URL Format:**
```
https://sqs.{region}.amazonaws.com/{account-id}/{queue-name}
```

### 2. **Message**
Unit of data sent through queue
- Up to 256 KB of text (any format)
- Contains message body and optional attributes
- Unique message ID assigned by SQS
- Receipt handle for operations after receiving

**Message Components:**
- **Message Body**: Actual data (required)
- **Message Attributes**: Metadata (optional)
- **Message ID**: Unique identifier
- **Receipt Handle**: Token for delete/visibility operations
- **MD5 Hash**: Integrity verification

### 3. **Producer (Sender)**
Application that sends messages to queue
- Uses `SendMessage` API
- Can send batch (up to 10 messages)
- Receives message ID on success

### 4. **Consumer (Receiver)**
Application that retrieves and processes messages
- Uses `ReceiveMessage` API
- Processes message
- Deletes message using `DeleteMessage` API
- Must delete explicitly (not automatic)

### 5. **Visibility Timeout**
Time during which message is invisible after being received
- Default: 30 seconds
- Range: 0 seconds to 12 hours
- Prevents multiple consumers processing same message
- Can be extended using `ChangeMessageVisibility`
- If not deleted within timeout, message becomes visible again

**How it Works:**
```
1. Consumer receives message (message becomes invisible)
2. Visibility timeout starts (e.g., 30 seconds)
3. Consumer processes message
4. If processed: Delete message (removed from queue)
5. If timeout expires: Message becomes visible again
```

### 6. **Message Retention Period**
How long messages stay in queue if not deleted
- Default: 4 days
- Range: 1 minute to 14 days
- After retention period, message automatically deleted
- Configure per queue

### 7. **Delivery Delay**
Postpone message delivery
- Default: 0 seconds (immediate)
- Range: 0 to 15 minutes
- Can set at queue or message level
- Useful for delayed processing

### 8. **Receive Message Wait Time**
Long polling duration
- Default: 0 seconds (short polling)
- Range: 0 to 20 seconds
- Reduces empty responses
- Lowers costs

---

## Queue Types

### 1. **Standard Queue**

Default queue type with maximum throughput.

**Characteristics:**
- **Unlimited Throughput**: Nearly unlimited TPS (transactions per second)
- **At-Least-Once Delivery**: Message delivered at least once, possibly more
- **Best-Effort Ordering**: Messages generally delivered in order sent
- **No Guarantee of Order**: May arrive out of order
- **Duplicates Possible**: Same message may be delivered multiple times

**When to Use:**
- Throughput is priority
- Duplicate messages acceptable
- Order doesn't matter
- Application can handle duplicates

**Example Use Cases:**
- Batch operations
- Log aggregation
- Image processing
- Video transcoding
- Data pipelines

**Advantages:**
- Unlimited throughput
- Lower latency
- No restrictions

**Limitations:**
- No ordering guarantee
- Possible duplicates
- Application must handle deduplication

### 2. **FIFO Queue**

Guarantees order and exactly-once processing.

**Characteristics:**
- **First-In-First-Out**: Strict ordering preserved
- **Exactly-Once Processing**: No duplicates
- **Limited Throughput**: 
  - Without batching: 300 TPS
  - With batching: 3,000 TPS
- **Message Groups**: Ordered within groups
- **Content-Based Deduplication**: Automatic duplicate prevention

**Queue Naming:**
- Must end with `.fifo` suffix
- Example: `my-queue.fifo`

**When to Use:**
- Order critical
- No duplicates allowed
- Financial transactions
- Event sequencing
- Command processing

**Example Use Cases:**
- Order processing
- Banking transactions
- Stock trading
- User commands
- E-commerce checkout

**Key Features:**

#### **Message Groups**
Logical grouping of messages
- Messages in same group processed in order
- Different groups processed in parallel
- Specified by `MessageGroupId`

**Example:**
```
Group 1 (User A): Msg1 → Msg2 → Msg3 (sequential)
Group 2 (User B): Msg1 → Msg2 → Msg3 (sequential)
Groups processed in parallel
```

#### **Deduplication**
Prevents duplicate messages within 5-minute window

**Two Methods:**

**1. Content-Based Deduplication**
- Enable on queue
- SQS generates ID from message body
- Same body = duplicate
- Automatic

**2. Message Deduplication ID**
- Explicitly provide ID
- More control
- Custom logic

```json
{
  "MessageBody": "Order #123",
  "MessageDeduplicationId": "order-123-2025-01-01",
  "MessageGroupId": "user-456"
}
```

**Advantages:**
- Guaranteed order
- No duplicates
- Simplified application logic

**Limitations:**
- Lower throughput (300-3,000 TPS)
- Must end with .fifo
- Higher latency

### Standard vs FIFO Comparison

| Feature | Standard | FIFO |
|---------|----------|------|
| Throughput | Unlimited | 300-3,000 TPS |
| Ordering | Best-effort | Guaranteed |
| Duplicates | Possible | Not possible |
| Latency | Lower | Slightly higher |
| Use case | High volume | Critical ordering |
| Cost | Lower | Same |
| Naming | Any | Must end .fifo |

---

## Message Lifecycle

### 1. **Sending Messages**

**Single Message:**
```bash
aws sqs send-message \
  --queue-url https://sqs.us-east-1.amazonaws.com/123456789012/MyQueue \
  --message-body "Hello World" \
  --message-attributes '{"Priority":{"DataType":"Number","StringValue":"1"}}'
```

**Batch Send (up to 10):**
```bash
aws sqs send-message-batch \
  --queue-url https://sqs.us-east-1.amazonaws.com/123456789012/MyQueue \
  --entries '[
    {"Id":"1","MessageBody":"Message 1"},
    {"Id":"2","MessageBody":"Message 2"}
  ]'
```

**Delayed Message:**
```bash
aws sqs send-message \
  --queue-url URL \
  --message-body "Delayed message" \
  --delay-seconds 300
```

### 2. **Receiving Messages**

**Short Polling (default):**
```bash
aws sqs receive-message \
  --queue-url URL \
  --max-number-of-messages 10
```

**Long Polling:**
```bash
aws sqs receive-message \
  --queue-url URL \
  --max-number-of-messages 10 \
  --wait-time-seconds 20
```

**Parameters:**
- `MaxNumberOfMessages`: 1-10 (default 1)
- `WaitTimeSeconds`: 0-20 (long polling)
- `VisibilityTimeout`: Override queue default
- `AttributeNames`: Request message attributes
- `MessageAttributeNames`: Request custom attributes

### 3. **Processing Messages**

**Workflow:**
```
1. Receive message
2. Message becomes invisible (visibility timeout)
3. Process message
4. Delete message (if successful)
5. If processing fails:
   - Let visibility timeout expire (retry)
   - Or move to DLQ after max attempts
```

### 4. **Deleting Messages**

Must delete after successful processing:
```bash
aws sqs delete-message \
  --queue-url URL \
  --receipt-handle "RECEIPT_HANDLE"
```

**Batch Delete (up to 10):**
```bash
aws sqs delete-message-batch \
  --queue-url URL \
  --entries '[
    {"Id":"1","ReceiptHandle":"HANDLE1"},
    {"Id":"2","ReceiptHandle":"HANDLE2"}
  ]'
```

### 5. **Visibility Timeout Management**

**Extend Timeout:**
```bash
aws sqs change-message-visibility \
  --queue-url URL \
  --receipt-handle "HANDLE" \
  --visibility-timeout 300
```

**Use Cases:**
- Long processing tasks
- Prevent message redelivery
- Give more time to complete

### 6. **Message Flow Diagram**

```
Producer → [Send] → Queue → [Receive] → Consumer
                      ↓         ↓
                  Invisible  Process
                      ↓         ↓
                   Timeout   [Delete]
                      ↓         ↓
                   Visible   Removed
                      ↓
                   Retry or DLQ
```

---

## Message Attributes and Properties

### System Attributes

**ApproximateReceiveCount**
- Number of times message received
- Used for DLQ configuration
- Tracks failed attempts

**SentTimestamp**
- When message sent to queue
- Milliseconds since epoch

**ApproximateFirstReceiveTimestamp**
- First time message received
- Milliseconds since epoch

**SenderId**
- AWS account ID or IAM principal

**MessageDeduplicationId** (FIFO only)
- Deduplication identifier

**MessageGroupId** (FIFO only)
- Message group identifier

**SequenceNumber** (FIFO only)
- Large non-consecutive number
- Assigned by SQS

### Message Attributes

Custom metadata (up to 10 attributes)

**Data Types:**
- String
- Number
- Binary
- String.Array (custom)

**Example:**
```json
{
  "MessageAttributes": {
    "Priority": {
      "DataType": "Number",
      "StringValue": "1"
    },
    "Author": {
      "DataType": "String",
      "StringValue": "John Doe"
    },
    "Timestamp": {
      "DataType": "Number",
      "StringValue": "1640000000"
    }
  }
}
```

**Use Cases:**
- Filtering messages
- Message routing
- Priority processing
- Metadata without parsing body

### Large Messages

**Size Limits:**
- Standard: 256 KB
- Extended (using S3): Up to 2 GB

**Extended Client Library:**
- Automatically stores large payloads in S3
- Queue stores S3 pointer
- Transparent to application

**Implementation:**
```
1. Message > 256 KB
2. Upload to S3
3. Send S3 reference to SQS
4. Consumer retrieves from S3
5. Delete from S3 after processing
```

---

## Polling Mechanisms

### 1. **Short Polling (Default)**

Queries subset of servers for messages.

**Characteristics:**
- `WaitTimeSeconds = 0`
- Returns immediately
- May return empty even if messages available
- Samples subset of servers
- More API calls = higher cost

**When to Use:**
- Need immediate response
- Low latency critical
- Infrequent messages

**Drawbacks:**
- More empty responses
- Higher costs
- More API calls
- Less efficient

### 2. **Long Polling (Recommended)**

Waits for messages to arrive.

**Characteristics:**
- `WaitTimeSeconds = 1-20`
- Waits up to specified seconds
- Returns when message available or timeout
- Queries all servers
- Fewer empty responses

**Configuration:**

**Queue Level:**
```bash
aws sqs set-queue-attributes \
  --queue-url URL \
  --attributes ReceiveMessageWaitTimeSeconds=20
```

**Request Level:**
```bash
aws sqs receive-message \
  --queue-url URL \
  --wait-time-seconds 20
```

**Benefits:**
- Reduced costs (fewer API calls)
- Fewer empty responses
- More efficient
- Better latency for messages

**When to Use:**
- Most use cases (recommended default)
- Cost optimization
- Regular message flow

### Short vs Long Polling

| Aspect | Short Polling | Long Polling |
|--------|---------------|--------------|
| Wait time | 0 seconds | 1-20 seconds |
| Empty responses | Many | Few |
| API calls | More | Fewer |
| Cost | Higher | Lower |
| Latency | Lower | Slightly higher |
| Efficiency | Lower | Higher |
| Recommended | Rarely | Most cases |

---

## Dead Letter Queues

### What is a DLQ?

Target queue for messages that can't be processed successfully.

**Purpose:**
- Isolate problematic messages
- Prevent infinite retries
- Debug failures
- Maintain main queue health

### How DLQ Works

```
Main Queue → Consumer (fails) → Retry
              ↓ (after max receives)
         Dead Letter Queue
              ↓
       Analysis/Debugging
```

**Configuration:**
```json
{
  "RedrivePolicy": {
    "deadLetterTargetArn": "arn:aws:sqs:region:account:DLQ",
    "maxReceiveCount": 3
  }
}
```

### Key Concepts

**maxReceiveCount**
- Maximum times message received before moving to DLQ
- Range: 1-1000
- Common: 3-5

**ApproximateReceiveCount**
- Tracked by SQS
- Increments each receive
- When reaches maxReceiveCount → DLQ

### DLQ Best Practices

**1. Use Same Queue Type**
- Standard DLQ for standard queue
- FIFO DLQ for FIFO queue
- Must match

**2. Set Appropriate Retention**
- DLQ retention ≥ source queue retention
- Typically 14 days (maximum)
- Time to investigate and fix

**3. Monitor DLQ**
- CloudWatch alarms
- Alert on messages in DLQ
- Investigate promptly

**4. Redrive Messages**
- Fix root cause
- Move messages back to source queue
- Use redrive policy

### Redrive to Source Queue

**Redrive API:**
```bash
aws sqs start-message-move-task \
  --source-arn "arn:aws:sqs:region:account:DLQ" \
  --destination-arn "arn:aws:sqs:region:account:MainQueue" \
  --max-number-of-messages-per-second 10
```

**Use Cases:**
- After fixing bug
- After deploying fix
- After resolving dependency issues

### Common DLQ Scenarios

**Poison Messages:**
- Malformed data
- Invalid format
- Missing required fields

**Solution:** Validate and sanitize

**Processing Errors:**
- Database unavailable
- External API timeout
- Insufficient permissions

**Solution:** Fix infrastructure/permissions

**Business Logic Failures:**
- Invalid state
- Constraint violations
- Business rule failures

**Solution:** Review and update logic

### DLQ Monitoring

**CloudWatch Metrics:**
- ApproximateNumberOfMessagesVisible
- ApproximateAgeOfOldestMessage
- NumberOfMessagesSent
- NumberOfMessagesReceived

**Alarms:**
```
IF ApproximateNumberOfMessagesVisible > 0
THEN Alert team
```

---

## Security

### 1. **Encryption**

#### **Encryption at Rest**
- Uses AWS KMS
- Server-side encryption (SSE)
- Automatic encryption/decryption
- Transparent to applications

**Enable:**
```bash
aws sqs set-queue-attributes \
  --queue-url URL \
  --attributes KmsMasterKeyId=alias/aws/sqs
```

**Keys:**
- AWS managed: `alias/aws/sqs` (free)
- Customer managed: Custom KMS key (paid)

#### **Encryption in Transit**
- All API calls use HTTPS
- TLS 1.2+
- Automatic
- No configuration needed

### 2. **Access Control**

#### **IAM Policies**
Control who can access queue

**Example Policy:**
```json
{
  "Version": "2012-10-17",
  "Statement": [{
    "Effect": "Allow",
    "Action": [
      "sqs:SendMessage",
      "sqs:ReceiveMessage",
      "sqs:DeleteMessage"
    ],
    "Resource": "arn:aws:sqs:region:account:QueueName"
  }]
}
```

**Common Actions:**
- `sqs:SendMessage`
- `sqs:ReceiveMessage`
- `sqs:DeleteMessage`
- `sqs:ChangeMessageVisibility`
- `sqs:GetQueueAttributes`
- `sqs:GetQueueUrl`

#### **Queue Policies (Resource-Based)**
Control access to specific queue

**Example: Allow S3 to send messages:**
```json
{
  "Version": "2012-10-17",
  "Statement": [{
    "Effect": "Allow",
    "Principal": {
      "Service": "s3.amazonaws.com"
    },
    "Action": "sqs:SendMessage",
    "Resource": "arn:aws:sqs:region:account:QueueName",
    "Condition": {
      "ArnEquals": {
        "aws:SourceArn": "arn:aws:s3:::my-bucket"
      }
    }
  }]
}
```

**Use Cases:**
- Cross-account access
- Service-to-service access
- Public access (careful!)
- IP restrictions

### 3. **VPC Endpoints**

Access SQS without internet gateway

**Benefits:**
- Private connectivity
- No public IP needed
- Reduced data transfer costs
- Enhanced security

**Configuration:**
- Create VPC endpoint for SQS
- Update route tables
- Update security groups
- Use endpoint URL

### 4. **Audit and Compliance**

**CloudTrail Integration:**
- Logs all API calls
- Who, what, when
- Compliance audits
- Security analysis

**Logged Actions:**
- CreateQueue
- DeleteQueue
- SendMessage
- ReceiveMessage
- DeleteMessage
- ChangeMessageVisibility
- PurgeQueue

---

## Monitoring and Logging

### CloudWatch Metrics

**Queue Metrics:**

**ApproximateNumberOfMessagesVisible**
- Messages available for retrieval
- Not in-flight
- Most important metric

**ApproximateNumberOfMessagesNotVisible**
- Messages in-flight
- Being processed
- Hidden by visibility timeout

**ApproximateNumberOfMessagesDelayed**
- Messages with delay
- Not yet available

**ApproximateAgeOfOldestMessage**
- Age of oldest non-deleted message
- Seconds
- Indicates processing lag

**NumberOfMessagesSent**
- Messages added to queue
- Sum over period

**NumberOfMessagesReceived**
- Messages retrieved
- Sum over period

**NumberOfMessagesDeleted**
- Messages deleted
- Sum over period

**NumberOfEmptyReceives**
- Receive calls with no messages
- Indicates short polling inefficiency

**SentMessageSize**
- Size of messages (bytes)
- Average/sum/min/max

### CloudWatch Alarms

**Common Alarms:**

**Queue Depth:**
```
IF ApproximateNumberOfMessagesVisible > 1000
FOR 5 minutes
THEN Scale up consumers
```

**Old Messages:**
```
IF ApproximateAgeOfOldestMessage > 3600
FOR 5 minutes
THEN Alert team (processing too slow)
```

**DLQ Messages:**
```
IF DLQ ApproximateNumberOfMessagesVisible > 0
THEN Alert immediately
```

**Empty Receives:**
```
IF NumberOfEmptyReceives > 1000
FOR 10 minutes
THEN Enable long polling
```

### CloudWatch Logs

**Application Logging:**
- Log message IDs
- Processing results
- Errors and exceptions
- Processing duration

**Best Practices:**
- Structured logging (JSON)
- Include correlation IDs
- Log message attributes
- Track processing metrics

### X-Ray Integration

**Distributed Tracing:**
- Trace messages through system
- Identify bottlenecks
- Visualize message flow
- Performance analysis

**Implementation:**
- Instrument producer
- Instrument consumer
- Pass trace header
- View service map

---

## Integration Patterns

### 1. **Fan-Out Pattern**

One message to multiple consumers.

**Architecture:**
```
Producer → SNS Topic → Multiple SQS Queues → Multiple Consumers
```

**Use Cases:**
- Broadcast notifications
- Parallel processing
- Different processing logic per consumer

**Example:**
```
Order Created →
  → Email Queue → Send confirmation email
  → Inventory Queue → Update inventory
  → Analytics Queue → Track metrics
```

### 2. **Work Queue Pattern**

Distribute work among multiple workers.

**Architecture:**
```
Producer → SQS Queue → Multiple Workers (competing consumers)
```

**Use Cases:**
- Task distribution
- Load balancing
- Parallel processing

**Scaling:**
- Add/remove workers dynamically
- Based on queue depth
- Auto Scaling Group

### 3. **Priority Queue Pattern**

Different priorities for messages.

**Implementation:**
```
High Priority Queue → Workers (poll frequently)
Medium Priority Queue → Workers
Low Priority Queue → Workers (poll less frequently)
```

**Alternative:**
- Use message attributes
- Worker decides priority
- Process high priority first

### 4. **Request-Response Pattern**

Synchronous-like communication.

**Architecture:**
```
Client → Request Queue → Server
       ← Response Queue ←
```

**Implementation:**
- Include reply-to queue URL
- Include correlation ID
- Client polls response queue
- Match responses by correlation ID

### 5. **Saga Pattern**

Distributed transactions.

**Architecture:**
```
Service A → Queue 1 → Service B → Queue 2 → Service C
     ↓ (if failure)
Compensating transactions
```

**Use Cases:**
- Microservices transactions
- Multi-step workflows
- Rollback on failure

### 6. **Event Sourcing**

Store state changes as events.

**Architecture:**
```
Command → SQS → Event Handler → Event Store → SQS → Projections
```

**Benefits:**
- Audit trail
- Replay events
- Multiple views of data

### 7. **CQRS (Command Query Responsibility Segregation)**

Separate read and write models.

**Architecture:**
```
Commands → Command Queue → Write Model → Events → Event Queue → Read Models
```

### AWS Service Integrations

**Lambda:**
- Event source mapping
- Automatic polling
- Batch processing
- Parallel execution

**SNS:**
- Fan-out pattern
- Topic subscribers
- Message filtering

**S3:**
- S3 events to SQS
- Process uploaded files

**EventBridge:**
- Event-driven architectures
- Rule-based routing

**Step Functions:**
- Orchestrate workflows
- Long-running processes

**API Gateway:**
- HTTP to SQS
- REST API frontend

**ECS/EKS:**
- Container-based consumers
- Scalable processing

---

## Best Practices

### 1. **Queue Design**

**Use Long Polling:**
- Set ReceiveMessageWaitTimeSeconds = 20
- Reduces costs
- More efficient

**Set Appropriate Visibility Timeout:**
- Should be 6x average processing time
- Too short: Duplicate processing
- Too long: Slow retries
- Adjust based on monitoring

**Configure Dead Letter Queue:**
- Isolate problematic messages
- Set maxReceiveCount appropriately
- Monitor DLQ closely

**Choose Right Queue Type:**
- Standard: High throughput, best-effort order
- FIFO: Guaranteed order, exactly-once

**Retention Period:**
- Balance between recovery time and cost
- Critical systems: 14 days
- Short-lived data: 1-4 days

### 2. **Message Design**

**Keep Messages Small:**
- Under 256 KB
- Use S3 for large payloads
- Faster processing

**Include Metadata:**
- Use message attributes
- Processing hints
- Priority information
- Timestamps

**Idempotent Processing:**
- Handle duplicates gracefully
- Use unique identifiers
- Check before acting

**Message Body Format:**
- JSON (most common)
- XML
- Protocol Buffers
- Consistent format

### 3. **Producer Best Practices**

**Batch Operations:**
- Send up to 10 messages at once
- Reduces API calls
- Lower costs
- Better throughput

**Error Handling:**
- Retry on failure
- Exponential backoff
- Log failures
- Monitor send success rate

**Message Attributes:**
- Include relevant metadata
- Enable filtering
- Avoid parsing body for routing

**Validate Before Sending:**
- Check message format
- Validate size
- Ensure required fields

### 4. **Consumer Best Practices**

**Batch Receive:**
- Receive up to 10 messages
- Process in parallel or batch
- Delete in batch

**Extend Visibility Timeout:**
- If processing takes longer
- Prevent duplicate processing
- Use ChangeMessageVisibility

**Delete After Success:**
- Only delete when fully processed
- Handle partial failures
- Retry on error

**Implement Graceful Shutdown:**
- Stop receiving new messages
- Complete in-flight messages
- Extend visibility if needed

**Scale Consumers:**
- Based on queue depth
- ApproximateNumberOfMessagesVisible
- Auto Scaling
- CloudWatch alarms

### 5. **Error Handling**

**Implement Retries:**
- Transient errors (network, throttling)
- Exponential backoff
- Jitter to avoid thundering herd

**Use DLQ:**
- Move failed messages after max attempts
- Investigate and fix
- Redrive to source queue

**Poison Message Detection:**
- Log errors with message ID
- Identify patterns
- Fix root cause

**Circuit Breaker:**
- Stop processing if downstream fails
- Prevent cascading failures
- Resume when healthy

### 6. **Performance Optimization**

**Long Polling:**
- Always use for regular workloads
- Reduces empty receives
- Lower costs

**Batching:**
- Send/receive/delete in batches
- Up to 10 messages
- Better throughput

**Concurrent Processing:**
- Multiple threads/processes
- Parallel message processing
- Respect service limits

**Visibility Timeout:**
- Set based on actual processing time
- Monitor and adjust
- Extend if needed

### 7. **Security Best Practices**

**Encryption:**
- Enable at-rest encryption
- Use KMS
- In-transit automatic (HTTPS)

**Least Privilege:**
- IAM policies with minimum permissions
- Separate roles for producers/consumers
- Resource-based policies when needed

**VPC Endpoints:**
- Private connectivity
- No internet exposure
- Reduced costs

**Audit:**
- Enable CloudTrail
- Monitor access patterns
- Alert on anomalies

### 8. **Cost Optimization**

**Long Polling:**
- Fewer API calls
- Lower costs
- Enable by default

**Batch Operations:**
- Batch send/receive/delete
- Reduces API calls
- Same cost for 1 or 10 messages

**Right-Size Messages:**
- Avoid unnecessary data
- Use references (S3) for large data
- Compress if beneficial

**Appropriate Retention:**
- Don't over-retain
- Balance recovery vs cost
- Monitor and adjust

**Monitor Usage:**
- Track API calls
- Identify inefficiencies
- Optimize polling

### 9. **Monitoring Best Practices**

**Key Metrics:**
- Queue depth
- Age of oldest message
- Empty receives
- DLQ messages

**Alarms:**
- Queue depth too high
- Messages too old
- DLQ has messages
- Processing lag

**Dashboards:**
- Real-time visibility
- Historical trends
- Correlation with deployments

**Logging:**
- Structured logs
- Include message IDs
- Track processing metrics
- Error details

### 10. **Testing**

**Load Testing:**
- Test at expected volume
- Test spikes
- Measure latency

**Failure Testing:**
- Consumer failures
- Network issues
- Timeout scenarios
- DLQ behavior

**Integration Testing:**
- End-to-end flows
- Multiple services
- Error propagation

---

## Common Interview Questions

### Basic Level

**Q1: What is Amazon SQS?**
A: Amazon SQS is a fully managed message queuing service that enables you to decouple and scale microservices, distributed systems, and serverless applications. It allows asynchronous communication between application components without requiring them to be available at the same time.

**Q2: What are the two types of SQS queues?**
A:
- **Standard Queue**: Unlimited throughput, at-least-once delivery, best-effort ordering. May have duplicates and out-of-order messages.
- **FIFO Queue**: Guaranteed order (first-in-first-out), exactly-once processing, limited to 300-3,000 TPS. No duplicates. Queue name must end with `.fifo`.

**Q3: What is visibility timeout?**
A: Visibility timeout is the period during which a message is invisible to other consumers after being retrieved. During this time, the consumer processes the message. If the message isn't deleted before the timeout expires, it becomes visible again for retry. Default is 30 seconds, maximum is 12 hours.

**Q4: What is the maximum message size in SQS?**
A: The maximum message size is 256 KB. For larger messages, you can use the Amazon SQS Extended Client Library which stores the message payload in S3 and sends a reference through SQS (supports up to 2 GB).

**Q5: What is the default message retention period?**
A: The default retention period is 4 days. It can be configured from 1 minute to 14 days. After the retention period expires, messages are automatically deleted from the queue.

### Intermediate Level

**Q6: Explain the difference between short polling and long polling.**
A:
- **Short Polling** (default): Returns immediately, even if empty. Queries subset of servers. May miss messages. More API calls = higher cost.
- **Long Polling**: Waits up to 20 seconds for messages. Queries all servers. Fewer empty responses. Reduces costs. Set `WaitTimeSeconds` > 0 to enable.

**Recommendation:** Always use long polling for cost efficiency and fewer empty responses.

**Q7: What is a Dead Letter Queue and why is it important?**
A: A Dead Letter Queue (DLQ) is a target queue for messages that cannot be processed successfully after a maximum number of attempts. It's important because it:
- Isolates problematic messages from healthy ones
- Prevents infinite retry loops
- Enables debugging of failed messages
- Maintains main queue health
- Provides visibility into processing failures

**Configuration:** Set `maxReceiveCount` on source queue's redrive policy. When `ApproximateReceiveCount` reaches this limit, message moves to DLQ.

**Q8: How does SQS ensure message durability?**
A: SQS ensures durability by:
- Storing messages redundantly across multiple Availability Zones
- Replicating messages across multiple servers within each AZ
- Acknowledging SendMessage only after successful replication
- Not deleting messages until explicitly requested by consumer
- Maintaining messages even if servers fail

Messages are not lost unless explicitly deleted or retention period expires.

**Q9: Explain message groups in FIFO queues.**
A: Message groups in FIFO queues allow ordering within logical groups while enabling parallel processing across groups:
- Specified using `MessageGroupId` attribute
- Messages within same group processed in strict FIFO order
- Different groups can be processed in parallel
- Each group acts as a separate ordered queue
- Enables scalability while maintaining order where needed

**Example:** Order processing - each customer is a group. Customer A's orders processed in order, Customer B's orders processed in order, but A and B can be processed in parallel.

**Q10: How do you handle duplicate messages in Standard queues?**
A: Strategies to handle duplicates:
1. **Idempotent Processing**: Design operations to produce same result if executed multiple times
2. **Deduplication Logic**: Track processed message IDs in database/cache (DynamoDB, Redis)
3. **Unique Identifiers**: Use business-level unique IDs to detect duplicates
4. **Time-Based Windows**: Track recent message IDs (e.g., last hour)
5. **Use FIFO Queue**: If exact-once processing is critical

**Example:**
```python
# Check if message already processed
if not is_processed(message_id):
    process_message(message)
    mark_as_processed(message_id)
delete_message(receipt_handle)
```

### Advanced Level

**Q11: Design a scalable system using SQS for processing millions of images uploaded by users.**
A:
**Architecture:**
```
1. Upload Flow:
   S3 Bucket → S3 Event → SQS Standard Queue → Lambda/EC2 Workers

2. Processing Pipeline:
   Workers receive messages from queue (batch of 10)
   Validate image format and size
   Generate thumbnails (multiple sizes)
   Apply filters/transformations
   Store in S3 (processed images)
   Store metadata in DynamoDB
   Delete message from queue

3. Scaling Strategy:
   - Standard queue (high throughput needed)
   - Long polling (efficient)
   - Auto Scaling Group for EC2 workers based on:
     * ApproximateNumberOfMessagesVisible
     * Scale out if > 1000 messages
     * Scale in if < 100 messages
   - Or Lambda with SQS trigger (1000 concurrent by default)

4. Error Handling:
   - DLQ for failed processing (maxReceiveCount = 3)
   - Monitor DLQ with CloudWatch alarm
   - Retry after fixing issues

5. Optimization:
   - Batch operations (receive/delete 10 at once)
   - Use S3 reference in message (not full image)
   - Parallel processing within worker
   - Use Spot Instances for cost (fault-tolerant)

6. Monitoring:
   - Queue depth
   - Age of oldest message
   - Processing time per image
   - Success/failure rates
   - DLQ messages
```

**Q12: How would you implement a priority queue system using SQS?**
A: Multiple approaches:

**Approach 1: Multiple Queues (Recommended)**
```
High-Priority Queue    → Workers (poll every 1s)
Medium-Priority Queue  → Workers (poll every 5s)
Low-Priority Queue     → Workers (poll every 30s)

Workers prioritize:
1. Check high-priority queue
2. If empty, check medium-priority
3. If empty, check low-priority
```

**Approach 2: Message Attributes + Single Queue**
```
- Add Priority message attribute (1=high, 3=low)
- Workers receive multiple messages
- Sort by priority attribute
- Process highest priority first
- Challenge: No guaranteed ordering
```

**Approach 3: Combination**
```
Critical Queue (FIFO) → Dedicated workers
Standard Queue → Shared workers with priority attribute

Benefits:
- Critical messages guaranteed processing
- Standard messages use single queue (simpler)
- Optimal resource utilization
```

**Implementation:**
```python
def process_messages():
    # Try high priority first
    messages = receive_from_high_priority_queue()
    if messages:
        process_batch(messages)
        return
    
    # Then medium
    messages = receive_from_medium_priority_queue()
    if messages:
        process_batch(messages)
        return
    
    # Finally low
    messages = receive_from_low_priority_queue()
    if messages:
        process_batch(messages)
```

**Q13: Explain how you would implement exactly-once processing with Standard queues.**
A: Since Standard queues can deliver duplicates, implement application-level deduplication:

**Strategy 1: Idempotent Operations**
- Design operations that produce same result if repeated
- Example: "Set status to COMPLETE" vs "Increment counter"

**Strategy 2: Deduplication Table**
```python
def process_message(message):
    message_id = message['MessageId']
    business_id = extract_business_id(message['Body'])
    
    # DynamoDB with conditional write
    try:
        dynamodb.put_item(
            TableName='ProcessedMessages',
            Item={'id': business_id, 'processed_at': timestamp},
            ConditionExpression='attribute_not_exists(id)'
        )
    except ConditionalCheckFailedException:
        # Already processed, skip
        print(f"Duplicate detected: {business_id}")
        return
    
    # Process only if not duplicate
    actual_processing(message)
    
    # Delete from queue
    delete_message(receipt_handle)
```

**Strategy 3: Distributed Lock**
```python
# Use Redis or DynamoDB for locking
def process_with_lock(message):
    business_id = extract_business_id(message)
    
    with distributed_lock(business_id, ttl=300):
        # Only one worker can execute this
        if not is_already_processed(business_id):
            process_message(message)
            mark_processed(business_id)
```

**Strategy 4: Time-Window Deduplication**
```python
# Track recent messages (Redis with TTL)
def process_if_unique(message):
    message_hash = hash(message['Body'])
    key = f"msg:{message_hash}"
    
    # Set with NX (only if not exists) and TTL
    if redis.set(key, "1", nx=True, ex=3600):  # 1 hour window
        process_message(message)
        delete_message(receipt_handle)
    else:
        # Duplicate within window, skip
        delete_message(receipt_handle)
```

**Best Practice:** Combine idempotent design with deduplication checks for critical operations.

**Q14: How do you handle backpressure when consumers can't keep up with message rate?**
A:
**Strategies:**

**1. Scale Consumers Horizontally**
```
Auto Scaling based on queue depth:
- CloudWatch metric: ApproximateNumberOfMessagesVisible
- Scale out if > 1000 messages for 5 minutes
- Scale in if < 100 messages for 10 minutes
```

**2. Throttle Producers**
```python
def send_with_backpressure(message):
    queue_depth = get_queue_attributes()['ApproximateNumberOfMessages']
    
    if queue_depth > THRESHOLD:
        # Implement backoff
        time.sleep(calculate_backoff(queue_depth))
        # Or reject request with 429 (Rate Limited)
    
    send_message(message)
```

**3. Batch Processing**
```python
# Process multiple messages together
messages = receive_messages(max_messages=10)
process_batch(messages)  # More efficient than one-by-one
delete_batch(messages)
```

**4. Optimize Processing**
- Profile and optimize slow operations
- Parallel processing within consumer
- Cache frequently accessed data
- Database connection pooling

**5. Use Multiple Queues**
```
Fast-Track Queue → Optimized for speed
Bulk Queue → Batch processing with delay
```

**6. Circuit Breaker**
```python
if downstream_service_failing():
    # Stop consuming temporarily
    # Extend visibility timeout on in-flight messages
    # Wait for recovery
    # Resume processing
```

**7. Rate Limiting**
```
Limit consumers to X messages per second
Prevent overwhelming downstream services
```

**Monitoring:**
- ApproximateAgeOfOldestMessage (should stay low)
- If increasing → consumers can't keep up
- Alert and scale

**Q15: Compare SQS with other messaging services (SNS, Kinesis, EventBridge).**
A:

| Feature | SQS | SNS | Kinesis | EventBridge |
|---------|-----|-----|---------|-------------|
| **Type** | Queue | Pub/Sub | Stream | Event Bus |
| **Pattern** | Point-to-point | Fan-out | Real-time stream | Event routing |
| **Consumers** | Pull | Push | Pull | Push |
| **Ordering** | FIFO only | No | Yes | No |
| **Retention** | 1 min - 14 days | None | 24h - 365 days | None |
| **Replay** | No | No | Yes | No |
| **Throughput** | Very high | Very high | 1MB/s per shard | High |
| **Use case** | Work queues | Notifications | Real-time analytics | Event-driven |

**When to use each:**

**SQS:**
- Decouple components
- Work distribution
- Asynchronous processing
- Guaranteed delivery
- Pull-based consumption

**SNS:**
- Fan-out to multiple subscribers
- Push notifications
- Mobile alerts
- Email/SMS
- Broadcast messages

**Kinesis:**
- Real-time streaming
- Log aggregation
- Click stream analysis
- IoT data
- Replay capability needed

**EventBridge:**
- Event-driven architectures
- Complex routing rules
- SaaS integration
- Schedule-based events
- Schema registry

**Common Combinations:**
- SNS → SQS: Fan-out pattern
- Kinesis → Lambda → SQS: Stream processing with queuing
- EventBridge → SQS: Event filtering and queuing

**Q16: How would you implement a request-response pattern with SQS?**
A:
**Architecture:**
```
Client → Request Queue → Server → Response Queue → Client
```

**Implementation:**

**Client Side:**
```python
import uuid
import json

def send_request(request_data):
    # Generate unique correlation ID
    correlation_id = str(uuid.uuid4())
    
    # Create response queue (or use pre-created one)
    response_queue_url = create_or_get_response_queue()
    
    # Send request with metadata
    message = {
        'data': request_data,
        'reply_to': response_queue_url,
        'correlation_id': correlation_id
    }
    
    sqs.send_message(
        QueueUrl=request_queue_url,
        MessageBody=json.dumps(message)
    )
    
    # Poll for response
    return wait_for_response(response_queue_url, correlation_id)

def wait_for_response(queue_url, correlation_id, timeout=30):
    start_time = time.time()
    
    while time.time() - start_time < timeout:
        messages = sqs.receive_message(
            QueueUrl=queue_url,
            WaitTimeSeconds=20,  # Long polling
            MaxNumberOfMessages=10
        )
        
        for message in messages.get('Messages', []):
            body = json.loads(message['Body'])
            if body['correlation_id'] == correlation_id:
                # Found response
                sqs.delete_message(
                    QueueUrl=queue_url,
                    ReceiptHandle=message['ReceiptHandle']
                )
                return body['response']
    
    raise TimeoutError("No response received")
```

**Server Side:**
```python
def process_requests():
    while True:
        messages = sqs.receive_message(
            QueueUrl=request_queue_url,
            WaitTimeSeconds=20,
            MaxNumberOfMessages=10
        )
        
        for message in messages.get('Messages', []):
            body = json.loads(message['Body'])
            
            # Process request
            result = process_business_logic(body['data'])
            
            # Send response
            response = {
                'correlation_id': body['correlation_id'],
                'response': result
            }
            
            sqs.send_message(
                QueueUrl=body['reply_to'],
                MessageBody=json.dumps(response)
            )
            
            # Delete request
            sqs.delete_message(
                QueueUrl=request_queue_url,
                ReceiptHandle=message['ReceiptHandle']
            )
```

**Optimization:**
- Use temporary queues (delete after response)
- Implement timeout on client side
- Use message attributes for correlation ID
- Consider WebSocket for better request-response
- For truly synchronous needs, use API Gateway + Lambda

**Q17: Explain how you would handle message ordering across multiple consumers.**
A:

**FIFO Queue Approach:**
```python
# Use MessageGroupId for ordering
def send_ordered_messages(user_id, events):
    for event in events:
        sqs.send_message(
            QueueUrl=fifo_queue_url,
            MessageBody=json.dumps(event),
            MessageGroupId=str(user_id),  # All user's messages in order
            MessageDeduplicationId=event['id']
        )

# Consumer processes in order per user
# Multiple consumers can process different users in parallel
```

**Challenges with Multiple Consumers:**
- FIFO guarantees order within MessageGroupId
- Each MessageGroupId processed by one consumer at a time
- Other groups can be processed in parallel

**Scaling Strategy:**
```
User 1 (GroupId: user-1) → Consumer A
User 2 (GroupId: user-2) → Consumer B
User 3 (GroupId: user-3) → Consumer C

Within each group: strict order
Across groups: parallel processing
```

**Partition by Key:**
```python
def get_message_group_id(item):
    # Partition by some key
    key = item['user_id']  # or order_id, session_id, etc.
    return f"group-{key}"

# Ensures all messages for same key processed in order
# Different keys can be processed in parallel
```

**Global Ordering (Single Consumer):**
```
If absolute global order needed:
- Single MessageGroupId
- Only one consumer processes at a time
- Limits throughput to ~300 TPS
- Not scalable
```

**Alternative: Kinesis**
```
For true ordering at scale:
- Use Kinesis instead of SQS
- Partition key ensures ordering within partition
- Multiple shards for parallelism
- Each shard processes in order
```

**Q18: How do you ensure message processing completes before visibility timeout expires?**
A:

**Strategies:**

**1. Set Appropriate Initial Timeout**
```python
# Calculate based on expected processing time
AVG_PROCESSING_TIME = 120  # seconds
VISIBILITY_TIMEOUT = AVG_PROCESSING_TIME * 6  # 12 minutes buffer
```

**2. Dynamic Extension**
```python
import threading

def process_with_heartbeat(message, receipt_handle):
    # Start heartbeat thread
    stop_event = threading.Event()
    heartbeat = threading.Thread(
        target=extend_visibility_periodically,
        args=(receipt_handle, stop_event)
    )
    heartbeat.start()
    
    try:
        # Process message (may take variable time)
        result = long_running_process(message)
        
        # Success - stop heartbeat and delete
        stop_event.set()
        heartbeat.join()
        delete_message(receipt_handle)
        
    except Exception as e:
        # Error - stop heartbeat, let message return to queue
        stop_event.set()
        heartbeat.join()
        raise

def extend_visibility_periodically(receipt_handle, stop_event):
    while not stop_event.is_set():
        time.sleep(30)  # Extend every 30 seconds
        if not stop_event.is_set():
            sqs.change_message_visibility(
                QueueUrl=queue_url,
                ReceiptHandle=receipt_handle,
                VisibilityTimeout=300  # Extend by 5 minutes
            )
```

**3. Checkpoint-Based Processing**
```python
def process_with_checkpoints(message, receipt_handle):
    checkpoints = split_into_checkpoints(message)
    
    for i, checkpoint in enumerate(checkpoints):
        # Process chunk
        process_checkpoint(checkpoint)
        
        # Extend visibility after each checkpoint
        sqs.change_message_visibility(
            QueueUrl=queue_url,
            ReceiptHandle=receipt_handle,
            VisibilityTimeout=300
        )
    
    # All checkpoints done, delete message
    delete_message(receipt_handle)
```

**4. Monitor Processing Time**
```python
import time

def process_with_monitoring(message, receipt_handle):
    start_time = time.time()
    max_processing_time = 600  # 10 minutes
    
    while not is_complete:
        # Do work
        process_chunk()
        
        # Check if running too long
        if time.time() - start_time > max_processing_time:
            # Extend or abandon
            extend_visibility(receipt_handle, 300)
            start_time = time.time()
```

**5. Break Down Large Tasks**
```python
# Instead of one large task
def split_work(large_task):
    # Split into smaller subtasks
    subtasks = divide_task(large_task)
    
    # Send each as separate message
    for subtask in subtasks:
        send_message(subtask)
    
    # Each subtask completes within visibility timeout
```

**Best Practices:**
- Start with conservative visibility timeout
- Monitor actual processing times
- Adjust based on P95/P99 latency
- Always extend before timeout expires
- Log when extensions happen (investigate why taking long)

**Q19: Design a system to handle 1 million messages per second with SQS.**
A:

**Architecture:**

**1. Multiple Standard Queues (Sharding)**
```
Producers → Load Balancer → Multiple SQS Queues (shard by key)
                              Queue 1 → Consumer Group 1
                              Queue 2 → Consumer Group 2
                              ...
                              Queue N → Consumer Group N
```

**2. Producer Layer**
```python
# Hash-based sharding
def send_message(message):
    # Determine shard
    shard_key = message['user_id']  # or other partition key
    queue_index = hash(shard_key) % NUM_QUEUES
    queue_url = QUEUE_URLS[queue_index]
    
    # Batch send for efficiency
    batch_buffer[queue_index].append(message)
    
    if len(batch_buffer[queue_index]) >= 10:
        sqs.send_message_batch(
            QueueUrl=queue_url,
            Entries=batch_buffer[queue_index]
        )
        batch_buffer[queue_index].clear()

# Run multiple producer instances
# Load balance across producers
```

**3. Consumer Layer**
```
Per Queue:
- Auto Scaling Group of EC2 instances
- Or Lambda with reserved concurrency
- Target: Each instance handles 1000-10000 msg/s

For 1M msg/s:
- 100 queues
- 10,000 msg/s per queue
- 10 consumers per queue
- 1000 msg/s per consumer

Total: 1000 consumer instances
```

**4. Implementation**
```python
# High-performance consumer
import concurrent.futures
from threading import Thread

def consumer_worker(queue_url):
    while True:
        # Batch receive
        messages = sqs.receive_message(
            QueueUrl=queue_url,
            MaxNumberOfMessages=10,
            WaitTimeSeconds=1  # Short wait for high throughput
        )
        
        if not messages.get('Messages'):
            continue
        
        # Process in parallel
        with concurrent.futures.ThreadPoolExecutor(max_workers=10) as executor:
            futures = [
                executor.submit(process_message, msg)
                for msg in messages['Messages']
            ]
            concurrent.futures.wait(futures)
        
        # Batch delete
        delete_entries = [
            {'Id': msg['MessageId'], 'ReceiptHandle': msg['ReceiptHandle']}
            for msg in messages['Messages']
        ]
        sqs.delete_message_batch(
            QueueUrl=queue_url,
            Entries=delete_entries
        )

# Run multiple worker threads per instance
for _ in range(NUM_THREADS):
    Thread(target=consumer_worker, args=(queue_url,)).start()
```

**5. Monitoring & Auto Scaling**
```
CloudWatch Metrics per queue:
- ApproximateNumberOfMessagesVisible
- ApproximateAgeOfOldestMessage

Auto Scaling Policy:
- Scale out if queue depth > 10000
- Scale in if queue depth < 1000
- Monitor aggregate across all queues
```

**6. Optimization**
```
- Use Placement Groups for low latency
- Enhanced Networking on EC2
- Process in memory (avoid disk I/O)
- Batch database operations
- Connection pooling
- Async I/O where possible
- Consider Kinesis for true streaming (alternative)
```

**7. Cost Optimization**
```
- Use Spot Instances (fault-tolerant)
- Reserved Instances for baseline
- Batch operations (10 messages = 1 request)
- Long polling (reduces API calls)
- Right-size instances
```

**Trade-offs:**
- More queues = more management overhead
- More consumers = higher cost
- Batch size vs latency balance

**Q20: How do you handle SQS throttling and rate limits?**
A:

**SQS Service Limits:**
- 120,000 in-flight messages per standard queue
- 20,000 in-flight messages per FIFO queue
- API throttling based on account/region

**Strategies:**

**1. Exponential Backoff with Jitter**
```python
import random
import time

def send_with_backoff(message, max_retries=5):
    for attempt in range(max_retries):
        try:
            sqs.send_message(
                QueueUrl=queue_url,
                MessageBody=message
            )
            return  # Success
            
        except ClientError as e:
            if e.response['Error']['Code'] in ['ThrottlingException', 'RequestLimitExceeded']:
                # Exponential backoff with jitter
                base_delay = 2 ** attempt
                jitter = random.uniform(0, 1)
                delay = base_delay + jitter
                
                print(f"Throttled, retrying in {delay:.2f}s")
                time.sleep(delay)
            else:
                raise
    
    raise Exception("Max retries exceeded")
```

**2. Batch Operations**
```python
# Send 10 messages with 1 API call
sqs.send_message_batch(
    QueueUrl=queue_url,
    Entries=[
        {'Id': str(i), 'MessageBody': msg}
        for i, msg in enumerate(messages[:10])
    ]
)
```

**3. Connection Pooling**
```python
# Reuse HTTP connections
import boto3
from botocore.config import Config

config = Config(
    max_pool_connections=50,
    retries={'max_attempts': 3, 'mode': 'adaptive'}
)

sqs = boto3.client('sqs', config=config)
```

**4. Request Service Limit Increase**
```
If consistently hitting limits:
- Request limit increase via AWS Support
- Provide use case justification
- May take a few days
```

**5. Multiple Queues**
```python
# Distribute across multiple queues
def distribute_load(messages):
    queues = [queue1_url, queue2_url, queue3_url]
    
    for i, message in enumerate(messages):
        queue_url = queues[i % len(queues)]
        send_message(queue_url, message)
```

**6. Monitor and Alert**
```
CloudWatch Alarms:
- NumberOfMessagesReceived (sudden drops)
- UserErrors metric (throttling)
- Alert when seeing throttle errors
- Investigate and optimize
```

**7. Rate Limiting at Application**
```python
from ratelimit import limits, sleep_and_retry

@sleep_and_retry
@limits(calls=100, period=1)  # 100 calls per second
def send_message_rate_limited(message):
    return sqs.send_message(
        QueueUrl=queue_url,
        MessageBody=message
    )
```

---

## Summary Cheat Sheet

### Queue Types
- **Standard**: Unlimited throughput, at-least-once, best-effort ordering
- **FIFO**: Guaranteed order, exactly-once, 300-3000 TPS, must end with `.fifo`

### Key Concepts
- **Visibility Timeout**: 30s default, 0-12h range
- **Message Retention**: 4 days default, 1 min-14 days range
- **Message Size**: 256 KB max (2 GB with Extended Client)
- **Long Polling**: Set WaitTimeSeconds = 1-20 (recommended)

### Operations
- **SendMessage**: Add message to queue
- **ReceiveMessage**: Get messages (1-10)
- **DeleteMessage**: Remove after processing
- **ChangeMessageVisibility**: Extend timeout

### Dead Letter Queue
- Move failed messages after maxReceiveCount
- Must be same type (Standard/FIFO)
- Monitor with CloudWatch
- Redrive after fixing issues

### Security
- Encryption at rest (KMS)
- Encryption in transit (HTTPS)
- IAM policies for access control
- Queue policies for cross-account

### Best Practices
1. Use long polling (reduces cost)
2. Batch operations (send/receive/delete 10)
3. Set appropriate visibility timeout
4. Configure DLQ
5. Implement idempotent processing
6. Monitor queue depth and age
7. Scale consumers based on metrics

### Integration
- Lambda: Event source mapping
- SNS: Fan-out pattern
- S3: Event notifications
- Step Functions: Workflow orchestration
- EventBridge: Event routing

### Interview Tips
1. Know Standard vs FIFO differences
2. Understand visibility timeout
3. Explain polling mechanisms
4. Know DLQ configuration
5. Understand at-least-once vs exactly-once
6. Practice architecture design questions
7. Understand scaling patterns
8. Know security best practices
9. Understand cost optimization
10. Be ready for troubleshooting scenarios

---

*Last Updated: 2025*
*Based on AWS Official Documentation and Best Practices*