# AWS Lambda vs Amazon SQS — Event-Driven Roles, Patterns, and Trade-offs

> Lambda is **compute**; SQS is a **managed message queue**. They are complementary parts of event-driven systems.

## 1) Quick definitions
- **AWS Lambda**: Serverless, on-demand code execution. Scales concurrency by spawning execution environments. Pay-per-request + duration. Best for short-lived handlers, API backends, stream/queue processors, cron, and glue logic.
- **Amazon SQS**: Fully managed queue (Standard or FIFO). Durable buffering, decoupling producers/consumers, at-least-once (Standard) or exactly-once + ordering (FIFO). Pay-per-request; no compute.

## 2) Core differences (at a glance)

| Aspect | Lambda | SQS |
| --- | --- | --- |
| Type | Compute (functions) | Messaging (queue) |
| Triggered by | Events/invokes | Producers pushing messages |
| Scaling | Concurrency (unreserved/reserved/provisioned) | Throughput via message ingress; consumers scale separately |
| Ordering | Not guaranteed (unless enforced upstream); Kinesis/DDB can give per-shard ordering | FIFO queues: ordered + exactly-once; Standard: best-effort order, at-least-once |
| Delivery semantics | Depends on trigger (sync/async/poll); handler must be idempotent | Standard: at-least-once; FIFO: exactly-once + ordered |
| Persistence | Ephemeral compute; no persistence | Durable, replicated message storage |
| Back-pressure | Concurrency limits, throttling | Queue length naturally buffers; visibility timeout controls retry |
| Retry behavior | Varies: sync returns error; async retries twice; pollers retry until success or DLQ | Redrive via DLQ; consumer retry driven by visibility timeout / receive count |
| Pricing | Requests + duration (per ms, memory/arch) + optional provisioned concurrency | Per request (api calls), small payload cost; no duration billing |
| Typical use | Business logic, transforms, APIs, cron, stream/queue processing | Decoupling, buffering, smoothing bursts, fan-out/fan-in staging |

## 3) How they work together (common pattern)
```
Producer --> SQS Queue --> Lambda Event Source Mapping (poller) --> Handler --> Downstream (DB/API)
```
- Lambda **polls** SQS (you don’t run your own poller), receives batches, processes, and deletes on success.
- Failures: message becomes visible again; after max receives, SQS can send to DLQ.

### ASCII flow (Standard queue)
```
[Producer] --> [SQS Standard] --(poll/batch)--> [Lambda] --> [Service/DB]
                 ^ retry on fail / visibility timeout ^
                 \-------------- DLQ on poison -------/
```

### ASCII flow (FIFO queue with ordering)
```
[Producer] --> [SQS FIFO (MessageGroupId = key)] --(ordered batches per group)--> [Lambda]
    |                                                                              |
    \----------------- DLQ on exceed maxReceiveCount ------------------------------/
```

## 4) When to choose what
- **Use Lambda** when you need compute: transform, route, validate, enrich, call downstream APIs/DBs, serve APIs.
- **Use SQS** when you need buffering, decoupling, smoothing bursts, protecting downstreams, or enforcing per-key ordering (FIFO).
- **Use both** when you want durable buffering + serverless workers. Lambda concurrency + SQS visibility timeout + DLQ provide back-pressure and resilience.

## 5) Key configuration for Lambda + SQS
- **Batch size**: up to 10 for Standard; up to 10 for FIFO. Tune to balance throughput vs blast radius of a failed batch.
- **Visibility timeout**: must exceed your max Lambda processing time (including retries) to avoid duplicate in-flight.
- **Maximum receives (redrive policy)**: set `maxReceiveCount` to move poison messages to DLQ.
- **Concurrency controls**: set **reserved concurrency** on the function to protect downstreams; SQS poller will respect throttles and retry.
- **Scaling**: Lambda will scale pollers; Standard queues can scale high; FIFO has limited throughput per MessageGroupId (up to 3000 msg/s with batching if multiple groups, otherwise 300 msg/s per group baseline).

## 6) Error handling differences
- **Lambda (async sources)**: 2 retries + optional DLQ/destinations.
- **Lambda (polling SQS)**: retries until success or DLQ via SQS redrive; a failed batch reappears after visibility timeout.
- **SQS DLQ**: configure on the queue; messages exceeding `maxReceiveCount` go to DLQ for inspection.

## 7) Ordering and idempotency
- **Ordering**: Use SQS FIFO + `MessageGroupId` for per-key order; Lambda poller preserves per-group order.
- **Idempotency**: Handlers must be idempotent (at-least-once delivery). Use request IDs, dedup keys, or DynamoDB conditional writes.

## 8) Performance & cost levers
- Lambda: memory ↔ CPU ↔ duration; Arm64 cheaper; provisioned concurrency for low latency; reduce package size and init.
- SQS: cost is per API request and payload size; batch receives to reduce calls; FIFO throughput is lower—plan capacity.

## 9) Security
- **Lambda execution role**: allow `sqs:ReceiveMessage`, `sqs:DeleteMessage`, `sqs:GetQueueAttributes`.
- **SQS resource policy**: allow Lambda service principal to poll if cross-account.
- Encrypt SQS with KMS; avoid secrets in messages; use VPC endpoints if needed for private routing.

## 10) Typical interview angles
- Explain Lambda vs SQS roles (compute vs queue) and how they complement.
- How Lambda polls SQS, batch size, visibility timeout, DLQ, and poison-pill handling.
- Ordering with FIFO + MessageGroupId; throughput limits; why idempotency matters.
- Back-pressure: reserved concurrency on Lambda; buffering in SQS.
- Failure flows: what happens on handler exception; how messages get retried and eventually land in DLQ.
- Cost levers: Lambda duration/memory vs SQS request batching.

## 11) Mini patterns
- **Fan-in / Buffer**: Multiple producers → SQS → Lambda → DB (smooth bursts).
- **Per-tenant ordering**: FIFO + MessageGroupId = tenant; Lambda processes per-tenant in order.
- **DLQ triage**: SQS → Lambda; failures → SQS DLQ → separate “DLQ consumer” Lambda for alerting/repair.

## 12) Quick snippets (pseudocode)
- Handler idempotency (Python):
```
# idempotent write keyed by event_id
put_item(
  Item={"id": event_id, "payload": body},
  ConditionExpression="attribute_not_exists(id)"
)
```
- Safe batch delete (Node.js):
```
const entries = records.map(r => ({ Id: r.messageId, ReceiptHandle: r.receiptHandle }));
await sqs.deleteMessageBatch({ QueueUrl, Entries: entries }).promise();
```

## 13) Reference diagrams (text-only)
- Standard (at-least-once, unordered):
```
[Producers] -> [SQS Standard Queue] ->(batch)-> [Lambda] -> [Downstream]
                     |                              ^
                     |--(maxReceiveCount exceeded)->| DLQ
```
- FIFO (ordered, exactly-once):
```
[Producers] --(MessageGroupId)-> [SQS FIFO Queue] ->(ordered batch)-> [Lambda]
                                  |                               |
                                  |--(DLQ on exceed receive count)-|
```

## 14) Official references
- Lambda + SQS: https://docs.aws.amazon.com/lambda/latest/dg/with-sqs.html
- SQS dev guide: https://docs.aws.amazon.com/AWSSimpleQueueService/latest/SQSDeveloperGuide/welcome.html
- Lambda event source mappings: https://docs.aws.amazon.com/lambda/latest/dg/invocation-eventsourcemapping.html
- Lambda error handling (async): https://docs.aws.amazon.com/lambda/latest/dg/invocation-async.html
- SQS quotas: https://docs.aws.amazon.com/AWSSimpleQueueService/latest/SQSDeveloperGuide/sqs-quotas.html