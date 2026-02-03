🌟How to Reduce Docker Image Size🌟

Reducing image size is important because smaller images:
• Build faster during CI/CD.
• Deploy faster to production.
• Consume less storage in registries and on servers.
• Reduce attack surface by having fewer unnecessary packages.

⸻

1. Choose a Minimal Base Image
• Instead of using large OS images like ubuntu or debian, use alpine or scratch.

• Example:

# Bad (large)
FROM ubuntu:20.04

# Good (small)
FROM alpine:3.19

• Why? Alpine is ~5 MB compared to Ubuntu (~70–100 MB).

⸻

2. Use Multi-Stage Builds
• Compile your app in one stage, then copy only the required output to a final lightweight stage.

• Example:

# Stage 1: Build
FROM golang:1.22 AS builder
WORKDIR /app
COPY . .
RUN go build -o myapp .

# Stage 2: Minimal runtime
FROM alpine:3.19
WORKDIR /app
COPY —from=builder /app/myapp .
CMD [“./myapp”]

• Result: Instead of carrying build tools and dependencies in production, you only keep the binary.

⸻

3. Remove Unnecessary Files
• Clear package caches and temp files after installation.
• Example:

RUN apk add —no-cache git \
&& rm -rf /var/cache/apk/* /tmp/*

• In apt-based images:

RUN apt-get update && apt-get install -y git \
&& rm -rf /var/lib/apt/lists/*

4. Combine RUN Instructions
• Each RUN creates a new image layer. Combine related commands to avoid extra layers.
• Example:

# Bad
RUN apt-get update
RUN apt-get install -y git
RUN rm -rf /var/lib/apt/lists/*

# Good
RUN apt-get update && apt-get install -y git && rm -rf /var/lib/apt/lists/*

5. Use .dockerignore
• Prevent large, unnecessary files (like .git, node_modules, logs) from being sent to the Docker build context.
• Example .dockerignore:

.git
node_modules
*.log
*.tmp

• Why? A smaller build context means smaller images.

🌟Summary Best Practices:
• Use small base images (alpine, slim, scratch).
• Apply multi-stage builds.
• Remove caches & temp files.
• Use .dockerignore.
• Combine commands into fewer layers.
• Avoid copying unnecessary files.
