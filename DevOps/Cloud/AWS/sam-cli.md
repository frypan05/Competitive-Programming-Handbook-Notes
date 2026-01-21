# AWS SAM CLI (Serverless Application Model) - Complete Guide

## Table of Contents
1. [Introduction](#introduction)
2. [Core Concepts](#core-concepts)
3. [Installation and Setup](#installation-and-setup)
4. [SAM Template Anatomy](#sam-template-anatomy)
5. [SAM CLI Commands](#sam-cli-commands)
6. [Local Development and Testing](#local-development-and-testing)
7. [Building and Packaging](#building-and-packaging)
8. [Deployment](#deployment)
9. [CI/CD Integration](#cicd-integration)
10. [Advanced Features](#advanced-features)
11. [Best Practices](#best-practices)
12. [Common Interview Questions](#common-interview-questions)

---

## Introduction

**AWS SAM (Serverless Application Model)** is an open-source framework for building serverless applications on AWS. It provides shorthand syntax to express functions, APIs, databases, and event source mappings.

### What is SAM?

SAM consists of two main components:
1. **SAM Template Specification**: Extension of CloudFormation with serverless-focused syntax
2. **SAM CLI**: Command-line tool for local development, testing, and deployment

### Key Features
- **Simplified Syntax**: Define serverless resources with less code
- **Local Testing**: Run Lambda functions and APIs locally
- **Built-in Best Practices**: Security, deployment preferences
- **Single Deployment**: All resources in one stack
- **Extension of CloudFormation**: Full CFN capabilities available
- **Open Source**: Community-driven development

### Why SAM?
- Faster development cycle
- Local debugging and testing
- Gradual deployments (canary, linear)
- Built-in API Gateway and DynamoDB support
- Infrastructure as Code (IaC)
- No additional cost (only AWS resources)

### SAM vs Other Tools

| Feature | SAM | Serverless Framework | Terraform | CDK |
|---------|-----|---------------------|-----------|-----|
| Focus | AWS Serverless | Multi-cloud | Multi-cloud | AWS |
| Language | YAML/JSON | YAML | HCL | TypeScript/Python/Java |
| Local Testing | Built-in | Plugins | Limited | Limited |
| CloudFormation | Native | Generates | No | Generates |
| Learning Curve | Low | Medium | Medium | Higher |
| Deployment | CloudFormation | Custom | State-based | CloudFormation |

---

## Core Concepts

### 1. **SAM Template**
YAML or JSON file defining serverless application
- Starts with `Transform: AWS::Serverless-2016-10-31`
- Simplified resource definitions
- Transformed to CloudFormation during deployment

**Basic Structure:**
```yaml
AWSTemplateFormatVersion: '2010-09-09'
Transform: AWS::Serverless-2016-10-31
Description: My serverless application

Globals:
  Function:
    Timeout: 30
    Runtime: python3.9

Parameters:
  Environment:
    Type: String
    Default: dev

Resources:
  MyFunction:
    Type: AWS::Serverless::Function
    Properties:
      Handler: app.lambda_handler
      Runtime: python3.9
      Events:
        Api:
          Type: Api
          Properties:
            Path: /hello
            Method: get

Outputs:
  ApiUrl:
    Description: API Gateway endpoint URL
    Value: !Sub https://${ServerlessRestApi}.execute-api.${AWS::Region}.amazonaws.com/Prod/hello
```

### 2. **SAM Resource Types**

**AWS::Serverless::Function**
- Lambda function with automatic IAM role
- Event source mappings
- Environment variables
- Layers

**AWS::Serverless::Api**
- API Gateway REST API
- CORS configuration
- Authorization
- Usage plans

**AWS::Serverless::HttpApi**
- API Gateway HTTP API (v2)
- Lower cost, lower latency
- JWT authorization

**AWS::Serverless::SimpleTable**
- DynamoDB table with single attribute primary key
- Simplified syntax

**AWS::Serverless::Application**
- Nested application from SAR (Serverless Application Repository)
- Reusable components

**AWS::Serverless::LayerVersion**
- Lambda Layer
- Shared code and dependencies

**AWS::Serverless::StateMachine**
- Step Functions state machine
- Workflow orchestration

### 3. **Globals Section**
Define common properties once, apply to all resources
```yaml
Globals:
  Function:
    Runtime: python3.9
    Timeout: 30
    MemorySize: 256
    Environment:
      Variables:
        TABLE_NAME: !Ref MyTable
    Tracing: Active
  Api:
    Cors:
      AllowOrigin: "'*'"
      AllowHeaders: "'*'"
```

### 4. **Events**
Trigger sources for Lambda functions
- **Api**: API Gateway REST API
- **HttpApi**: API Gateway HTTP API
- **S3**: S3 bucket events
- **SNS**: SNS topic
- **SQS**: SQS queue
- **DynamoDB**: DynamoDB streams
- **Kinesis**: Kinesis streams
- **CloudWatchEvent**: EventBridge rules
- **Schedule**: Cron/rate expressions
- **CloudWatchLogs**: Log group subscriptions

### 5. **Policies**
Pre-defined IAM policy templates
```yaml
Policies:
  - DynamoDBCrudPolicy:
      TableName: !Ref MyTable
  - S3ReadPolicy:
      BucketName: my-bucket
  - SQSPollerPolicy:
      QueueName: my-queue
```

---

## Installation and Setup

### Installation

**Prerequisites:**
- AWS CLI configured
- Docker installed (for local testing)
- Python 3.7+ or Homebrew (macOS)

**Installation Methods:**

**1. Using pip (Cross-platform):**
```bash
pip install aws-sam-cli
sam --version
```

**2. Using Homebrew (macOS):**
```bash
brew tap aws/tap
brew install aws-sam-cli
sam --version
```

**3. Using MSI installer (Windows):**
Download from: https://github.com/aws/aws-sam-cli/releases

**4. Using binary (Linux):**
```bash
wget https://github.com/aws/aws-sam-cli/releases/latest/download/aws-sam-cli-linux-x86_64.zip
unzip aws-sam-cli-linux-x86_64.zip -d sam-installation
sudo ./sam-installation/install
sam --version
```

### Initial Configuration

**1. Configure AWS Credentials:**
```bash
aws configure
# Enter Access Key ID
# Enter Secret Access Key
# Enter Default region
# Enter Default output format
```

**2. Verify Installation:**
```bash
sam --version
aws sts get-caller-identity
docker --version
```

**3. Set Up Editor:**
- Install AWS Toolkit for VS Code
- YAML syntax highlighting
- SAM template validation

---

## SAM Template Anatomy

### Complete Example

```yaml
AWSTemplateFormatVersion: '2010-09-09'
Transform: AWS::Serverless-2016-10-31
Description: Complete serverless application example

# Global configuration
Globals:
  Function:
    Runtime: python3.9
    Timeout: 30
    MemorySize: 256
    Environment:
      Variables:
        STAGE: !Ref Environment
        LOG_LEVEL: INFO
    Tracing: Active
    Tags:
      Project: MyApp
      Environment: !Ref Environment

# Input parameters
Parameters:
  Environment:
    Type: String
    Default: dev
    AllowedValues:
      - dev
      - staging
      - prod

# Conditions
Conditions:
  IsProduction: !Equals [!Ref Environment, prod]

# Resources
Resources:
  # Lambda Function
  MyFunction:
    Type: AWS::Serverless::Function
    Properties:
      CodeUri: src/
      Handler: app.lambda_handler
      Runtime: python3.9
      Description: Main application function
      MemorySize: 512
      Timeout: 60
      Environment:
        Variables:
          TABLE_NAME: !Ref MyTable
          QUEUE_URL: !Ref MyQueue
      Policies:
        - DynamoDBCrudPolicy:
            TableName: !Ref MyTable
        - SQSSendMessagePolicy:
            QueueName: !GetAtt MyQueue.QueueName
      Events:
        # API Gateway REST API
        GetApi:
          Type: Api
          Properties:
            Path: /items
            Method: get
            RestApiId: !Ref MyApi
        PostApi:
          Type: Api
          Properties:
            Path: /items
            Method: post
            RestApiId: !Ref MyApi
        # SQS Queue trigger
        QueueEvent:
          Type: SQS
          Properties:
            Queue: !GetAtt MyQueue.Arn
            BatchSize: 10
        # Scheduled event (cron)
        ScheduledEvent:
          Type: Schedule
          Properties:
            Schedule: cron(0 12 * * ? *)
            Description: Daily at noon UTC

  # API Gateway
  MyApi:
    Type: AWS::Serverless::Api
    Properties:
      StageName: !Ref Environment
      Auth:
        DefaultAuthorizer: MyCognitoAuthorizer
        Authorizers:
          MyCognitoAuthorizer:
            UserPoolArn: !GetAtt MyCognitoUserPool.Arn
      Cors:
        AllowOrigin: "'*'"
        AllowHeaders: "'Content-Type,Authorization'"
        AllowMethods: "'GET,POST,PUT,DELETE'"
      GatewayResponses:
        DEFAULT_4XX:
          ResponseParameters:
            Headers:
              Access-Control-Allow-Origin: "'*'"

  # HTTP API (simpler, cheaper)
  MyHttpApi:
    Type: AWS::Serverless::HttpApi
    Properties:
      CorsConfiguration:
        AllowOrigins:
          - "*"
        AllowMethods:
          - GET
          - POST
        AllowHeaders:
          - Content-Type

  # DynamoDB Table
  MyTable:
    Type: AWS::Serverless::SimpleTable
    Properties:
      TableName: !Sub ${Environment}-items-table
      PrimaryKey:
        Name: id
        Type: String
      ProvisionedThroughput:
        ReadCapacityUnits: 5
        WriteCapacityUnits: 5
      SSESpecification:
        SSEEnabled: true
      Tags:
        Environment: !Ref Environment

  # SQS Queue
  MyQueue:
    Type: AWS::SQS::Queue
    Properties:
      QueueName: !Sub ${Environment}-items-queue
      VisibilityTimeout: 300
      MessageRetentionPeriod: 1209600
      RedrivePolicy:
        deadLetterTargetArn: !GetAtt MyDLQ.Arn
        maxReceiveCount: 3

  # Dead Letter Queue
  MyDLQ:
    Type: AWS::SQS::Queue
    Properties:
      QueueName: !Sub ${Environment}-items-dlq
      MessageRetentionPeriod: 1209600

  # Lambda Layer
  MyLayer:
    Type: AWS::Serverless::LayerVersion
    Properties:
      LayerName: common-dependencies
      Description: Shared dependencies
      ContentUri: layers/
      CompatibleRuntimes:
        - python3.9
      RetentionPolicy: Retain

  # Step Functions State Machine
  MyStateMachine:
    Type: AWS::Serverless::StateMachine
    Properties:
      DefinitionUri: statemachine/workflow.asl.json
      Role: !GetAtt StateMachineRole.Arn
      Events:
        StartExecution:
          Type: Api
          Properties:
            Path: /workflow
            Method: post

  # Cognito User Pool
  MyCognitoUserPool:
    Type: AWS::Cognito::UserPool
    Properties:
      UserPoolName: !Sub ${Environment}-user-pool
      AutoVerifiedAttributes:
        - email
      Policies:
        PasswordPolicy:
          MinimumLength: 8
          RequireUppercase: true
          RequireLowercase: true
          RequireNumbers: true

  # IAM Role for State Machine
  StateMachineRole:
    Type: AWS::IAM::Role
    Properties:
      AssumeRolePolicyDocument:
        Version: '2012-10-17'
        Statement:
          - Effect: Allow
            Principal:
              Service: states.amazonaws.com
            Action: sts:AssumeRole
      Policies:
        - PolicyName: InvokeLambda
          PolicyDocument:
            Version: '2012-10-17'
            Statement:
              - Effect: Allow
                Action:
                  - lambda:InvokeFunction
                Resource: !GetAtt MyFunction.Arn

# Outputs
Outputs:
  ApiUrl:
    Description: API Gateway endpoint URL
    Value: !Sub https://${MyApi}.execute-api.${AWS::Region}.amazonaws.com/${Environment}
    Export:
      Name: !Sub ${AWS::StackName}-ApiUrl

  HttpApiUrl:
    Description: HTTP API endpoint URL
    Value: !Sub https://${MyHttpApi}.execute-api.${AWS::Region}.amazonaws.com

  FunctionArn:
    Description: Lambda Function ARN
    Value: !GetAtt MyFunction.Arn
    Export:
      Name: !Sub ${AWS::StackName}-FunctionArn

  TableName:
    Description: DynamoDB table name
    Value: !Ref MyTable

  QueueUrl:
    Description: SQS Queue URL
    Value: !Ref MyQueue
```

### Policy Templates

**Common SAM Policy Templates:**
```yaml
Policies:
  # DynamoDB
  - DynamoDBCrudPolicy:
      TableName: !Ref MyTable
  - DynamoDBReadPolicy:
      TableName: !Ref MyTable
  - DynamoDBWritePolicy:
      TableName: !Ref MyTable

  # S3
  - S3ReadPolicy:
      BucketName: my-bucket
  - S3WritePolicy:
      BucketName: my-bucket
  - S3CrudPolicy:
      BucketName: my-bucket

  # SQS
  - SQSPollerPolicy:
      QueueName: !GetAtt MyQueue.QueueName
  - SQSSendMessagePolicy:
      QueueName: !GetAtt MyQueue.QueueName

  # SNS
  - SNSPublishMessagePolicy:
      TopicName: !GetAtt MyTopic.TopicName

  # Secrets Manager
  - AWSSecretsManagerGetSecretValuePolicy:
      SecretArn: !Ref MySecret

  # CloudWatch Logs
  - CloudWatchPutMetricPolicy: {}

  # Step Functions
  - StepFunctionsExecutionPolicy:
      StateMachineName: !GetAtt MyStateMachine.Name

  # Custom IAM Policy
  - Statement:
      - Effect: Allow
        Action:
          - s3:GetObject
        Resource: !Sub arn:aws:s3:::my-bucket/*
```

---

## SAM CLI Commands

### 1. **sam init**
Initialize new SAM project

**Interactive mode:**
```bash
sam init
# Choose template
# Choose runtime
# Choose project name
```

**Quick start templates:**
```bash
# Hello World
sam init --runtime python3.9 --name my-app --app-template hello-world

# API Gateway + Lambda
sam init --runtime nodejs18.x --dependency-manager npm --app-template quick-start-web

# Step Functions
sam init --runtime python3.9 --app-template quick-start-step-functions

# All templates
sam init --list-templates
```

**From custom location:**
```bash
sam init --location gh:aws-samples/cookiecutter-aws-sam-python
sam init --location git+ssh://git@github.com/user/repo.git
```

### 2. **sam validate**
Validate SAM template

```bash
# Validate template syntax
sam validate

# Validate specific template
sam validate --template template.yaml

# Lint (additional checks)
sam validate --lint
```

### 3. **sam build**
Build serverless application

**Basic build:**
```bash
sam build
```

**Build specific function:**
```bash
sam build MyFunction
```

**Build options:**
```bash
# Use container (consistent environment)
sam build --use-container

# Specific base image
sam build --use-container --build-image public.ecr.aws/sam/build-python3.9

# Parallel builds
sam build --parallel

# Debug mode
sam build --debug

# Skip pull docker image
sam build --use-container --skip-pull-image

# Build in specific directory
sam build --build-dir .aws-sam/build
```

**Build with dependencies:**
```bash
# Python (requirements.txt)
sam build

# Node.js (package.json)
sam build

# Java (pom.xml or build.gradle)
sam build

# Go (go.mod)
sam build
```

### 4. **sam local**
Test functions locally

**Invoke function:**
```bash
# Invoke with no event
sam local invoke MyFunction

# Invoke with event file
sam local invoke MyFunction -e events/event.json

# Invoke with stdin
echo '{"key": "value"}' | sam local invoke MyFunction

# With environment variables
sam local invoke MyFunction --env-vars env.json

# Debug mode (port 5858)
sam local invoke MyFunction -d 5858

# Docker network
sam local invoke MyFunction --docker-network my-network
```

**Start API locally:**
```bash
# Start API Gateway locally
sam local start-api

# Specific port
sam local start-api --port 3000

# Specific host
sam local start-api --host 0.0.0.0

# With environment variables
sam local start-api --env-vars env.json

# Warm containers (faster subsequent calls)
sam local start-api --warm-containers EAGER

# Debug mode
sam local start-api -d 5858
```

**Start Lambda endpoint:**
```bash
# Start Lambda runtime interface emulator
sam local start-lambda

# Invoke via AWS CLI
aws lambda invoke --function-name MyFunction \
  --endpoint-url http://127.0.0.1:3001 \
  --no-verify-ssl \
  output.json
```

**Generate sample event:**
```bash
# S3 event
sam local generate-event s3 put

# API Gateway event
sam local generate-event apigateway aws-proxy

# SQS event
sam local generate-event sqs receive-message

# DynamoDB event
sam local generate-event dynamodb update

# CloudWatch Logs
sam local generate-event cloudwatch logs

# All available events
sam local generate-event --help
```

### 5. **sam deploy**
Deploy application to AWS

**Guided deployment (first time):**
```bash
sam deploy --guided

# Prompts:
# - Stack Name
# - AWS Region
# - Confirm changes before deploy
# - Allow SAM CLI IAM role creation
# - Save arguments to samconfig.toml
```

**Subsequent deployments:**
```bash
# Uses samconfig.toml
sam deploy

# Override stack name
sam deploy --stack-name my-stack

# Override region
sam deploy --region us-west-2

# Override parameters
sam deploy --parameter-overrides Environment=prod

# Without confirmation
sam deploy --no-confirm-changeset

# With capabilities
sam deploy --capabilities CAPABILITY_IAM CAPABILITY_AUTO_EXPAND

# Specific profile
sam deploy --profile production
```

**Advanced deployment:**
```bash
# Use existing S3 bucket
sam deploy --s3-bucket my-deployment-bucket

# Add tags
sam deploy --tags "Project=MyApp Environment=Prod"

# Disable rollback
sam deploy --no-disable-rollback

# Force upload
sam deploy --force-upload

# Resolve S3 and resolve image repos
sam deploy --resolve-s3 --resolve-image-repos
```

### 6. **sam package**
Package and upload artifacts to S3

```bash
# Package application
sam package \
  --template-file template.yaml \
  --output-template-file packaged.yaml \
  --s3-bucket my-deployment-bucket

# With prefix
sam package \
  --s3-bucket my-bucket \
  --s3-prefix my-app/deployments

# Use profile
sam package --profile production
```

### 7. **sam logs**
Fetch logs from CloudWatch

```bash
# Tail logs for function
sam logs -n MyFunction --stack-name my-stack --tail

# Fetch logs from specific time
sam logs -n MyFunction --stack-name my-stack --start-time '10min ago'

# Filter pattern
sam logs -n MyFunction --stack-name my-stack --filter ERROR

# Specific time range
sam logs -n MyFunction --stack-name my-stack \
  --start-time '2024-01-01T00:00:00' \
  --end-time '2024-01-01T23:59:59'

# Include traces
sam logs -n MyFunction --stack-name my-stack --include-traces
```

### 8. **sam sync**
Automatically sync local changes to AWS

```bash
# Sync code changes (fast)
sam sync --watch

# Sync infrastructure changes (slower)
sam sync --watch --code false

# Sync to specific stack
sam sync --stack-name my-stack --watch

# Sync and tail logs
sam sync --watch --logs
```

### 9. **sam delete**
Delete deployed application

```bash
# Delete stack (interactive)
sam delete

# Delete specific stack
sam delete --stack-name my-stack

# Skip confirmation
sam delete --no-prompts

# Retain S3 bucket
sam delete --no-prompts --s3-bucket my-bucket
```

### 10. **sam list**
List resources and endpoints

```bash
# List stack resources
sam list stack-outputs --stack-name my-stack

# List endpoints
sam list endpoints --stack-name my-stack --output json

# List resources
sam list resources --stack-name my-stack
```

---

## Local Development and Testing

### Environment Variables

**env.json:**
```json
{
  "MyFunction": {
    "TABLE_NAME": "local-table",
    "API_KEY": "test-key-123",
    "LOG_LEVEL": "DEBUG"
  }
}
```

**Usage:**
```bash
sam local invoke MyFunction --env-vars env.json
sam local start-api --env-vars env.json
```

### Event Examples

**API Gateway event (events/api-get.json):**
```json
{
  "httpMethod": "GET",
  "path": "/items",
  "queryStringParameters": {
    "id": "123"
  },
  "headers": {
    "Content-Type": "application/json"
  },
  "body": null
}
```

**S3 event (events/s3-put.json):**
```json
{
  "Records": [
    {
      "eventName": "ObjectCreated:Put",
      "s3": {
        "bucket": {
          "name": "my-bucket"
        },
        "object": {
          "key": "uploads/file.jpg"
        }
      }
    }
  ]
}
```

**SQS event (events/sqs.json):**
```json
{
  "Records": [
    {
      "messageId": "123",
      "body": "{\"order_id\": \"12345\"}",
      "attributes": {
        "ApproximateReceiveCount": "1"
      }
    }
  ]
}
```

### Debugging

**VS Code launch.json:**
```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "SAM CLI - Debug",
      "type": "python",
      "request": "attach",
      "port": 5858,
      "host": "localhost",
      "pathMappings": [
        {
          "localRoot": "${workspaceFolder}/src",
          "remoteRoot": "/var/task"
        }
      ]
    }
  ]
}
```

**Start debugging:**
```bash
# Python
sam local invoke MyFunction -e events/event.json -d 5858

# Node.js
sam local invoke MyFunction -e events/event.json -d 9229 --debugger-path /usr/local/lib/node_modules/node-inspector

# In VS Code: Run > Start Debugging
```

### Local DynamoDB

**Using Docker:**
```bash
# Start local DynamoDB
docker run -p 8000:8000 amazon/dynamodb-local

# Create table
aws dynamodb create-table \
  --table-name local-table \
  --attribute-definitions AttributeName=id,AttributeType=S \
  --key-schema AttributeName=id,KeyType=HASH \
  --provisioned-throughput ReadCapacityUnits=5,WriteCapacityUnits=5 \
  --endpoint-url http://localhost:8000

# Connect Lambda to local DynamoDB
sam local invoke MyFunction --docker-network host
```

---

## Building and Packaging

### Build Process

**Dependencies:**

**Python (requirements.txt):**
```txt
boto3==1.28.0
requests==2.31.0
pydantic==2.0.0
```

**Node.js (package.json):**
```json
{
  "name": "my-function",
  "version": "1.0.0",
  "dependencies": {
    "aws-sdk": "^2.1400.0",
    "axios": "^1.4.0"
  }
}
```

**Build commands:**
```bash
# Build all functions
sam build

# Build with Docker (consistent environment)
sam build --use-container

# Build for specific runtime
sam build --use-container --build-image public.ecr.aws/sam/build-python3.9
```

### Lambda Layers

**Layer structure:**
```
layers/
└── my-layer/
    ├── python/          # For Python
    │   └── lib/
    └── nodejs/          # For Node.js
        └── node_modules/
```

**Template:**
```yaml
MyLayer:
  Type: AWS::Serverless::LayerVersion
  Properties:
    LayerName: my-dependencies
    Description: Shared dependencies
    ContentUri: layers/my-layer/
    CompatibleRuntimes:
      - python3.9
      - python3.10
    RetentionPolicy: Retain

MyFunction:
  Type: AWS::Serverless::Function
  Properties:
    Layers:
      - !Ref MyLayer
```

### Container Images

**Dockerfile:**
```dockerfile
FROM public.ecr.aws/lambda/python:3.9

# Copy function code
COPY app.py requirements.txt ./

# Install dependencies
RUN pip install -r requirements.txt

# Set handler
CMD ["app.lambda_handler"]
```

**Template:**
```yaml
MyFunction:
  Type: AWS::Serverless::Function
  Properties:
    PackageType: Image
    ImageUri: my-function:latest
    ImageConfig:
      Command: ["app.lambda_handler"]
  Metadata:
    Dockerfile: Dockerfile
    DockerContext: ./src
    DockerTag: latest
```

**Build and deploy:**
```bash
# Build container
sam build

# Deploy (creates ECR repo automatically)
sam deploy --guided --resolve-image-repos
```

---

## Deployment

### Configuration File (samconfig.toml)

**Generated by `sam deploy --guided`:**
```toml
version = 0.1

[default.deploy.parameters]
stack_name = "my-app"
s3_bucket = "aws-sam-cli-managed-default-samclisourcebucket"
s3_prefix = "my-app"
region = "us-east-1"
confirm_changeset = true
capabilities = "CAPABILITY_IAM"
parameter_overrides = "Environment=\"dev\""
image_repositories = []

[production.deploy.parameters]
stack_name = "my-app-prod"
s3_bucket = "my-production-bucket"
region = "us-east-1"
confirm_changeset = false
capabilities = "CAPABILITY_IAM"
parameter_overrides = "Environment=\"prod\""
```

**Use specific configuration:**
```bash
# Use default
sam deploy

# Use production config
sam deploy --config-env production
```

### Deployment Preferences

**Gradual deployments:**
```yaml
MyFunction:
  Type: AWS::Serverless::Function
  Properties:
    AutoPublishAlias: live
    DeploymentPreference:
      Type: Canary10Percent5Minutes
      Alarms:
        - !Ref ErrorAlarm
      Hooks:
        PreTraffic: !Ref PreTrafficHook
        PostTraffic: !Ref PostTrafficHook

ErrorAlarm:
  Type: AWS::CloudWatch::Alarm
  Properties:
    AlarmName: !Sub ${AWS::StackName}-errors
    MetricName: Errors
    Namespace: AWS/Lambda
    Statistic: Sum
    Period: 60
    EvaluationPeriods: 1
    Threshold: 1
    ComparisonOperator: GreaterThanThreshold
    Dimensions:
      - Name: FunctionName
        Value: !Ref MyFunction
```

**Deployment types:**
- **Canary10Percent30Minutes**: 10% for 30 min, then 100%
- **Canary10Percent5Minutes**: 10% for 5 min, then 100%
- **Linear10PercentEvery10Minutes**: +10% every 10 min
- **Linear10PercentEvery1Minute**: +10% every 1 min
- **AllAtOnce**: Immediate 100%

### Multi-Environment Deployment

**Parameters per environment:**
```bash
# Development
sam deploy \
  --stack-name my-app-dev \
  --parameter-overrides Environment=dev MemorySize=512

# Staging
sam deploy \
  --stack-name my-app-staging \
  --parameter-overrides Environment=staging MemorySize=1024

# Production
sam deploy \
  --stack-name my-app-prod \
  --parameter-overrides Environment=prod MemorySize=2048 \
  --no-confirm-changeset
```

---

## CI/CD Integration

### GitHub Actions

**.github/workflows/deploy.yml:**
```yaml
name: Deploy SAM Application

on:
  push:
    branches:
      - main

jobs:
  deploy:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Setup Python
        uses: actions/setup-python@v4
        with:
          python-version: '3.9'
      
      - name: Setup SAM CLI
        uses: aws-actions/setup-sam@v2
      
      - name: Configure AWS Credentials
        uses: aws-actions/configure-aws-credentials@v2
        with:
          aws-access-key-id: ${{ secrets.AWS_ACCESS_KEY_ID }}
          aws-secret-access-key: ${{ secrets.AWS_SECRET_ACCESS_KEY }}
          aws-region: us-east-1
      
      - name: Build
        run: sam build --use-container
      
      - name: Deploy
        run: |
          sam deploy \
            --no-confirm-changeset \
            --no-fail-on-empty-changeset \
            --stack-name my-app-${{ github.ref_name }} \
            --capabilities CAPABILITY_IAM \
            --parameter-overrides Environment=${{ github.ref_name }}
```

### AWS CodePipeline

**buildspec.yml:**
```yaml
version: 0.2

phases:
  install:
    runtime-versions:
      python: 3.9
    commands:
      - pip install aws-sam-cli
  
  build:
    commands:
      - sam build --use-container
      - sam package --s3-bucket $ARTIFACT_BUCKET --output-template-file packaged.yaml

artifacts:
  files:
    - packaged.yaml
    - samconfig.toml
```

**pipeline.yaml:**
```yaml
Resources:
  Pipeline:
    Type: AWS::CodePipeline::Pipeline
    Properties:
      Stages:
        - Name: Source
          Actions:
            - Name: SourceCode
              ActionTypeId:
                Category: Source
                Owner: AWS
                Provider: CodeCommit
                Version: 1
              Configuration:
                RepositoryName: my-repo
                BranchName: main
        
        - Name: Build
          Actions:
            - Name: BuildAndPackage
              ActionTypeId:
                Category: Build
                Owner: AWS
                Provider: CodeBuild
                Version: 1
              Configuration:
                ProjectName: !Ref CodeBuildProject
        
        - Name: Deploy
          Actions:
            - Name: CreateChangeSet
              ActionTypeId:
                Category: Deploy
                Owner: AWS
                Provider: CloudFormation
                Version: 1
              Configuration:
                ActionMode: CHANGE_SET_REPLACE
                StackName: my-app
                ChangeSetName: my-changeset
                TemplatePath: BuildArtifact::packaged.yaml
                Capabilities: CAPABILITY_IAM
            
            - Name: ExecuteChangeSet
              ActionTypeId:
                Category: Deploy
                Owner: AWS
                Provider: CloudFormation
                Version: 1
              Configuration:
                ActionMode: CHANGE_SET_EXECUTE
                StackName: my-app
                ChangeSetName: my-changeset
```

### GitLab CI

**.gitlab-ci.yml:**
```yaml
image: python:3.9

stages:
  - build
  - test
  - deploy

before_script:
  - pip install aws-sam-cli
  - aws configure set aws_access_key_id $AWS_ACCESS_KEY_ID
  - aws configure set aws_secret_access_key $AWS_SECRET_ACCESS_KEY
  - aws configure set region us-east-1

build:
  stage: build
  script:
    - sam build --use-container
  artifacts:
    paths:
      - .aws-sam/

test:
  stage: test
  script:
    - sam local invoke MyFunction -e events/test.json

deploy_dev:
  stage: deploy
  script:
    - sam deploy --no-confirm-changeset --stack-name my-app-dev
  only:
    - develop

deploy_prod:
  stage: deploy
  script:
    - sam deploy --no-confirm-changeset --stack-name my-app-prod
  only:
    - main
  when: manual
```

---

## Advanced Features

### Nested Applications

**Using SAR (Serverless Application Repository):**
```yaml
Resources:
  MonitoringApp:
    Type: AWS::Serverless::Application
    Properties:
      Location:
        ApplicationId: arn:aws:serverlessrepo:us-east-1:123456789012:applications/monitoring
        SemanticVersion: 1.0.0
      Parameters:
        AlertEmail: admin@example.com
```

**Nested stacks:**
```yaml
Resources:
  DatabaseStack:
    Type: AWS::Serverless::Application
    Properties:
      Location: ./database/template.yaml
      Parameters:
        Environment: !Ref Environment
```

### Custom Domains

**Using Route53 and ACM:**
```yaml
Resources:
  MyApi:
    Type: AWS::Serverless::Api
    Properties:
      StageName: prod
      Domain:
        DomainName: api.example.com
        CertificateArn: !Ref Certificate
        Route53:
          HostedZoneId: !Ref HostedZone

  Certificate:
    Type: AWS::CertificateManager::Certificate
    Properties:
      DomainName: api.example.com
      ValidationMethod: DNS
```

### API Gateway Authorization

**Cognito authorizer:**
```yaml
MyApi:
  Type: AWS::Serverless::Api
  Properties:
    Auth:
      DefaultAuthorizer: MyCognitoAuth
      Authorizers:
        MyCognitoAuth:
          UserPoolArn: !GetAtt UserPool.Arn
          Identity:
            Header: Authorization

MyFunction:
  Type: AWS::Serverless::Function
  Properties:
    Events:
      SecureApi:
        Type: Api
        Properties:
          Path: /secure
          Method: get
          RestApiId: !Ref MyApi
          Auth:
            Authorizer: MyCognitoAuth
```

**Lambda authorizer:**
```yaml
MyApi:
  Type: AWS::Serverless::Api
  Properties:
    Auth:
      DefaultAuthorizer: MyLambdaAuth
      Authorizers:
        MyLambdaAuth:
          FunctionArn: !GetAtt AuthorizerFunction.Arn
          FunctionPayloadType: REQUEST
          Identity:
            Headers:
              - Authorization
```

### X-Ray Tracing

```yaml
Globals:
  Function:
    Tracing: Active

Resources:
  MyFunction:
    Type: AWS::Serverless::Function
    Properties:
      Policies:
        - AWSXRayDaemonWriteAccess
```

**In code (Python):**
```python
from aws_xray_sdk.core import xray_recorder
from aws_xray_sdk.core import patch_all

patch_all()

@xray_recorder.capture('process_request')
def lambda_handler(event, context):
    # Your code here
    pass
```

### VPC Configuration

```yaml
MyFunction:
  Type: AWS::Serverless::Function
  Properties:
    VpcConfig:
      SecurityGroupIds:
        - !Ref LambdaSecurityGroup
      SubnetIds:
        - !Ref PrivateSubnet1
        - !Ref PrivateSubnet2
```

### Reserved Concurrency

```yaml
MyFunction:
  Type: AWS::Serverless::Function
  Properties:
    ReservedConcurrentExecutions: 10
```

### Provisioned Concurrency

```yaml
MyFunction:
  Type: AWS::Serverless::Function
  Properties:
    AutoPublishAlias: live
    ProvisionedConcurrencyConfig:
      ProvisionedConcurrentExecutions: 5
```

---

## Best Practices

### 1. **Template Organization**

**Use parameters:**
```yaml
Parameters:
  Environment:
    Type: String
  MemorySize:
    Type: Number
    Default: 512
  LogLevel:
    Type: String
    Default: INFO
```

**Use Globals:**
```yaml
Globals:
  Function:
    Runtime: python3.9
    Timeout: 30
    Environment:
      Variables:
        LOG_LEVEL: !Ref LogLevel
```

**Separate concerns:**
```
template.yaml          # Main template
database.yaml          # Database resources
api.yaml              # API resources
functions/            # Function code
  ├── user/
  ├── order/
  └── payment/
```

### 2. **Security**

**IAM least privilege:**
```yaml
Policies:
  - DynamoDBCrudPolicy:
      TableName: !Ref MyTable
  # Not: AmazonDynamoDBFullAccess
```

**Environment variables for secrets:**
```yaml
Environment:
  Variables:
    SECRET_ARN: !Ref MySecret

# In code: retrieve from Secrets Manager
```

**Enable encryption:**
```yaml
MyTable:
  Type: AWS::Serverless::SimpleTable
  Properties:
    SSESpecification:
      SSEEnabled: true
```

### 3. **Performance**

**Right-size memory:**
```yaml
# Test and monitor
MemorySize: 1024  # Higher memory = more CPU
```

**Use layers for dependencies:**
```yaml
# Faster deployments, shared code
Layers:
  - !Ref DependenciesLayer
```

**Provisioned concurrency for latency-sensitive:**
```yaml
ProvisionedConcurrencyConfig:
  ProvisionedConcurrentExecutions: 5
```

### 4. **Cost Optimization**

**Use appropriate memory:**
```yaml
# Don't over-provision
MemorySize: 512  # vs 3008
```

**Set appropriate timeouts:**
```yaml
# Don't use default 3 seconds if need more
# Don't use 900 if need 10
Timeout: 30
```

**Use SQS for async:**
```yaml
# Instead of synchronous invocations
Events:
  QueueEvent:
    Type: SQS
    Properties:
      Queue: !GetAtt MyQueue.Arn
```

### 5. **Monitoring**

**CloudWatch metrics:**
```yaml
ErrorAlarm:
  Type: AWS::CloudWatch::Alarm
  Properties:
    MetricName: Errors
    Namespace: AWS/Lambda
    Statistic: Sum
    Period: 60
    EvaluationPeriods: 2
    Threshold: 1
    ComparisonOperator: GreaterThanThreshold
```

**Structured logging:**
```python
import json
import logging

logger = logging.getLogger()
logger.setLevel(logging.INFO)

def lambda_handler(event, context):
    logger.info(json.dumps({
        'event': 'request_received',
        'request_id': context.request_id,
        'user_id': event.get('user_id')
    }))
```

**X-Ray tracing:**
```yaml
Tracing: Active
```

### 6. **Testing**

**Unit tests:**
```python
# tests/unit/test_handler.py
import pytest
from src import app

def test_lambda_handler():
    event = {"key": "value"}
    context = {}
    response = app.lambda_handler(event, context)
    assert response['statusCode'] == 200
```

**Integration tests:**
```bash
# Start local API
sam local start-api &

# Run tests
pytest tests/integration/

# Cleanup
pkill -f "sam local"
```

**Load testing:**
```python
# tests/load/locustfile.py
from locust import HttpUser, task

class APIUser(HttpUser):
    @task
    def get_items(self):
        self.client.get("/items")
```

### 7. **Version Control**

**Exclude from git:**
```gitignore
.aws-sam/
packaged.yaml
samconfig.toml
*.pyc
__pycache__/
.env
```

**Include:**
- template.yaml
- Source code
- Tests
- Requirements files
- README

### 8. **Documentation**

**README.md:**
```markdown
# My Serverless App

## Prerequisites
- AWS CLI configured
- SAM CLI installed
- Docker running

## Local Development
```bash
sam build
sam local start-api
```

## Deployment
```bash
sam deploy --guided
```

## Architecture
[Include diagram]

## API Endpoints
- GET /items - List items
- POST /items - Create item
```

---

## Common Interview Questions

### Basic Level

**Q1: What is AWS SAM?**
A: AWS SAM (Serverless Application Model) is an open-source framework for building serverless applications on AWS. It consists of:
1. SAM Template Specification - Simplified CloudFormation syntax for serverless resources
2. SAM CLI - Command-line tool for local development, testing, and deployment

It simplifies defining Lambda functions, APIs, databases, and event source mappings.

**Q2: What is the difference between SAM and CloudFormation?**
A: SAM is an extension of CloudFormation specifically for serverless:
- **SAM**: Simplified syntax (`AWS::Serverless::Function` vs verbose Lambda+IAM+Logs)
- **Transform**: SAM templates use `Transform: AWS::Serverless-2016-10-31` which transforms SAM syntax to CloudFormation
- **Local Testing**: SAM CLI provides local invoke, start-api (CF doesn't)
- **Deployment**: SAM CLI handles packaging and deployment
- **Full CF Available**: Can use any CloudFormation resource in SAM templates

SAM makes serverless easier but compiles to CloudFormation for deployment.

**Q3: What are the main SAM resource types?**
A: 
- `AWS::Serverless::Function` - Lambda function with auto-generated IAM role
- `AWS::Serverless::Api` - API Gateway REST API
- `AWS::Serverless::HttpApi` - API Gateway HTTP API (v2)
- `AWS::Serverless::SimpleTable` - DynamoDB table (simplified)
- `AWS::Serverless::LayerVersion` - Lambda Layer
- `AWS::Serverless::StateMachine` - Step Functions state machine
- `AWS::Serverless::Application` - Nested application (SAR)

**Q4: How do you test Lambda functions locally with SAM?**
A:
```bash
# Build first
sam build

# Invoke function
sam local invoke MyFunction -e events/event.json

# Start API Gateway locally
sam local start-api

# Generate sample events
sam local generate-event s3 put > events/s3-event.json
```

Local testing uses Docker to simulate Lambda execution environment.

**Q5: What is the purpose of `sam build`?**
A: `sam build`:
1. Resolves dependencies (pip install, npm install, etc.)
2. Copies source code to build directory (.aws-sam/build)
3. Prepares artifacts for deployment or local testing
4. Can use Docker containers for consistent build environment (`--use-container`)
5. Optimizes for Lambda runtime

Required before `sam deploy` or `sam local invoke`.

### Intermediate Level

**Q6: Explain the SAM deployment process.**
A:
```bash
1. sam build
   - Resolves dependencies
   - Prepares artifacts

2. sam package (optional, done by deploy)
   - Uploads code to S3
   - Replaces local URIs with S3 URIs
   - Creates packaged template

3. sam deploy
   - Creates CloudFormation changeset
   - Shows changes (if confirmation enabled)
   - Executes changeset
   - Deploys stack
```

**First deployment:**
```bash
sam deploy --guided
# Saves configuration to samconfig.toml
```

**Subsequent:**
```bash
sam deploy  # Uses saved configuration
```

**Q7: What are SAM Policy Templates and why use them?**
A: SAM Policy Templates are pre-defined IAM policies for common use cases:
```yaml
Policies:
  - DynamoDBCrudPolicy:
      TableName: !Ref MyTable
  - S3ReadPolicy:
      BucketName: my-bucket
```

**Benefits:**
- Least privilege by default
- No need to write IAM policies manually
- Best practices built-in
- Reduced boilerplate
- Type-safe (validated at build)

**Alternative (verbose):**
```yaml
Policies:
  - Version: '2012-10-17'
    Statement:
      - Effect: Allow
        Action:
          - dynamodb:GetItem
          - dynamodb:PutItem
        Resource: !GetAtt MyTable.Arn
```

**Q8: How does SAM handle API Gateway integration?**
A: SAM simplifies API Gateway:

**SAM Syntax:**
```yaml
MyFunction:
  Type: AWS::Serverless::Function
  Properties:
    Events:
      GetApi:
        Type: Api
        Properties:
          Path: /items
          Method: get
```

**Behind the scenes creates:**
- API Gateway REST API
- Stage (Prod default)
- Resource (/items)
- Method (GET)
- Integration with Lambda
- Lambda permission for API Gateway
- CloudWatch Logs
- IAM role

**Equivalent CloudFormation:** 100+ lines vs 10 SAM lines.

**Q9: Explain Globals section in SAM templates.**
A: Globals define common properties applied to all resources of that type:

```yaml
Globals:
  Function:
    Runtime: python3.9
    Timeout: 30
    MemorySize: 512
    Environment:
      Variables:
        LOG_LEVEL: INFO
    Tracing: Active
  Api:
    Cors:
      AllowOrigin: "'*'"
```

**Benefits:**
- DRY (Don't Repeat Yourself)
- Consistent configuration
- Easy to update all functions
- Override per resource if needed

**Override example:**
```yaml
SpecialFunction:
  Type: AWS::Serverless::Function
  Properties:
    Timeout: 900  # Overrides global 30
```

**Q10: How do you handle multiple environments with SAM?**
A:

**Approach 1: Parameters**
```yaml
Parameters:
  Environment:
    Type: String
    AllowedValues: [dev, staging, prod]

Resources:
  MyFunction:
    Type: AWS::Serverless::Function
    Properties:
      FunctionName: !Sub ${Environment}-my-function
```

**Deploy:**
```bash
sam deploy --parameter-overrides Environment=prod
```

**Approach 2: samconfig.toml**
```toml
[dev.deploy.parameters]
stack_name = "my-app-dev"
parameter_overrides = "Environment=\"dev\""

[prod.deploy.parameters]
stack_name = "my-app-prod"
parameter_overrides = "Environment=\"prod\""
```

**Deploy:**
```bash
sam deploy --config-env prod
```

**Approach 3: Separate templates**
```
templates/
├── template-dev.yaml
├── template-staging.yaml
└── template-prod.yaml
```

### Advanced Level

**Q11: Design a serverless REST API with authentication using SAM.**
A:
```yaml
AWSTemplateFormatVersion: '2010-09-09'
Transform: AWS::Serverless-2016-10-31

Resources:
  # Cognito User Pool
  UserPool:
    Type: AWS::Cognito::UserPool
    Properties:
      UserPoolName: !Sub ${AWS::StackName}-users
      AutoVerifiedAttributes:
        - email
      Schema:
        - Name: email
          Required: true

  UserPoolClient:
    Type: AWS::Cognito::UserPoolClient
    Properties:
      UserPoolId: !Ref UserPool
      ExplicitAuthFlows:
        - ALLOW_USER_PASSWORD_AUTH
        - ALLOW_REFRESH_TOKEN_AUTH

  # API Gateway with Cognito Auth
  MyApi:
    Type: AWS::Serverless::Api
    Properties:
      StageName: prod
      Auth:
        DefaultAuthorizer: CognitoAuthorizer
        Authorizers:
          CognitoAuthorizer:
            UserPoolArn: !GetAtt UserPool.Arn
            Identity:
              Header: Authorization
      Cors:
        AllowOrigin: "'*'"
        AllowHeaders: "'Content-Type,Authorization'"

  # DynamoDB Table
  ItemsTable:
    Type: AWS::Serverless::SimpleTable
    Properties:
      PrimaryKey:
        Name: id
        Type: String
      ProvisionedThroughput:
        ReadCapacityUnits: 5
        WriteCapacityUnits: 5

  # CRUD Functions
  GetItemsFunction:
    Type: AWS::Serverless::Function
    Properties:
      CodeUri: src/
      Handler: handlers.get_items
      Runtime: python3.9
      Environment:
        Variables:
          TABLE_NAME: !Ref ItemsTable
      Policies:
        - DynamoDBReadPolicy:
            TableName: !Ref ItemsTable
      Events:
        GetItems:
          Type: Api
          Properties:
            RestApiId: !Ref MyApi
            Path: /items
            Method: get

  CreateItemFunction:
    Type: AWS::Serverless::Function
    Properties:
      CodeUri: src/
      Handler: handlers.create_item
      Runtime: python3.9
      Environment:
        Variables:
          TABLE_NAME: !Ref ItemsTable
      Policies:
        - DynamoDBCrudPolicy:
            TableName: !Ref ItemsTable
      Events:
        CreateItem:
          Type: Api
          Properties:
            RestApiId: !Ref MyApi
            Path: /items
            Method: post

  # Public endpoint (no auth)
  HealthCheckFunction:
    Type: AWS::Serverless::Function
    Properties:
      CodeUri: src/
      Handler: handlers.health_check
      Runtime: python3.9
      Events:
        HealthCheck:
          Type: Api
          Properties:
            RestApiId: !Ref MyApi
            Path: /health
            Method: get
            Auth:
              Authorizer: NONE

Outputs:
  ApiUrl:
    Value: !Sub https://${MyApi}.execute-api.${AWS::Region}.amazonaws.com/prod
  UserPoolId:
    Value: !Ref UserPool
  UserPoolClientId:
    Value: !Ref UserPoolClient
```

**Q12: How do you implement gradual deployments with SAM?**
A: Use DeploymentPreference with CodeDeploy integration:

```yaml
MyFunction:
  Type: AWS::Serverless::Function
  Properties:
    Handler: app.handler
    Runtime: python3.9
    AutoPublishAlias: live
    DeploymentPreference:
      Type: Canary10Percent10Minutes
      Alarms:
        - !Ref CanaryErrorsAlarm
        - !Ref LatencyAlarm
      Hooks:
        PreTraffic: !Ref PreTrafficHook
        PostTraffic: !Ref PostTrafficHook

# Monitor errors during deployment
CanaryErrorsAlarm:
  Type: AWS::CloudWatch::Alarm
  Properties:
    AlarmName: !Sub ${AWS::StackName}-canary-errors
    MetricName: Errors
    Namespace: AWS/Lambda
    Dimensions:
      - Name: FunctionName
        Value: !Ref MyFunction
    Statistic: Sum
    Period: 60
    EvaluationPeriods: 2
    Threshold: 1
    ComparisonOperator: GreaterThanThreshold

# Pre-deployment validation
PreTrafficHook:
  Type: AWS::Serverless::Function
  Properties:
    Handler: hooks.pre_traffic
    Runtime: python3.9
    DeploymentPreference:
      Enabled: false

# Post-deployment validation
PostTrafficHook:
  Type: AWS::Serverless::Function
  Properties:
    Handler: hooks.post_traffic
    Runtime: python3.9
    DeploymentPreference:
      Enabled: false
```

**Process:**
1. New version deployed
2. PreTraffic hook runs (smoke tests)
3. If pass: 10% traffic to new version
4. Monitor for 10 minutes
5. If alarms trigger: Automatic rollback
6. If healthy: Shift remaining 90%
7. PostTraffic hook runs (validation)

**Deployment types:**
- Canary10Percent30Minutes
- Canary10Percent5Minutes
- Linear10PercentEvery10Minutes
- Linear10PercentEvery1Minute
- AllAtOnce

**Q13: How do you optimize cold starts in SAM applications?**
A:

**1. Provisioned Concurrency:**
```yaml
MyFunction:
  Type: AWS::Serverless::Function
  Properties:
    AutoPublishAlias: live
    ProvisionedConcurrencyConfig:
      ProvisionedConcurrentExecutions: 5
```

**2. Use Lambda Layers:**
```yaml
# Separate dependencies from code
DependenciesLayer:
  Type: AWS::Serverless::LayerVersion
  Properties:
    ContentUri: layers/dependencies/
    CompatibleRuntimes:
      - python3.9

MyFunction:
  Type: AWS::Serverless::Function
  Properties:
    Layers:
      - !Ref DependenciesLayer
```

**3. Optimize Package Size:**
```yaml
# Use --use-container for consistent builds
# Exclude unnecessary files
```

**4. Right-size Memory:**
```yaml
# More memory = more CPU = faster init
MemorySize: 1024  # vs 128
```

**5. Connection Reuse:**
```python
# Outside handler (reused across invocations)
import boto3
dynamodb = boto3.resource('dynamodb')
table = dynamodb.Table(os.environ['TABLE_NAME'])

def lambda_handler(event, context):
    # Use pre-initialized connection
    table.put_item(Item=item)
```

**6. Lazy Loading:**
```python
# Import only what's needed
from boto3 import client as boto_client
# vs
import boto3
```

**7. Use Newer Runtimes:**
```yaml
Runtime: python3.11  # Faster than python3.7
```

**Q14: Explain SAM sync and when to use it.**
A: `sam sync` is for rapid development - syncs code changes to AWS without full deployment:

**Traditional workflow:**
```bash
# Edit code
sam build          # ~30s
sam deploy         # ~2-3 min
# Total: ~3-4 min per change
```

**With sam sync:**
```bash
sam sync --watch   # Watches for changes
# Edit code
# Auto synced in ~5-10 seconds
```

**How it works:**
- **Code-only changes**: Direct upload to Lambda (fast)
- **Infrastructure changes**: CloudFormation deployment (slower)

**Use cases:**
- **Development**: Rapid iteration
- **Debugging**: Quick feedback loop
- **Testing**: Test in real AWS environment

**When NOT to use:**
- Production deployments (use `sam deploy`)
- CI/CD pipelines (not reproducible)
- First deployment (use `sam deploy --guided`)

**Options:**
```bash
# Watch and sync code changes
sam sync --watch

# Sync code only (skip infrastructure)
sam sync --code

# Sync specific resource
sam sync --watch --resource MyFunction

# Tail logs while syncing
sam sync --watch --logs
```

**Q15: How do you debug Lambda functions locally with SAM?**
A:

**1. Setup VS Code:**

**launch.json:**
```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "SAM CLI Debug",
      "type": "python",
      "request": "attach",
      "port": 5858,
      "host": "localhost",
      "pathMappings": [
        {
          "localRoot": "${workspaceFolder}/src",
          "remoteRoot": "/var/task"
        }
      ]
    }
  ]
}
```

**2. Start debug session:**
```bash
# Terminal 1: Start function in debug mode
sam local invoke MyFunction -e events/event.json -d 5858

# VS Code: Run > Start Debugging (F5)
```

**3. For API debugging:**
```bash
# Terminal 1: Start API in debug mode
sam local start-api -d 5858

# VS Code: Start debugging
# Terminal 2: Make requests
curl http://localhost:3000/items
```

**4. Set breakpoints:**
- Click left gutter in VS Code
- Execution pauses at breakpoint
- Inspect variables, call stack
- Step through code

**5. Debug with environment variables:**
```bash
sam local invoke -e events/event.json --env-vars env.json -d 5858
```

**6. Node.js debugging:**
```bash
sam local invoke -d 9229
# Use Node.js debugger in VS Code
```

**Tips:**
- Use `print()` / `console.log()` for simple debugging
- Check CloudWatch Logs: `sam logs -n MyFunction --tail`
- Use X-Ray for distributed tracing
- Test with real AWS services using `--docker-network host`

**Q16: How do you implement API Gateway request validation with SAM?**
A:

```yaml
MyApi:
  Type: AWS::Serverless::Api
  Properties:
    StageName: prod
    DefinitionBody:
      openapi: 3.0.1
      info:
        title: My API
        version: 1.0.0
      paths:
        /items:
          post:
            requestBody:
              required: true
              content:
                application/json:
                  schema:
                    $ref: '#/components/schemas/Item'
            x-amazon-apigateway-request-validator: all
            x-amazon-apigateway-integration:
              uri: !Sub arn:aws:apigateway:${AWS::Region}:lambda:path/2015-03-31/functions/${CreateItemFunction.Arn}/invocations
              httpMethod: POST
              type: aws_proxy
      components:
        schemas:
          Item:
            type: object
            required:
              - name
              - price
            properties:
              name:
                type: string
                minLength: 1
                maxLength: 100
              price:
                type: number
                minimum: 0
              description:
                type: string
      x-amazon-apigateway-request-validators:
        all:
          validateRequestBody: true
          validateRequestParameters: true

CreateItemFunction:
  Type: AWS::Serverless::Function
  Properties:
    CodeUri: src/
    Handler: handlers.create_item
    Events:
      CreateItem:
        Type: Api
        Properties:
          RestApiId: !Ref MyApi
          Path: /items
          Method: post
```

**Benefits:**
- Validation before Lambda invocation
- Reduced Lambda cost (invalid requests rejected at API Gateway)
- Consistent error responses
- OpenAPI/Swagger documentation

**Alternative - Model validation:**
```yaml
MyApi:
  Type: AWS::Serverless::Api
  Properties:
    Models:
      ItemModel:
        type: object
        required:
          - name
        properties:
          name:
            type: string
          price:
            type: number

MyFunction:
  Properties:
    Events:
      PostItem:
        Type: Api
        Properties:
          RequestModel:
            Model: ItemModel
            Required: true
```

**Q17: How do you handle secrets and sensitive data in SAM applications?**
A:

**1. AWS Secrets Manager:**
```yaml
MySecret:
  Type: AWS::SecretsManager::Secret
  Properties:
    Name: !Sub ${AWS::StackName}/db-password
    SecretString: !Sub |
      {
        "username": "admin",
        "password": "${DBPassword}"
      }

MyFunction:
  Type: AWS::Serverless::Function
  Properties:
    Environment:
      Variables:
        SECRET_ARN: !Ref MySecret
    Policies:
      - AWSSecretsManagerGetSecretValuePolicy:
          SecretArn: !Ref MySecret
```

**In code:**
```python
import boto3
import json

secrets_client = boto3.client('secretsmanager')

def get_secret(secret_arn):
    response = secrets_client.get_secret_value(SecretId=secret_arn)
    return json.loads(response['SecretString'])

def lambda_handler(event, context):
    secret = get_secret(os.environ['SECRET_ARN'])
    db_password = secret['password']
```

**2. SSM Parameter Store:**
```yaml
MyParameter:
  Type: AWS::SSM::Parameter
  Properties:
    Name: !Sub /${AWS::StackName}/api-key
    Type: String
    Value: !Ref ApiKey

MyFunction:
  Properties:
    Policies:
      - SSMParameterReadPolicy:
          ParameterName: !Sub ${AWS::StackName}/api-key
```

**3. Environment Variables (encrypted):**
```yaml
MyFunction:
  Type: AWS::Serverless::Function
  Properties:
    Environment:
      Variables:
        DB_HOST: mydb.rds.amazonaws.com
        DB_NAME: myapp
    KmsKeyArn: !GetAtt KMSKey.Arn  # Encrypt env vars
```

**Best Practices:**
- Never hardcode secrets in templates
- Use Secrets Manager for rotation
- Use SSM for configuration
- Reference secrets at runtime (not deployment)
- Enable encryption
- Use separate secrets per environment

**Q18: Design a complete CI/CD pipeline for SAM application.**
A:

**GitHub Actions (.github/workflows/cicd.yml):**
```yaml
name: CI/CD Pipeline

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

env:
  AWS_REGION: us-east-1

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Setup Python
        uses: actions/setup-python@v4
        with:
          python-version: '3.9'
      
      - name: Install dependencies
        run: |
          pip install pytest pytest-cov boto3
          pip install -r src/requirements.txt
      
      - name: Run unit tests
        run: pytest tests/unit --cov=src --cov-report=xml
      
      - name: Upload coverage
        uses: codecov/codecov-action@v3
        with:
          files: ./coverage.xml

  build:
    needs: test
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Setup SAM CLI
        uses: aws-actions/setup-sam@v2
      
      - name: Build
        run: sam build --use-container
      
      - name: Upload artifacts
        uses: actions/upload-artifact@v3
        with:
          name: sam-build
          path: .aws-sam/build/

  deploy-dev:
    needs: build
    if: github.ref == 'refs/heads/develop'
    runs-on: ubuntu-latest
    environment: development
    steps:
      - uses: actions/checkout@v3
      
      - uses: actions/download-artifact@v3
        with:
          name: sam-build
          path: .aws-sam/build/
      
      - name: Configure AWS
        uses: aws-actions/configure-aws-credentials@v2
        with:
          aws-access-key-id: ${{ secrets.AWS_ACCESS_KEY_ID }}
          aws-secret-access-key: ${{ secrets.AWS_SECRET_ACCESS_KEY }}
          aws-region: ${{ env.AWS_REGION }}
      
      - name: Deploy to Dev
        run: |
          sam deploy \
            --no-confirm-changeset \
            --no-fail-on-empty-changeset \
            --stack-name myapp-dev \
            --capabilities CAPABILITY_IAM \
            --parameter-overrides Environment=dev \
            --tags Environment=dev
      
      - name: Integration Tests
        run: |
          export API_URL=$(aws cloudformation describe-stacks \
            --stack-name myapp-dev \
            --query 'Stacks[0].Outputs[?OutputKey==`ApiUrl`].OutputValue' \
            --output text)
          pytest tests/integration/

  deploy-prod:
    needs: [build, deploy-dev]
    if: github.ref == 'refs/heads/main'
    runs-on: ubuntu-latest
    environment: production
    steps:
      - uses: actions/checkout@v3
      
      - uses: actions/download-artifact@v3
        with:
          name: sam-build
          path: .aws-sam/build/
      
      - name: Configure AWS
        uses: aws-actions/configure-aws-credentials@v2
        with:
          aws-access-key-id: ${{ secrets.PROD_AWS_ACCESS_KEY_ID }}
          aws-secret-access-key: ${{ secrets.PROD_AWS_SECRET_ACCESS_KEY }}
          aws-region: ${{ env.AWS_REGION }}
      
      - name: Deploy to Prod
        run: |
          sam deploy \
            --no-confirm-changeset \
            --no-fail-on-empty-changeset \
            --stack-name myapp-prod \
            --capabilities CAPABILITY_IAM \
            --parameter-overrides Environment=prod \
            --tags Environment=prod
      
      - name: Smoke Tests
        run: |
          export API_URL=$(aws cloudformation describe-stacks \
            --stack-name myapp-prod \
            --query 'Stacks[0].Outputs[?OutputKey==`ApiUrl`].OutputValue' \
            --output text)
          pytest tests/smoke/
      
      - name: Notify
        if: always()
        uses: 8398a7/action-slack@v3
        with:
          status: ${{ job.status }}
          webhook_url: ${{ secrets.SLACK_WEBHOOK }}
```

**Pipeline stages:**
1. **Test**: Unit tests, coverage
2. **Build**: SAM build with containers
3. **Deploy Dev**: Auto-deploy develop branch
4. **Integration Tests**: Test in dev
5. **Deploy Prod**: Manual approval, deploy main
6. **Smoke Tests**: Verify production
7. **Notify**: Slack/email notification

**Q19: How do you implement rate limiting in SAM?**
A:

**1. API Gateway Usage Plans:**
```yaml
MyApi:
  Type: AWS::Serverless::Api
  Properties:
    StageName: prod

# Usage Plan
MyUsagePlan:
  Type: AWS::ApiGateway::UsagePlan
  Properties:
    UsagePlanName: BasicPlan
    ApiStages:
      - ApiId: !Ref MyApi
        Stage: prod
    Throttle:
      BurstLimit: 2000  # Max concurrent requests
      RateLimit: 1000   # Requests per second
    Quota:
      Limit: 10000      # Total requests per period
      Period: DAY

# API Key
MyApiKey:
  Type: AWS::ApiGateway::ApiKey
  Properties:
    Name: BasicApiKey
    Enabled: true

# Associate key with plan
UsagePlanKey:
  Type: AWS::ApiGateway::UsagePlanKey
  Properties:
    KeyId: !Ref MyApiKey
    KeyType: API_KEY
    UsagePlanId: !Ref MyUsagePlan
```

**Usage:**
```bash
curl -H "x-api-key: YOUR_API_KEY" https://api.example.com/items
```

**2. Lambda-level rate limiting (DynamoDB):**
```python
import boto3
from datetime import datetime, timedelta

dynamodb = boto3.resource('dynamodb')
rate_limit_table = dynamodb.Table('RateLimits')

def check_rate_limit(user_id, limit=100, window=60):
    now = int(datetime.now().timestamp())
    window_start = now - window
    
    # Increment counter
    response = rate_limit_table.update_item(
        Key={'userId': user_id, 'window': window_start},
        UpdateExpression='ADD requestCount :inc',
        ExpressionAttributeValues={':inc': 1},
        ReturnValues='UPDATED_NEW'
    )
    
    count = response['Attributes']['requestCount']
    return count <= limit

def lambda_handler(event, context):
    user_id = event['requestContext']['authorizer']['claims']['sub']
    
    if not check_rate_limit(user_id):
        return {
            'statusCode': 429,
            'body': json.dumps({'error': 'Rate limit exceeded'})
        }
    
    # Process request
```

**3. WAF rate-based rules:**
```yaml
MyWebACL:
  Type: AWS::WAFv2::WebACL
  Properties:
    DefaultAction:
      Allow: {}
    Rules:
      - Name: RateLimitRule
        Priority: 1
        Statement:
          RateBasedStatement:
            Limit: 2000
            AggregateKeyType: IP
        Action:
          Block: {}

# Associate with API
MyApiAssociation:
  Type: AWS::WAFv2::WebACLAssociation
  Properties:
    ResourceArn: !Sub arn:aws:apigateway:${AWS::Region}::/restapis/${MyApi}/stages/prod
    WebACLArn: !GetAtt MyWebACL.Arn
```

**Q20: What are common SAM deployment failures and how do you troubleshoot?**
A:

**Common Issues:**

**1. Insufficient IAM Permissions:**
```
Error: User is not authorized to perform: cloudformation:CreateStack
```
**Solution:**
- Ensure IAM user/role has CloudFormation permissions
- Check `sam deploy --capabilities CAPABILITY_IAM`

**2. S3 Bucket Access:**
```
Error: Unable to upload artifact to S3
```
**Solution:**
```bash
# Create bucket manually
aws s3 mb s3://my-deployment-bucket

# Or use --resolve-s3
sam deploy --guided --resolve-s3
```

**3. Stack Already Exists:**
```
Error: Stack [name] already exists
```
**Solution:**
```bash
# Use different stack name
sam deploy --stack-name myapp-v2

# Or delete existing
sam delete --stack-name myapp
```

**4. Resource Limit Exceeded:**
```
Error: LimitExceededException: Account limit exceeded
```
**Solution:**
- Request limit increase (Service Quotas)
- Clean up unused resources
- Use Lambda reserved concurrency

**5. Invalid Template:**
```
Error: Template format error
```
**Solution:**
```bash
# Validate template
sam validate

# Check YAML syntax
yamllint template.yaml
```

**6. Rollback:**
```
Stack rollback in progress
```
**Solution:**
```bash
# Check events
sam list stack-outputs --stack-name myapp

# View logs
sam logs -n MyFunction --stack-name myapp

# Describe stack
aws cloudformation describe-stack-events --stack-name myapp
```

**7. Function Timeout During Deploy:**
```
Error: Lambda function validation failed
```
**Solution:**
- Check Lambda can reach dependencies (VPC, internet)
- Increase timeout
- Check security groups

**Troubleshooting Steps:**
1. Check CloudFormation events in AWS Console
2. Review CloudWatch Logs
3. Use `sam logs` to fetch function logs
4. Validate template: `sam validate --lint`
5. Test locally: `sam local invoke`
6. Enable debug: `sam deploy --debug`
7. Check IAM permissions
8. Review stack outputs and resources

---

## Summary Cheat Sheet

### Essential Commands
```bash
# Initialize
sam init

# Build
sam build
sam build --use-container

# Test locally
sam local invoke MyFunction -e events/event.json
sam local start-api
sam local generate-event s3 put

# Deploy
sam deploy --guided
sam deploy

# Sync (dev only)
sam sync --watch

# Logs
sam logs -n MyFunction --tail

# Delete
sam delete
```

### Resource Types
- `AWS::Serverless::Function` - Lambda
- `AWS::Serverless::Api` - API Gateway REST
- `AWS::Serverless::HttpApi` - API Gateway HTTP
- `AWS::Serverless::SimpleTable` - DynamoDB
- `AWS::Serverless::LayerVersion` - Lambda Layer
- `AWS::Serverless::StateMachine` - Step Functions

### Key Concepts
- Transform: `AWS::Serverless-2016-10-31`
- Globals: Shared properties
- Events: Function triggers
- Policies: IAM policy templates
- DeploymentPreference: Gradual deployments

### Best Practices
1. Use `sam build --use-container` for consistency
2. Always use policy templates (least privilege)
3. Enable tracing: `Tracing: Active`
4. Use Globals for common config
5. Test locally before deploying
6. Use parameters for environments
7. Enable gradual deployments for production
8. Monitor with CloudWatch and X-Ray
9. Use layers for dependencies
10. Version control samconfig.toml

### Interview Tips
1. Know SAM vs CloudFormation differences
2. Understand local testing workflow
3. Explain deployment process
4. Know resource types and their use cases
5. Understand policy templates
6. Practice gradual deployments
7. Know troubleshooting steps
8. Understand CI/CD integration
9. Practice template writing
10. Know cost optimization strategies

---

*Last Updated: 2025*
*Based on AWS Official Documentation and Best Practices*