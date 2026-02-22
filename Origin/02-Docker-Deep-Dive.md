# 02 — Docker Deep Dive

---

## SECTION 1 — How Docker Works Internally

Docker uses three Linux kernel features to create isolated environments:

- **Namespaces** — isolate PID, network, mount, UTS (hostname), IPC, user. Each container sees its own isolated "world."
- **cgroups (control groups)** — limit and account for CPU, memory, disk I/O, and network bandwidth per container.
- **Union filesystem (overlay2)** — layers stacked on top of each other. Each Dockerfile instruction creates a read-only layer. A thin writable layer is added on top when a container starts.

Docker is NOT a VM. There is no hypervisor and no separate kernel. Containers share the host kernel. This makes them faster to start and lighter in memory, but less isolated than VMs.

---

## SECTION 2 — CMD vs ENTRYPOINT (was asked — know this cold)

**Q: What is the difference between CMD and ENTRYPOINT?**

```
ENTRYPOINT — defines the fixed executable that ALWAYS runs.
             Cannot be overridden by arguments after the image name in docker run.

CMD        — provides default arguments to ENTRYPOINT,
             OR the default command if no ENTRYPOINT is set.
             IS overridden by arguments after the image name in docker run.
```

```dockerfile
ENTRYPOINT ["python", "app.py"]
CMD ["--port", "8080"]

# docker run myimage                    → python app.py --port 8080
# docker run myimage --port 9090        → python app.py --port 9090   (CMD overridden)
# docker run --entrypoint bash myimage  → bash                        (ENTRYPOINT overridden)
```

---

**Q: Shell form vs exec form — what is the difference and why does it matter?**

```dockerfile
# Shell form — runs through /bin/sh -c
CMD echo "hello"
ENTRYPOINT python app.py

# Exec form — runs directly, no shell involved
CMD ["python", "app.py"]
ENTRYPOINT ["nginx", "-g", "daemon off;"]
```

**Always use exec form for ENTRYPOINT and CMD.**

With shell form, `/bin/sh` becomes PID 1 and does NOT forward signals like SIGTERM to your
process. With exec form, your process IS PID 1 and receives signals directly. This is critical
for graceful shutdown — `docker stop` sends SIGTERM to PID 1.

---

**Q: What is the difference between COPY and ADD?**

```
COPY — copies files/directories from the build context into the image.
       Simple and predictable. Preferred for almost all use cases.

ADD  — does everything COPY does, PLUS:
         - Automatically unpacks local .tar archives into the image
         - Can download files from remote URLs
```

**Best practice: Always prefer COPY unless you specifically need tar extraction.**
Using ADD with a URL is an anti-pattern — use `RUN curl` instead for better layer caching control.

---

## SECTION 3 — HOW TO REDUCE BUILD TIME (was asked — critical)

### Strategy 1: Order layers by change frequency

Docker caches each layer. If a layer's instruction and its inputs are unchanged, Docker reuses
the cache. The rule: put things that change RARELY at the TOP and things that change OFTEN at
the BOTTOM.

```dockerfile
# BAD — any file change busts the cache at COPY and re-runs npm install every time
FROM node:20-alpine
WORKDIR /app
COPY . .
RUN npm install

# GOOD — npm install only re-runs when package.json/package-lock.json change
FROM node:20-alpine
WORKDIR /app
COPY package.json package-lock.json ./   # rarely changes
RUN npm ci --only=production             # cached unless deps change
COPY . .                                 # source changes don't bust npm cache layer
```

### Strategy 2: Use .dockerignore

Reduces the build context size sent to the Docker daemon. Smaller context = faster build.

```
node_modules
.git
*.log
dist
coverage
.env
.DS_Store
__pycache__
*.pyc
.pytest_cache
tests/
docs/
```

### Strategy 3: Enable BuildKit

```bash
DOCKER_BUILDKIT=1 docker build .
# Docker 23+ has BuildKit enabled by default
```

BuildKit enables: parallel stage execution, better caching, cache mounts, and secrets mounting.

### Strategy 4: Cache mounts (BuildKit only)

Cache mounts persist the package manager's cache directory between builds. They are NOT
committed to the image layer — they are purely a build-time speed optimization.

```dockerfile
# Cache apt packages between builds
RUN --mount=type=cache,target=/var/cache/apt \
    apt-get update && apt-get install -y --no-install-recommends curl

# Cache npm downloads between builds
RUN --mount=type=cache,target=/root/.npm \
    npm ci --only=production

# Cache pip downloads between builds
RUN --mount=type=cache,target=/root/.cache/pip \
    pip install -r requirements.txt
```

### Strategy 5: Use registry cache in CI/CD

```bash
docker build \
  --cache-from type=registry,ref=registry.company.com/myapp:buildcache \
  --cache-to   type=registry,ref=registry.company.com/myapp:buildcache,mode=max \
  -t myapp:latest .
```

---

## SECTION 4 — HOW TO REDUCE IMAGE SIZE (was asked — critical)

### Technique 1: Use minimal base images

| Base Image | Approximate Size |
|------------|-----------------|
| ubuntu:22.04 | ~77 MB |
| debian:bookworm | ~117 MB |
| debian:bookworm-slim | ~74 MB |
| python:3.12 | ~1.0 GB |
| python:3.12-slim | ~130 MB |
| python:3.12-alpine | ~50 MB |
| alpine:3.19 | ~7 MB |
| gcr.io/distroless/static | ~2 MB |
| scratch | 0 MB (empty) |

### Technique 2: Combine RUN commands — cleanup must be in the SAME layer

Every RUN instruction creates a new image layer. If you install packages in one layer and clean
up in another, the packages are still present in the earlier layer — the cleanup has no effect
on total image size.

```dockerfile
# BAD — 3 separate layers; packages still baked into layer 1 even after cleanup
RUN apt-get update
RUN apt-get install -y curl build-essential
RUN rm -rf /var/lib/apt/lists/*

# GOOD — single layer; apt cache cleaned before the layer is committed
RUN apt-get update && \
    apt-get install -y --no-install-recommends curl build-essential && \
    rm -rf /var/lib/apt/lists/*
```

### Technique 3: Multi-stage builds

Use multiple FROM instructions. Earlier stages compile/build. The final stage only copies the
necessary artifacts — the build tools, source code, and intermediate files never appear in the
final image.

```dockerfile
# Stage 1: Build (has compiler, build tools, full source)
FROM golang:1.21-alpine AS builder
WORKDIR /build
COPY go.mod go.sum ./
RUN go mod download
COPY . .
RUN CGO_ENABLED=0 GOOS=linux go build -ldflags="-w -s" -o server ./cmd/server

# Stage 2: Final image (ONLY the binary — no Go toolchain, no source)
FROM gcr.io/distroless/static:nonroot
COPY --from=builder /build/server /server
ENTRYPOINT ["/server"]
# Result: ~8 MB instead of ~300 MB
```

### Technique 4: --no-install-recommends for apt

```dockerfile
RUN apt-get install -y --no-install-recommends \
    python3 python3-pip libpq5
```

### Technique 5: Run as non-root user (security + best practice)

```dockerfile
RUN addgroup --system appgroup && adduser --system --ingroup appgroup appuser
USER appuser
```

### Technique 6: Use specific version tags, not `latest`

```dockerfile
FROM python:3.12-slim   # NOT python:latest
```

Benefits: reproducible builds, predictable image size, known security surface.

---

## SECTION 5 — Docker Networking

**Q: What network drivers does Docker have? Explain each.**

```
bridge (default) — containers on the same bridge can communicate.
                   Docker manages DNS on user-defined bridges (containers reach each other by name).
                   Uses iptables for NAT to reach the outside world.
                   Each container gets an IP on 172.17.0.0/16 by default.

host             — container shares the host's network namespace entirely.
                   No NAT overhead. Port on container = port on host.
                   Use for: performance-critical apps, when you need host-level networking.

none             — no network interface except loopback. Fully isolated.

overlay          — multi-host networking for Docker Swarm. Uses VXLAN tunneling between hosts.

macvlan          — assigns a real MAC address to the container. Appears as a physical device
                   on the LAN. Use for: legacy apps that need direct LAN access.
```

---

**Q: What is the difference between EXPOSE and -p?**

```
EXPOSE     — documentation only. Tells developers/tools which port the app listens on.
             Does NOT make the port accessible from outside the container.
             Written in the Dockerfile.

-p 8080:80 — actually publishes the port by creating an iptables NAT rule on the host.
             Maps host port 8080 → container port 80.
             Passed at runtime with docker run.
```

---

**Q: How do containers communicate on the same host?**

On a user-defined bridge network, Docker provides DNS — containers reach each other by container
name automatically. The default bridge network does NOT have this; you must use IPs.

```bash
# Create a custom network (always do this, never use the default bridge in production)
docker network create myapp-net

docker run -d --name db  --network myapp-net postgres:16
docker run -d --name api --network myapp-net myapp:latest

# Inside the 'api' container, 'db' resolves by DNS:
# curl http://db:5432  -- works because Docker DNS resolves 'db' to the container IP
```

---

## SECTION 6 — Docker Volumes

**Q: Three ways to persist data in Docker. When to use each?**

```
1. Named Volumes  — docker run -v pgdata:/var/lib/postgresql/data postgres
                    Managed by Docker. Stored in /var/lib/docker/volumes/.
                    Best for production. Can be shared between containers.
                    Docker handles backup/restore with volume drivers.

2. Bind Mounts    — docker run -v $(pwd):/app myapp
                    Maps a specific host directory into the container.
                    Best for development (live code reload without rebuild).
                    The container can modify host files — use with caution in production.

3. tmpfs Mounts   — docker run --tmpfs /tmp:size=100m myapp
                    Stored in host memory only. Never written to disk.
                    Best for: secrets, sensitive session data, scratch space.
                    Data lost when container stops.
```

---

## SECTION 7 — PID 1 and Signal Handling

**Q: What is PID 1 in a container and why does it matter?**

A: PID 1 is the first process started inside the container. In Linux, PID 1 is special:
- It has NO default SIGTERM handler (unlike other processes which default to exiting on SIGTERM).
- If your process is PID 1 and doesn't explicitly handle SIGTERM, `docker stop` waits 10 seconds
  then sends SIGKILL. Your app gets no chance to flush data, close DB connections, etc.

**Solutions:**

1. Use exec form in ENTRYPOINT — your app becomes PID 1 directly:
   ```dockerfile
   ENTRYPOINT ["python", "app.py"]   # exec form — python is PID 1
   ```

2. Use `docker run --init` — adds `tini` as a minimal init process that forwards signals:
   ```bash
   docker run --init myapp
   ```

3. Use `exec "$@"` at the end of entrypoint shell scripts:
   ```bash
   #!/bin/sh
   set -e
   # do setup work here...
   exec "$@"    # replaces the shell with your process — it becomes PID 1
   ```

---

## SECTION 8 — Docker Security

**Q: How do you harden a Docker container?**

1. Run as non-root user: `USER appuser`
2. Read-only root filesystem: `docker run --read-only`
3. Drop Linux capabilities: `--cap-drop ALL --cap-add NET_BIND_SERVICE`
4. No new privileges: `--security-opt no-new-privileges:true`
5. Scan images for CVEs: `trivy image myapp:latest`, `docker scout cves myapp:latest`
6. Use minimal base images (smaller attack surface)
7. Never embed secrets in the image — use Docker secrets, environment variables from a vault
8. Use `.dockerignore` to prevent `.env`, SSH keys, credentials from entering the build context
9. Enable Docker Content Trust: `export DOCKER_CONTENT_TRUST=1`

---

## SECTION 9 — Docker Compose

```yaml
version: '3.9'

services:
  api:
    build:
      context: .
      dockerfile: Dockerfile
      target: production
    image: myapi:dev
    ports:
      - "8080:8080"
    environment:
      - DATABASE_URL=postgresql://postgres:password@db:5432/mydb
    env_file:
      - .env
    depends_on:
      db:
        condition: service_healthy
    volumes:
      - .:/app                  # bind mount for dev hot-reload
    networks:
      - backend
    restart: unless-stopped
    healthcheck:
      test: ["CMD", "curl", "-f", "http://localhost:8080/health"]
      interval: 30s
      timeout: 10s
      retries: 3
      start_period: 10s

  db:
    image: postgres:16-alpine
    environment:
      POSTGRES_DB: mydb
      POSTGRES_USER: postgres
      POSTGRES_PASSWORD: password
    volumes:
      - pgdata:/var/lib/postgresql/data
    networks:
      - backend
    healthcheck:
      test: ["CMD-SHELL", "pg_isready -U postgres"]
      interval: 10s
      timeout: 5s
      retries: 5

  redis:
    image: redis:7-alpine
    networks:
      - backend

networks:
  backend:
    driver: bridge

volumes:
  pgdata:
```

---

## SECTION 10 — Must-Know Docker Commands

```bash
# Build
docker build -t myapp:v1.0 .
docker build --no-cache -t myapp:v1.0 .
docker build --build-arg ENV=prod -t myapp:v1.0 .
docker build --target production .              # build specific stage

# Run
docker run -d -p 8080:80 --name webserver nginx
docker run -it --rm ubuntu bash                 # interactive, auto-remove on exit
docker run --env-file .env myapp:v1.0
docker run --init myapp:v1.0                    # tini as PID 1

# Inspect & Debug
docker logs -f --tail=100 webserver
docker exec -it webserver bash
docker inspect webserver                        # full JSON metadata
docker stats                                    # live CPU/memory per container
docker top webserver                            # processes running inside container

# Images
docker images
docker history myapp:v1.0                       # see each layer and its size
docker pull nginx:1.25-alpine
docker push myrepo/myapp:v1.0
docker rmi myapp:v1.0

# Networking
docker network ls
docker network create mynet
docker network inspect mynet

# Volumes
docker volume ls
docker volume create pgdata
docker volume inspect pgdata

# Cleanup
docker system prune -af --volumes               # remove ALL unused objects (nuclear)
docker image prune -af                          # unused images only
docker container prune -f                       # stopped containers only

# Save/Load (for air-gapped environments)
docker save myapp:v1.0 | gzip > myapp.tar.gz
docker load < myapp.tar.gz
```

---

## SECTION 11 — Tricky Interview Questions

**Q: What happens during `docker stop`?**

1. Docker sends SIGTERM to PID 1 inside the container.
2. Waits for `--time` seconds (default 10s) for the process to exit gracefully.
3. If still running: sends SIGKILL — process is force-killed immediately.

Your app MUST handle SIGTERM within the timeout window for graceful shutdown.

---

**Q: What is image layer caching and how is the cache invalidated?**

Docker computes a cache key for each layer using the instruction string and its inputs.
If the key matches a cached layer, Docker reuses it. Cache invalidation happens when:
- `COPY`/`ADD`: any file in the copied set changes (content hash changes)
- `RUN`: the command string itself changes
- `ARG`/`ENV` value changes
- Any layer above is invalidated (invalidation cascades downward)
- `--no-cache` flag disables all caching

---

**Q: Can two containers share the same network namespace?**

Yes. This is exactly how Kubernetes pods work — all containers in a pod share one network
namespace, so they communicate via `localhost`. In Docker:

```bash
docker run -d --name app1 myimage1
docker run -d --name app2 --network container:app1 myimage2
# app2 shares app1's network namespace — they share the same IP and ports
```

---

**Q: What is the difference between a Docker image and a Docker container?**

```
Image     — read-only template. A stack of immutable layers defined by a Dockerfile.
            Like a class definition in OOP.

Container — a running (or stopped) instance of an image. Has a thin writable layer on top.
            Like an object instantiated from a class.
            Multiple containers can run from the same image simultaneously.
```

---

**Q: What is a dangling image?**

A: An image layer that has no tag and is not referenced by any other image. Created when you
rebuild an image with the same tag — the old layers lose their tag reference.

```bash
docker images -f dangling=true          # list dangling images
docker image prune -f                   # remove all dangling images
docker system prune -f                  # remove dangling images + stopped containers
```
