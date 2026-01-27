# AWS Lambda — Deep Dive for DevOps & Interviews

## 1) What is AWS Lambda?
- Serverless, event-driven compute: you provide code, AWS runs it on demand.
- Pay-per-use: charged for request count and execution time (per ms) based on memory size.
- Scales automatically by creating concurrent execution environments.
- Ideal for event-driven workloads: API calls, file drops, queue/stream processing, scheduled jobs, lightweight ETL, glue logic between services.

## 2) Core execution model
- Lifecycle phases:
  - **Init** (cold start): download code, create runtime, run init code (module load, global scope), run extensions init.
  - **Invoke**: handler is called; execution environment can be reused for subsequent invokes (warm start).
  - **Shutdown**: environment eventually frozen/retired; not guaranteed to persist.
- **Handler signature** example (Python):
```/dev/null/lambda_handler.py#L1-10
import json

def handler(event, context):
    return {
        "statusCode": 200,
        "body": json.dumps({"ok": True})
    }
```
- **Context** provides request ID, function name/version, remaining time, log group/stream.

## 3) Packaging & deployment
- **Artifacts**: ZIP (all languages) or container image (up to 10 GB, OCI compatible).
- **Size limits**: ZIP uncompressed code ≤250 MB; /tmp storage up to 10 GB.
- **Runtimes**: Provided by AWS (e.g., python3.x, nodejs, java, dotnet, ruby, go) or custom runtime via Runtime API.
- **Deployment options**:
  - **AWS SAM** (CloudFormation transform) — quick dev & local testing.
  - **CDK** — infrastructure as code in TypeScript/Python/Java/etc.
  - **Terraform** — `aws_lambda_function`, `aws_lambda_alias`, `aws_cloudwatch_event_rule`, etc.
  - **Serverless Framework** — community tool wrapping CloudFormation.
- **Versioning & aliases**:
  - `PublishVersion` creates immutable numbered versions.
  - **Alias** points to a version; can shift traffic (weighted) for gradual rollouts.
  - CI/CD pattern: deploy new version → create/update alias → shift traffic → promote/rollback.

## 4) Triggers (event sources)
- **API**: API Gateway (REST/HTTP), ALB, Lambda Function URLs.
- **Storage/Data**: S3 event notifications, DynamoDB Streams, Kinesis Data Streams, MSK.
- **Messaging**: SQS (standard/FIFO), SNS, EventBridge (rules, Scheduler).
- **Other**: Step Functions, CloudWatch Logs subscriptions, CodeCommit, Cognito triggers, IoT, Custom apps via direct invoke or EventBridge PutEvents.
- **Invocation models**:
  - **Synchronous**: API Gateway, ALB, Function URL, direct SDK invoke.
  - **Asynchronous**: S3, EventBridge, SNS — Lambda manages retries & DLQ/destinations.
  - **Poll-based**: SQS, Kinesis, DynamoDB Streams — Lambda service polls and batches records.

## 5) Concurrency & scaling
- **Unreserved concurrency**: shared pool per account/region.
- **Reserved concurrency**: cap & guarantee for a function; prevents noisy-neighbor exhaustion.
- **Provisioned concurrency**: pre-warms environments to cut cold starts; billed separately.
- **Burst limits**: region-specific (e.g., up to thousands of concurrent executions burst, then ramp).
- **Throttling**: returns `429` for sync; queues retries for async. Set per-function reserved concurrency to protect downstreams.

## 6) Cold starts & performance
- Factors: runtime (Java/.NET slower), package size, VPC ENI setup, init code heaviness, extensions.
- Mitigations:
  - Use **Provisioned Concurrency** for latency-sensitive paths.
  - Keep dependencies slim; lazy-load clients.
  - Prefer lighter runtimes (Node.js/Python/Go) for latency-critical functions.
  - Keep init work minimal; reuse SDK clients outside handler.
  - For VPC functions, enable **Hyperplane ENI** (default) and keep SGs/Subnets minimal.

## 7) Memory, CPU, and /tmp
- Memory sizes 128 MB–10,240 MB; CPU scales linearly with memory; network throughput scales with memory.
- /tmp ephemeral storage default 512 MB; can request up to 10 GB (billed).
- Choose memory based on performance, not just RAM needs; higher memory ⇒ more CPU ⇒ faster runtime ⇒ potentially lower cost.

## 8) Networking (VPC vs non-VPC)
- **Non-VPC**: outbound internet via AWS-managed networking (fastest cold start).
- **VPC-enabled**: used to reach private resources (RDS, ElastiCache). Lambda creates elastic network interfaces; modern Hyperplane ENI minimizes cold start impact.
- Ensure **NAT** or VPC endpoints for outbound internet when in private subnets.

## 9) IAM & security
- **Execution role** (assumed by Lambda): grants access to AWS services (e.g., S3, SQS, DynamoDB).
- **Resource-based policies**: allow specific principals to invoke the function (e.g., API Gateway, other accounts).
- **Env vars**: encrypted at rest with KMS; avoid storing secrets in plaintext—prefer Secrets Manager/SSM Parameter Store.
- **Code signing**: enforce trusted publishers (optional).
- **Runtime updates**: keep runtimes current; monitor deprecation notices.
- **Least privilege**: scope IAM to exact actions/resources.
- **Network egress control**: VPC + SGs + VPC endpoints; optionally use egress filters.

## 10) Observability
- **Logging**: CloudWatch Logs per invocation (`/aws/lambda/<function>`). Use structured JSON for better search.
- **Metrics**: built-in (Invocations, Errors, Duration, Throttles, IteratorAge, ConcurrentExecutions). Create alarms (e.g., Errors > 0, Throttles > 0).
- **Tracing**: AWS X-Ray; enable active tracing. Wrap handlers with X-Ray SDK for subsegments.
- **Telemetry API & Extensions**: sidecar-like agents for exporting logs/metrics/traces to vendors.
- **Popular patterns**:
  - Correlate `aws_request_id` across logs.
  - Log at `INFO` for high-level, `DEBUG` only in dev; avoid secrets in logs.

## 11) Error handling, retries, DLQs, and destinations
- **Synchronous**: caller receives error; handle upstream (API Gateway → 5xx unless mapped).
- **Asynchronous (e.g., S3, EventBridge, SNS)**:
  - Retries: 2 attempts with backoff.
  - Configure **DLQ** (SQS/SNS) or **Destinations** (onSuccess/onFailure) to capture events.
- **Poll-based**:
  - **SQS**: batches up to 10 (standard) or per FIFO constraints; retries until maxReceiveCount → DLQ (on the queue).
  - **Kinesis/DynamoDB Streams**: retries until success; a poison record blocks the shard. Use **bisect on function error** and smaller batch/window to mitigate.
- **Lambda Destinations**: can send success/failure to SQS/SNS/EventBridge/another Lambda for async invokes.

## 12) CI/CD patterns
- **SAM**: `sam build`, `sam local invoke`, `sam deploy --guided`. Great for templated Lambda + API + EventBridge.
- **CDK**: higher-level constructs; synth to CloudFormation. Supports aliases, provisioned concurrency, canary/linear deployments with **CodeDeploy**.
- **Terraform**: codifies Lambda, IAM, triggers; use `archive_file` for ZIP or ECR for images.
- **Deployment safety**:
  - Use **aliases** with **CodeDeploy** for canary/linear traffic shifting.
  - Bake in alarms to automatically roll back on errors/latency.
  - Keep IaC as source of truth; avoid console drift.

## 13) Testing & local dev
- **Unit**: handler-level with mocks.
- **Integration**: SAM CLI (`sam local invoke/start-api`), LocalStack for AWS-mocking (good but not perfect).
- **Contract**: for APIs, validate request/response schemas.
- **Load/soak**: use realistic payloads; watch concurrency, throttles, downstream limits.

## 14) Common production patterns
- **API backend**: API Gateway/ALB/Function URL → Lambda → DB/other services.
- **Async workers**: SNS/SQS/EventBridge → Lambda → downstream service.
- **Stream processing**: Kinesis/DynamoDB Streams → Lambda → transform/route.
- **Cron**: EventBridge Scheduler/Rules → Lambda.
- **Fan-out**: S3 → EventBridge → multiple Lambdas; SNS topic → multiple Lambdas.
- **Data pipelines**: S3 ingest → Lambda transform → S3/Glue/Athena/Redshift.

## 15) Performance & cost tips
- Right-size memory for speed; test duration vs. memory to find cost/perf sweet spot.
- Use **Provisioned Concurrency** for steady, low-latency paths.
- Keep packages slim; avoid large monolith handlers; split by responsibility.
- Batch wisely for SQS/Kinesis: larger batches reduce cost but increase retry scope.
- Reuse SDK clients; cache config across invokes.
- Prefer **Arm64** for better price/perf if your deps support it.
- Monitor **Duration p95/p99** and **Throttles**.

## 16) Pricing (high level)
- **Requests**: $0.20 per million (first 1M free tier/month).
- **Compute**: per ms based on memory setting and architecture (Arm64 cheaper).
- **Provisioned Concurrency**: additional charge for pre-warmed capacity + execution.
- **Extras**: /tmp >512 MB billed; data transfer, CloudWatch logs, X-Ray sampling.
- Official calculator: https://calculator.aws/#/addService/Lambda

## 17) Security & compliance essentials
- Enforce least privilege on execution role and triggers.
- Use VPC + SGs for private resources; add VPC endpoints to avoid public egress.
- Encrypt env vars; store secrets in Secrets Manager/SSM with IAM-bound access.
- Enable CloudTrail for control-plane auditing; CloudWatch Logs/X-Ray for data-plane visibility.
- Rotate keys/secrets; avoid hardcoding.
- Consider Code Signing if you need a trust chain for artifacts.

## 18) DevOps interview quick-check (Q&A seeds)
- **Cold start causes & mitigations?** Runtimes, package size, VPC ENIs, provisioned concurrency, slim deps, lazy init.
- **Reserved vs. provisioned concurrency?** Reserved = limit/guarantee for a function; Provisioned = pre-warmed environments for low latency.
- **Async vs. poll-based retry semantics?** Async retries twice with backoff; pollers (SQS/Kinesis/DDB) retry until success or DLQ/poison handling.
- **How to do safe deployments?** Versions + aliases + CodeDeploy canary/linear; alarms for rollback.
- **Handle poison messages?** DLQs, smaller batches, bisect on error (Kinesis/DDB), move-to-DLQ on SQS.
- **VPC trade-offs?** Needed for private resources; cold-start impact mitigated by Hyperplane ENI; ensure NAT/endpoints for egress.
- **Observability stack?** CloudWatch Logs/Metrics/Alarms, X-Ray tracing, structured logs, Telemetry API/Extensions.
- **When not to use Lambda?** Long-running tasks, heavy CPU/GPU, large monoliths, high consistent throughput where containers/EC2 cheaper, very low-latency < few ms.

## 19) Minimal SAM template snippet (for reference)
```/dev/null/template.yaml#L1-30
AWSTemplateFormatVersion: '2010-09-09'
Transform: AWS::Serverless-2016-10-31
Resources:
  ApiFunction:
    Type: AWS::Serverless::Function
    Properties:
      Runtime: python3.11
      Handler: app.handler
      CodeUri: src/
      MemorySize: 512
      Timeout: 10
      Architectures: [arm64]
      Events:
        Api:
          Type: Api
          Properties:
            Path: /hello
            Method: get
      Policies:
        - AWSLambdaBasicExecutionRole
```

## 20) References (official, high-signal)
- AWS Lambda Dev Guide: https://docs.aws.amazon.com/lambda/latest/dg/welcome.html
- Event source mappings: https://docs.aws.amazon.com/lambda/latest/dg/invocation-eventsourcemapping.html
- Asynchronous invocation, retries, destinations: https://docs.aws.amazon.com/lambda/latest/dg/invocation-async.html
- Concurrency: https://docs.aws.amazon.com/lambda/latest/dg/configuration-concurrency.html
- Provisioned concurrency: https://docs.aws.amazon.com/lambda/latest/dg/provisioned-concurrency.html
- Security: https://docs.aws.amazon.com/lambda/latest/dg/lambda-security.html
- Pricing: https://aws.amazon.com/lambda/pricing/
- SAM: https://docs.aws.amazon.com/serverless-application-model/latest/developerguide/what-is-sam.html