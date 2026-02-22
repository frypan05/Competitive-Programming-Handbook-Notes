# 04 — CI/CD: Jenkins, GitHub Actions & GitLab CI

---

## SECTION 1 — CI/CD Fundamentals

**Q: What is CI/CD?**

```
CI (Continuous Integration)  — developers frequently merge code to a shared branch.
                                Each merge triggers automated build + tests.
                                Goal: catch integration bugs early, not at release time.

CD (Continuous Delivery)     — every passing build is releasable to production.
                                Deployment to production requires a manual approval gate.

CD (Continuous Deployment)   — every passing build is AUTOMATICALLY deployed to production.
                                No human gate whatsoever.
```

**Typical pipeline stages:**
1. Source — code push / PR triggers the pipeline
2. Build — compile, package, build Docker image
3. Test — unit tests, integration tests, security scan, lint
4. Artifact — push Docker image to registry, store build artifacts
5. Deploy to Staging — automated deployment
6. Integration / Smoke Tests — run against the staging environment
7. Deploy to Production — manual gate (Delivery) or automatic (Deployment)

---

**Q: What is the difference between Continuous Delivery and Continuous Deployment?**

A: In Continuous Delivery, every build is READY to deploy but a human approves the production
release. In Continuous Deployment, every passing build is AUTOMATICALLY deployed to production
with no human intervention. Most companies doing "CD" actually practice Continuous Delivery —
fully automated production deployment requires high confidence in test coverage.

---

**Q: How do you handle secrets in CI/CD pipelines?**

```
Jenkins        — Credentials store. Bind via credentials() DSL.
                 Types: usernamePassword, secretText, sshUserPrivateKey, file.
                 Access: DOCKER_CREDS = credentials('registry-creds')
                         → DOCKER_CREDS_USR and DOCKER_CREDS_PSW available.

GitHub Actions — Repository/Organization/Environment Secrets.
                 Access: ${{ secrets.SECRET_NAME }}
                 Secrets are automatically masked in logs.

GitLab CI      — CI/CD Variables (masked and/or protected).
                 Access: $SECRET_NAME

External tools — HashiCorp Vault, AWS Secrets Manager, Azure Key Vault.
                 Fetch secrets at runtime. Never bake secrets into pipeline YAML files.
```

---

**Q: How do you speed up a slow CI pipeline?**

1. Cache dependencies (npm, pip, maven, go mod) — avoid re-downloading every run
2. Parallelise independent stages and test suites
3. Docker BuildKit + registry layer caching — don't rebuild unchanged layers
4. Fail fast — run lint/unit tests before slow integration tests
5. Use lightweight base images for CI agents (alpine instead of ubuntu)
6. Pre-build a custom CI image with all tools already installed
7. Test impact analysis — only run tests affected by changed files

---

## SECTION 2 — Jenkins

**Q: What is Jenkins?**

A: An open-source automation server written in Java. Runs pipelines (jobs) that orchestrate
build, test, and deployment steps. Can run on a single server or distribute work to agents.

---

**Q: Controller vs Agent?**

```
Controller (formerly master) — manages pipeline scheduling, stores config, serves the UI,
                                coordinates agents.
                                Do NOT run heavy workloads on the controller itself.

Agent (formerly slave)       — worker node that executes pipeline steps.
Types:
  Permanent agents    — always-on machines registered permanently
  Docker agents       — containers spun up per build, torn down after (clean environment)
  Kubernetes agents   — pods created dynamically in a K8s cluster via kubernetes plugin
  EC2 agents          — AWS EC2 instances provisioned on demand via EC2 plugin
```

---

**Q: Declarative vs Scripted Pipeline?**

```
Declarative (recommended):
  - Structured pipeline {} block with a strict schema
  - Easier to read, write, validate, and onboard new engineers
  - Built-in directives: agent, stages, post, environment, options, when, parallel, input
  - Limited Groovy — use script {} block for complex logic only

Scripted:
  - Raw Groovy inside a node {} block
  - Maximum flexibility but harder to read and no schema validation
  - Suited for very complex dynamic pipeline logic
```

---

**Q: Explain the key Jenkinsfile directives.**

```
agent       — where to run: any, specific node label, docker container, K8s pod
stages      — container for all stage blocks
stage       — a named logical group of steps (shown in pipeline visualization)
steps       — the actual shell commands and DSL calls inside a stage
parallel    — run multiple stages concurrently inside a stage
post        — actions after pipeline: always, success, failure, unstable, cleanup
environment — declare environment variables (supports credentials() binding)
options     — pipeline-level settings: timeout, buildDiscarder, disableConcurrentBuilds
when        — conditional execution: branch, tag, expression, changeRequest
input       — pause pipeline for manual human approval
parameters  — define build parameters (string, choice, boolean, password)
```

---

## SECTION 3 — Jenkinsfile: Full Declarative Pipeline (Node.js/Python app to K8s)

```groovy
pipeline {
    agent {
        kubernetes {
            yaml """
apiVersion: v1
kind: Pod
spec:
  containers:
  - name: python
    image: python:3.12-slim
    command: [cat]
    tty: true
  - name: docker
    image: docker:24-dind
    securityContext:
      privileged: true
    env:
    - name: DOCKER_TLS_CERTDIR
      value: ""
  - name: kubectl
    image: bitnami/kubectl:1.28
    command: [cat]
    tty: true
"""
        }
    }

    environment {
        APP_NAME        = 'myapp'
        REGISTRY        = 'registry.company.com'
        IMAGE_TAG       = "${env.BRANCH_NAME}-${env.GIT_COMMIT[0..7]}"
        DOCKER_CREDS    = credentials('registry-credentials')
        KUBECONFIG_STAG = credentials('kubeconfig-staging')
        KUBECONFIG_PROD = credentials('kubeconfig-production')
    }

    options {
        timeout(time: 30, unit: 'MINUTES')
        buildDiscarder(logRotator(numToKeepStr: '20'))
        disableConcurrentBuilds()
        skipDefaultCheckout()
    }

    parameters {
        choice(name: 'LOG_LEVEL', choices: ['INFO', 'DEBUG', 'WARNING'], description: 'Log level')
        booleanParam(name: 'SKIP_TESTS', defaultValue: false, description: 'Skip test stage')
    }

    stages {
        stage('Checkout') {
            steps {
                checkout scm
                script {
                    env.GIT_AUTHOR = sh(
                        script: "git log -1 --format='%an'",
                        returnStdout: true
                    ).trim()
                    env.GIT_MESSAGE = sh(
                        script: "git log -1 --format='%s'",
                        returnStdout: true
                    ).trim()
                }
                echo "Building: ${env.GIT_MESSAGE} by ${env.GIT_AUTHOR}"
            }
        }

        stage('Install & Lint') {
            when {
                not { expression { params.SKIP_TESTS } }
            }
            steps {
                container('python') {
                    sh '''
                        pip install -r requirements.txt -r requirements-dev.txt --quiet
                        flake8 . --max-line-length=120
                        black --check .
                        isort --check-only .
                    '''
                }
            }
        }

        stage('Tests') {
            when {
                not { expression { params.SKIP_TESTS } }
            }
            parallel {
                stage('Unit Tests') {
                    steps {
                        container('python') {
                            sh '''
                                pytest tests/unit \
                                    -v \
                                    --junitxml=test-results/unit.xml \
                                    --cov=app \
                                    --cov-report=xml:coverage.xml
                            '''
                        }
                    }
                    post {
                        always {
                            junit 'test-results/unit.xml'
                        }
                    }
                }
                stage('Security Scan') {
                    steps {
                        container('python') {
                            sh 'pip-audit -r requirements.txt --desc on'
                        }
                    }
                }
            }
        }

        stage('Build Docker Image') {
            steps {
                container('docker') {
                    sh """
                        docker build \\
                            --build-arg BUILD_DATE=\$(date -u +%Y-%m-%dT%H:%M:%SZ) \\
                            --build-arg GIT_SHA=${env.GIT_COMMIT} \\
                            --build-arg APP_VERSION=${env.IMAGE_TAG} \\
                            -t ${REGISTRY}/${APP_NAME}:${IMAGE_TAG} \\
                            -t ${REGISTRY}/${APP_NAME}:latest \\
                            .
                    """
                }
            }
        }

        stage('Scan Image') {
            steps {
                container('docker') {
                    sh """
                        docker run --rm \\
                            -v /var/run/docker.sock:/var/run/docker.sock \\
                            aquasec/trivy:latest image \\
                            --exit-code 1 \\
                            --severity HIGH,CRITICAL \\
                            --no-progress \\
                            ${REGISTRY}/${APP_NAME}:${IMAGE_TAG}
                    """
                }
            }
        }

        stage('Push Image') {
            steps {
                container('docker') {
                    sh """
                        echo ${DOCKER_CREDS_PSW} | \\
                            docker login ${REGISTRY} -u ${DOCKER_CREDS_USR} --password-stdin
                        docker push ${REGISTRY}/${APP_NAME}:${IMAGE_TAG}
                        docker push ${REGISTRY}/${APP_NAME}:latest
                    """
                }
            }
        }

        stage('Deploy to Staging') {
            when {
                branch 'develop'
            }
            steps {
                container('kubectl') {
                    sh """
                        export KUBECONFIG=${KUBECONFIG_STAG}
                        kubectl set image deployment/${APP_NAME} \\
                            ${APP_NAME}=${REGISTRY}/${APP_NAME}:${IMAGE_TAG} \\
                            -n staging
                        kubectl rollout status deployment/${APP_NAME} \\
                            -n staging --timeout=5m
                    """
                }
            }
        }

        stage('Integration Tests (Staging)') {
            when {
                branch 'develop'
            }
            steps {
                container('python') {
                    sh 'pytest tests/integration -v --base-url=https://staging.example.com'
                }
            }
        }

        stage('Approve Production Deploy') {
            when {
                branch 'main'
            }
            steps {
                input(
                    message: "Deploy ${APP_NAME}:${IMAGE_TAG} to PRODUCTION?",
                    ok: 'Deploy Now',
                    submitter: 'ops-team,team-leads',
                    parameters: [
                        booleanParam(name: 'CONFIRMED', defaultValue: false,
                                     description: 'I have verified staging is healthy')
                    ]
                )
            }
        }

        stage('Deploy to Production') {
            when {
                branch 'main'
            }
            steps {
                container('kubectl') {
                    sh """
                        export KUBECONFIG=${KUBECONFIG_PROD}
                        kubectl set image deployment/${APP_NAME} \\
                            ${APP_NAME}=${REGISTRY}/${APP_NAME}:${IMAGE_TAG} \\
                            -n production
                        kubectl rollout status deployment/${APP_NAME} \\
                            -n production --timeout=10m
                        kubectl get pods -n production -l app=${APP_NAME}
                    """
                }
            }
        }
    }

    post {
        success {
            slackSend(
                channel: '#deployments',
                color: 'good',
                message: """:white_check_mark: *${APP_NAME}* \`${IMAGE_TAG}\` deployed successfully
Branch: ${env.BRANCH_NAME} | Author: ${env.GIT_AUTHOR}
Build: <${env.BUILD_URL}|#${env.BUILD_NUMBER}>"""
            )
        }
        failure {
            slackSend(
                channel: '#deployments',
                color: 'danger',
                message: """:x: *${APP_NAME}* \`${IMAGE_TAG}\` pipeline FAILED
Branch: ${env.BRANCH_NAME} | Author: ${env.GIT_AUTHOR}
Build: <${env.BUILD_URL}|#${env.BUILD_NUMBER}>"""
            )
            emailext(
                subject: "FAILED: ${env.JOB_NAME} #${env.BUILD_NUMBER}",
                body: "Pipeline failed. Check: ${env.BUILD_URL}",
                to: 'devops@company.com'
            )
        }
        always {
            cleanWs()
        }
    }
}
```

---

## SECTION 4 — Jenkinsfile: Scripted Pipeline Example

```groovy
node('linux-agent') {
    def image
    def registry = 'registry.company.com'
    def appName  = 'myapp'
    def tag      = "${env.BUILD_NUMBER}"

    try {
        stage('Checkout') {
            checkout scm
        }

        stage('Build') {
            image = docker.build("${registry}/${appName}:${tag}")
        }

        stage('Test') {
            image.inside {
                sh 'python -m pytest tests/ -v'
            }
        }

        stage('Push') {
            docker.withRegistry("https://${registry}", 'registry-credentials') {
                image.push(tag)
                image.push('latest')
            }
        }

        stage('Deploy') {
            sh """
                kubectl set image deployment/${appName} \
                    ${appName}=${registry}/${appName}:${tag} \
                    -n production
            """
        }

    } catch (err) {
        currentBuild.result = 'FAILURE'
        throw err
    } finally {
        sh 'docker system prune -f'
    }
}
```

---

## SECTION 5 — GitHub Actions: Full CI/CD Workflow

```yaml
# .github/workflows/ci-cd.yml
name: CI/CD Pipeline

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]
  workflow_dispatch:
    inputs:
      environment:
        description: 'Target environment'
        required: true
        default: 'staging'
        type: choice
        options: [staging, production]

env:
  REGISTRY: ghcr.io
  IMAGE_NAME: ${{ github.repository }}

jobs:
  # ── Lint & Test ─────────────────────────────────────────────────────────────
  test:
    name: Test (Python ${{ matrix.python-version }})
    runs-on: ubuntu-latest
    strategy:
      fail-fast: false
      matrix:
        python-version: ['3.11', '3.12']

    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Set up Python ${{ matrix.python-version }}
        uses: actions/setup-python@v5
        with:
          python-version: ${{ matrix.python-version }}

      - name: Cache pip packages
        uses: actions/cache@v4
        with:
          path: ~/.cache/pip
          key: ${{ runner.os }}-pip-${{ hashFiles('requirements*.txt') }}
          restore-keys: ${{ runner.os }}-pip-

      - name: Install dependencies
        run: pip install -r requirements.txt -r requirements-dev.txt

      - name: Lint
        run: |
          flake8 . --max-line-length=120
          black --check .
          isort --check-only .

      - name: Run tests
        run: pytest --cov=. --cov-report=xml -v

      - name: Upload coverage
        uses: codecov/codecov-action@v4
        if: matrix.python-version == '3.12'
        with:
          file: ./coverage.xml

  # ── Security Scan ───────────────────────────────────────────────────────────
  security:
    name: Security Scan
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Dependency vulnerability scan
        uses: pypa/gh-action-pip-audit@v1

      - name: Filesystem SAST scan
        uses: aquasecurity/trivy-action@master
        with:
          scan-type: fs
          scan-ref: .
          exit-code: 1
          severity: HIGH,CRITICAL

  # ── Build & Push Docker Image ───────────────────────────────────────────────
  build:
    name: Build & Push Image
    runs-on: ubuntu-latest
    needs: [test, security]
    if: github.event_name != 'pull_request'
    permissions:
      contents: read
      packages: write

    outputs:
      image-tag: ${{ steps.meta.outputs.version }}
      image-digest: ${{ steps.build.outputs.digest }}

    steps:
      - uses: actions/checkout@v4

      - name: Set up Docker Buildx
        uses: docker/setup-buildx-action@v3

      - name: Login to GitHub Container Registry
        uses: docker/login-action@v3
        with:
          registry: ${{ env.REGISTRY }}
          username: ${{ github.actor }}
          password: ${{ secrets.GITHUB_TOKEN }}

      - name: Extract Docker metadata (tags, labels)
        id: meta
        uses: docker/metadata-action@v5
        with:
          images: ${{ env.REGISTRY }}/${{ env.IMAGE_NAME }}
          tags: |
            type=sha,prefix={{branch}}-,format=short
            type=ref,event=branch
            type=raw,value=latest,enable=${{ github.ref == 'refs/heads/main' }}

      - name: Build and push Docker image
        id: build
        uses: docker/build-push-action@v5
        with:
          context: .
          push: true
          tags: ${{ steps.meta.outputs.tags }}
          labels: ${{ steps.meta.outputs.labels }}
          cache-from: type=gha
          cache-to: type=gha,mode=max
          build-args: |
            GIT_SHA=${{ github.sha }}
            BUILD_DATE=${{ github.event.repository.updated_at }}

      - name: Scan pushed image
        uses: aquasecurity/trivy-action@master
        with:
          scan-type: image
          image-ref: ${{ env.REGISTRY }}/${{ env.IMAGE_NAME }}:${{ steps.meta.outputs.version }}
          exit-code: 1
          severity: HIGH,CRITICAL

  # ── Deploy to Staging ───────────────────────────────────────────────────────
  deploy-staging:
    name: Deploy to Staging
    runs-on: ubuntu-latest
    needs: build
    if: github.ref == 'refs/heads/develop'
    environment:
      name: staging
      url: https://staging.example.com

    steps:
      - uses: actions/checkout@v4

      - name: Set up kubectl
        uses: azure/setup-kubectl@v3

      - name: Deploy to staging
        env:
          KUBECONFIG_DATA: ${{ secrets.KUBECONFIG_STAGING }}
        run: |
          echo "$KUBECONFIG_DATA" | base64 -d > /tmp/kubeconfig
          export KUBECONFIG=/tmp/kubeconfig
          kubectl set image deployment/myapp \
            myapp=${{ env.REGISTRY }}/${{ env.IMAGE_NAME }}:${{ needs.build.outputs.image-tag }} \
            -n staging
          kubectl rollout status deployment/myapp -n staging --timeout=5m
          kubectl get pods -n staging -l app=myapp

  # ── Deploy to Production ────────────────────────────────────────────────────
  deploy-production:
    name: Deploy to Production
    runs-on: ubuntu-latest
    needs: build
    if: github.ref == 'refs/heads/main'
    environment:
      name: production       # configured with required reviewers in GitHub Settings
      url: https://example.com

    steps:
      - uses: actions/checkout@v4

      - name: Set up kubectl
        uses: azure/setup-kubectl@v3

      - name: Deploy to production
        env:
          KUBECONFIG_DATA: ${{ secrets.KUBECONFIG_PROD }}
        run: |
          echo "$KUBECONFIG_DATA" | base64 -d > /tmp/kubeconfig
          export KUBECONFIG=/tmp/kubeconfig
          kubectl set image deployment/myapp \
            myapp=${{ env.REGISTRY }}/${{ env.IMAGE_NAME }}:${{ needs.build.outputs.image-tag }} \
            -n production
          kubectl rollout status deployment/myapp -n production --timeout=10m

      - name: Notify Slack on success
        uses: slackapi/slack-github-action@v1.27.0
        with:
          payload: |
            {
              "text": ":white_check_mark: *myapp* deployed to production: `${{ needs.build.outputs.image-tag }}`"
            }
        env:
          SLACK_WEBHOOK_URL: ${{ secrets.SLACK_WEBHOOK_URL }}
```

---

## SECTION 6 — GitLab CI

```yaml
# .gitlab-ci.yml
stages:
  - test
  - build
  - deploy

variables:
  DOCKER_DRIVER: overlay2
  DOCKER_TLS_CERTDIR: ""
  IMAGE: $CI_REGISTRY_IMAGE:$CI_COMMIT_SHORT_SHA

test:
  stage: test
  image: python:3.12-slim
  cache:
    key: pip-$CI_COMMIT_REF_SLUG
    paths: [.pip_cache/]
  before_script:
    - pip install --cache-dir .pip_cache -r requirements.txt -r requirements-dev.txt
  script:
    - flake8 . --max-line-length=120
    - pytest --cov=. --cov-report=xml -v
  artifacts:
    reports:
      coverage_report:
        coverage_format: cobertura
        path: coverage.xml
    junit: test-results/*.xml

build:
  stage: build
  image: docker:24
  services:
    - docker:24-dind
  before_script:
    - docker login -u $CI_REGISTRY_USER -p $CI_REGISTRY_PASSWORD $CI_REGISTRY
  script:
    - docker build -t $IMAGE .
    - docker push $IMAGE
  only:
    - main
    - develop

deploy_staging:
  stage: deploy
  image: bitnami/kubectl:1.28
  environment:
    name: staging
    url: https://staging.example.com
  script:
    - kubectl set image deployment/myapp myapp=$IMAGE -n staging
    - kubectl rollout status deployment/myapp -n staging --timeout=5m
  only:
    - develop

deploy_production:
  stage: deploy
  image: bitnami/kubectl:1.28
  environment:
    name: production
    url: https://example.com
  when: manual          # requires a human to click "play"
  script:
    - kubectl set image deployment/myapp myapp=$IMAGE -n production
    - kubectl rollout status deployment/myapp -n production --timeout=10m
  only:
    - main
```

---

## SECTION 7 — Key Interview Questions

**Q: What is pipeline as code and why is it important?**

A: The pipeline definition lives in the source repository alongside the application code
(Jenkinsfile, .github/workflows/, .gitlab-ci.yml). Benefits:
- Version controlled — every change is tracked, reviewed, and reversible
- Reviewed in the same PR as the application change
- Reproducible — same config produces the same pipeline everywhere
- Discoverable — any developer can see and understand the entire pipeline

---

**Q: How do you implement blue/green deployment in a pipeline?**

```
1. Deploy new version to "green" environment (separate from live "blue").
2. Run smoke tests against green.
3. Switch traffic: change LB target group, K8s Service selector, or DNS.
4. Keep blue running for instant rollback (just flip traffic back).
5. After confidence period, terminate blue.
```

---

**Q: What is a canary deployment?**

A: Route a small percentage of traffic (e.g. 5%) to the new version while the rest goes to the
old. Monitor error rate and latency. Gradually shift to 100% if healthy. Roll back instantly
by shifting traffic back to 0% on the new version. Implemented with: Argo Rollouts, Istio, or
manually with weighted target groups in AWS ALB.

---

**Q: What is the difference between a build artifact and a Docker image in CI?**

```
Build artifact — a compiled binary, JAR, ZIP, or test report. Stored in an artifact store
                 (S3, Nexus, JFrog Artifactory). Versioned but not a runnable environment.

Docker image   — a fully self-contained, runnable package. Includes the OS libraries,
                 runtime, and your application. Stored in a container registry (ECR, GHCR, Docker Hub).
                 The preferred artifact for containerised applications.
```

---

**Q: How do you roll back a failed deployment from a Jenkins pipeline?**

```groovy
// Option 1: kubectl rollout undo (Kubernetes)
sh 'kubectl rollout undo deployment/myapp -n production'

// Option 2: Re-run previous build with known good IMAGE_TAG
// Option 3: Tag previous image as latest and re-deploy

// Best practice: never tag "latest" as your deployment reference.
// Always use a specific, immutable tag (git SHA, build number).
// Rollback = re-deploy the previous immutable tag.
```

---

**Q: What is a shared library in Jenkins?**

A: Reusable Groovy code stored in a separate Git repository and loaded into pipelines. Avoids
copy-pasting common logic across many Jenkinsfiles. Defined in `vars/` (global variables,
callable as steps) or `src/` (Groovy classes).

```groovy
// Jenkinsfile — use shared library
@Library('my-shared-lib@main') _

pipeline {
    agent any
    stages {
        stage('Build') {
            steps {
                // Call a step defined in vars/buildDockerImage.groovy
                buildDockerImage(name: 'myapp', tag: env.BUILD_NUMBER)
            }
        }
    }
}
```
