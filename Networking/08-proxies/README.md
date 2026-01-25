# Proxies & Reverse Proxies

## What is a Proxy?

A proxy is an intermediary server that sits between clients and servers, forwarding requests and responses.

### The Proxy Concept

At its core, a proxy is a middleman:

```
Without Proxy:
Client ←------ Direct Connection ------→ Server

With Proxy:
Client ←--→ Proxy ←--→ Server
```

**Why Use a Middleman?**
1. **Control**: Proxy can inspect, modify, or block traffic
2. **Caching**: Proxy can store responses for faster access
3. **Security**: Proxy can filter malicious content
4. **Anonymity**: Hide client or server identity
5. **Load Distribution**: Distribute requests across servers

### Proxy vs Gateway vs Intermediary

**Terminology**:
- **Proxy**: Generic term for intermediary
- **Forward Proxy**: Sits in front of clients
- **Reverse Proxy**: Sits in front of servers
- **Gateway**: Can modify protocol (HTTP to HTTPS)
- **Tunnel**: Passes data without understanding it

## Forward Proxy vs Reverse Proxy

The key difference is **who it serves** and **where it sits**.

### Forward Proxy (Regular Proxy)

Sits in front of **clients**, makes requests on their behalf.

**Position in Network**:
```
Client (You) → Forward Proxy → Internet → Server

Client perspective: Proxy represents the server
Server perspective: Proxy IS the client
```

**What Happens**:
```
1. Client wants to access google.com
2. Client asks Forward Proxy: "Get me google.com"
3. Forward Proxy: Makes request to google.com
4. Forward Proxy: Receives response
5. Forward Proxy: Returns response to client

From google.com's view: Request came from proxy, not client
```

**Who Controls It**: Client's organization (company, ISP, user)

**Purpose from Client's Perspective**:
- Access control (block certain websites)
- Content filtering (block malware, adult content)
- Privacy (hide real IP address)
- Bypass geo-restrictions
- Caching (faster access to frequently visited sites)
- Bandwidth monitoring

**Corporate Forward Proxy Example**:
```
Employee Computer → Corporate Proxy → Internet

Proxy Rules:
- Block social media during work hours
- Block malware/phishing sites
- Log all internet activity
- Cache common downloads
- Enforce acceptable use policy
```

**Anonymous Forward Proxy Example**:
```
Your Computer (IP: 203.0.113.5)
       ↓
Forward Proxy (IP: 198.51.100.10)
       ↓
Website

Website logs: Request from 198.51.100.10 (not your real IP)
ISP sees: Connection to proxy (not final destination)
```

### Reverse Proxy

Sits in front of **servers**, receives requests on their behalf.

**Position in Network**:
```
Client → Internet → Reverse Proxy → Backend Server

Client perspective: Reverse proxy IS the server
Server perspective: Proxy represents the client
```

**What Happens**:
```
1. Client wants to access example.com
2. DNS resolves example.com to Reverse Proxy IP
3. Client connects to Reverse Proxy
4. Reverse Proxy: Receives request
5. Reverse Proxy: Forwards to appropriate backend server
6. Backend: Processes request
7. Reverse Proxy: Returns response to client

From client's view: Talked to example.com
Reality: Talked to reverse proxy, which talked to backend
```

**Who Controls It**: Server's organization (website owner, company)

**Purpose from Server's Perspective**:
- Load balancing (distribute traffic across servers)
- SSL termination (handle encryption once, not per server)
- Caching (reduce backend load)
- Security (hide backend topology, WAF, DDoS protection)
- Compression (compress responses once)
- Single entry point (one IP for multiple services)

**Reverse Proxy Architecture**:
```
Internet (Clients)
       ↓
Reverse Proxy (203.0.113.10) ← Only IP clients know
       ↓
    ┌──┴──┬───────┬────────┐
    ↓     ↓       ↓        ↓
Server1 Server2 Server3 Server4 (Private IPs: 10.0.1.x)
(API)   (API)   (Web)   (Web)

Clients can't directly access backend servers
All traffic goes through reverse proxy
```

### Key Differences

| Aspect | Forward Proxy | Reverse Proxy |
|--------|---------------|---------------|
| **Serves** | Clients | Servers |
| **Position** | Client-side | Server-side |
| **Controlled By** | Client's org | Server's org |
| **Hides** | Client identity | Server identity |
| **Client Knows?** | Yes (configured) | No (transparent) |
| **Purpose** | Access control, privacy | Load balancing, security |
| **Configuration** | Client-side | Server-side |
| **Examples** | Squid, corporate proxy | Nginx, HAProxy, Cloudflare |

### Transparent vs Explicit Proxy

**Explicit (Non-Transparent) Proxy**:
```
Client configuration:
- Proxy: proxy.company.com:8080
- Username: john
- Password: secret

Client knows about proxy and explicitly configured to use it
```

**Transparent Proxy**:
```
Network configuration:
- Router intercepts all HTTP traffic
- Redirects to proxy without client knowing

Client thinks: Direct connection to internet
Reality: All traffic goes through proxy
```

**How Transparent Proxy Works**:
```
1. Client sends packet: 203.0.113.5 → 93.184.216.34:80
2. Router intercepts at firewall
3. Router redirects to: proxy.internal:3128
4. Proxy processes and forwards
5. Proxy returns response as if from original server

Client never configured proxy - it's invisible
```

### Proxy Chain

Multiple proxies in sequence:

```
Client → Proxy 1 → Proxy 2 → Proxy 3 → Server

Each proxy only knows:
- Previous hop (where request came from)
- Next hop (where to send request)

Server sees: Request from Proxy 3
Proxy 3 sees: Request from Proxy 2
Proxy 2 sees: Request from Proxy 1
Proxy 1 sees: Request from Client

Result: Maximum anonymity (Tor uses this concept)
```

## Forward Proxy Use Cases

### 1. Corporate Network Access Control
```
Employee → Corporate Proxy → Internet
                ↓
         Block social media
         Log all requests
         Apply content filters
```

### 2. Hide Client IP
```
Your IP: 203.0.113.5
         ↓
    Forward Proxy
         ↓
Website sees: 198.51.100.10 (proxy IP)
```

### 3. Bypass Geographic Restrictions
```
You (in Country A) → Proxy (in Country B) → Service available in Country B
```

## Reverse Proxy Use Cases

### 1. Load Balancing
```
Client → Reverse Proxy → Backend 1
                      → Backend 2
                      → Backend 3
```

### 2. SSL Termination
```
HTTPS (encrypted) → Reverse Proxy → HTTP (unencrypted) → Backend
                   [SSL handled here]
```
Backend servers don't need to handle SSL/TLS.

### 3. Caching Static Content
```
Request for /static/logo.png
         ↓
    Reverse Proxy (has cached copy)
         ↓
    Returns immediately (no backend request)
```

### 4. Single Entry Point for Microservices
```
example.com/users    → User Service
example.com/orders   → Order Service
example.com/products → Product Service
           ↑
    Reverse Proxy (routes based on URL)
```

### 5. Security & DDoS Protection
```
Internet → Reverse Proxy (filters malicious traffic) → Backend
```

## Popular Proxy Solutions

### Nginx (Reverse Proxy)
- Most popular web server & reverse proxy
- High performance
- Easy to configure

### HAProxy
- Specialized load balancer & reverse proxy
- Very high performance
- Advanced features

### Apache (mod_proxy)
- Can act as reverse proxy
- Less efficient than Nginx for proxy workloads

### Squid (Forward Proxy)
- Caching proxy server
- Good for corporate networks
- Content filtering

### Envoy
- Modern cloud-native proxy
- Used in service mesh (Istio)
- Advanced traffic management

### Traefik
- Cloud-native reverse proxy
- Auto-discovery of services
- Great for Docker/Kubernetes

## Nginx as Reverse Proxy

### Basic Configuration
```nginx
server {
    listen 80;
    server_name example.com;

    location / {
        proxy_pass http://backend-server:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }
}
```

### Multiple Backend Services
```nginx
server {
    listen 80;
    server_name example.com;

    # API requests
    location /api/ {
        proxy_pass http://api-server:3000;
    }

    # Admin panel
    location /admin/ {
        proxy_pass http://admin-server:4000;
    }

    # Static files
    location /static/ {
        proxy_pass http://cdn-server:8080;
    }

    # Everything else
    location / {
        proxy_pass http://frontend-server:5000;
    }
}
```

### With Load Balancing
```nginx
upstream backend {
    server 10.0.1.10:8080;
    server 10.0.1.11:8080;
    server 10.0.1.12:8080;
}

server {
    listen 80;

    location / {
        proxy_pass http://backend;
    }
}
```

### With SSL/TLS Termination
```nginx
server {
    listen 443 ssl;
    server_name example.com;

    ssl_certificate /etc/ssl/certs/example.com.crt;
    ssl_certificate_key /etc/ssl/private/example.com.key;

    location / {
        # HTTPS from client, HTTP to backend
        proxy_pass http://backend:8080;
        proxy_set_header X-Forwarded-Proto https;
    }
}
```

### With Caching
```nginx
proxy_cache_path /var/cache/nginx levels=1:2 keys_zone=my_cache:10m;

server {
    listen 80;

    location / {
        proxy_cache my_cache;
        proxy_cache_valid 200 10m;
        proxy_cache_valid 404 1m;
        proxy_pass http://backend:8080;
    }
}
```

## Common Proxy Headers

When using a reverse proxy, important headers to forward:

```
X-Real-IP: 203.0.113.5              # Original client IP
X-Forwarded-For: 203.0.113.5        # Client IP (can be chain)
X-Forwarded-Proto: https            # Original protocol (http/https)
X-Forwarded-Host: example.com       # Original host
Host: backend.internal              # Target backend host
```

**Why important**: Backend needs to know the original client IP and protocol.

## Real-World Examples

### Example 1: Nginx as API Gateway
```
Mobile App     →
Web App        → Nginx (Reverse Proxy) → /api/v1/* → API v1
Desktop App    →                        → /api/v2/* → API v2
                                        → /auth/*   → Auth Service
```

### Example 2: Cloudflare as Reverse Proxy
```
User → Cloudflare (DDoS protection, CDN, caching) → Your Origin Server
```
Cloudflare acts as a reverse proxy for millions of websites.

### Example 3: Kubernetes Ingress
```
Internet → Ingress Controller (Nginx) → Service 1
                                      → Service 2
                                      → Service 3
```

### Example 4: Corporate Forward Proxy
```
Employee PC → Squid Proxy → Internet
                   ↓
              Logs requests
              Blocks malware
              Caches content
```

## Proxy vs VPN vs Firewall

| Feature | Proxy | VPN | Firewall |
|---------|-------|-----|----------|
| Scope | Application-level | System-wide | Network-level |
| Encryption | Optional | Yes | No |
| Speed | Fast | Slower | N/A |
| Use Case | HTTP traffic | All traffic | Traffic filtering |

## Transparent Proxy

Intercepts traffic without client configuration.

```
Client thinks: Direct connection to website
Reality: Traffic goes through proxy
```

**Use case**: Corporate networks where admin wants to monitor all traffic.

## SOCKS Proxy

More versatile than HTTP proxy - works with any protocol.

```
SOCKS5 Proxy
    ↓
Can handle: HTTP, HTTPS, FTP, SSH, etc.
```

**Example**: SSH dynamic port forwarding creates a SOCKS proxy.
```bash
ssh -D 8080 user@server
# Creates SOCKS proxy on localhost:8080
```

## Proxy Commands

### Test proxy connectivity:
```bash
# Use proxy for curl
curl -x http://proxy-server:8080 https://example.com

# With authentication
curl -x http://user:pass@proxy-server:8080 https://example.com

# Set proxy via environment variable
export http_proxy=http://proxy-server:8080
export https_proxy=http://proxy-server:8080
curl https://example.com
```

### Configure system-wide proxy (Linux):
```bash
# Edit /etc/environment
http_proxy="http://proxy-server:8080"
https_proxy="http://proxy-server:8080"
no_proxy="localhost,127.0.0.1,*.local"
```

### Nginx proxy commands:
```bash
# Test configuration
sudo nginx -t

# Reload configuration
sudo nginx -s reload

# Check if Nginx is running
sudo systemctl status nginx

# View error logs
sudo tail -f /var/log/nginx/error.log

# View access logs
sudo tail -f /var/log/nginx/access.log
```

## Proxy Caching

Reduces backend load by caching responses.

### Cache Decision Flow
```
1. Request comes in
2. Check if cached
   → Yes: Return cached response (cache hit)
   → No: Forward to backend, cache response (cache miss)
3. Return response to client
```

### Cache Headers
```
Cache-Control: public, max-age=3600    # Cache for 1 hour
Cache-Control: private, no-cache       # Don't cache
Cache-Control: no-store                # Never store
```

## Troubleshooting

### Issue: 502 Bad Gateway
**Meaning**: Proxy can't reach backend.

```bash
# Check if backend is running
curl http://backend:8080

# Check backend logs
docker logs backend-container

# Check network connectivity
ping backend-server
```

### Issue: Request Headers Not Forwarded
**Solution**: Add proxy headers in Nginx config:
```nginx
proxy_set_header Host $host;
proxy_set_header X-Real-IP $remote_addr;
proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
```

### Issue: WebSocket Connection Fails
**Solution**: Enable WebSocket support:
```nginx
location /ws/ {
    proxy_pass http://backend;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
}
```

### Issue: Large File Upload Fails
**Solution**: Increase client body size:
```nginx
client_max_body_size 100M;
```

## Security Considerations

### 1. Hide Backend Server Information
```nginx
proxy_hide_header X-Powered-By;
server_tokens off;
```

### 2. Rate Limiting
```nginx
limit_req_zone $binary_remote_addr zone=mylimit:10m rate=10r/s;

location / {
    limit_req zone=mylimit burst=20;
    proxy_pass http://backend;
}
```

### 3. IP Whitelisting
```nginx
location /admin/ {
    allow 203.0.113.0/24;    # Office IP range
    deny all;
    proxy_pass http://backend;
}
```

### 4. Add Security Headers
```nginx
add_header X-Frame-Options "SAMEORIGIN";
add_header X-Content-Type-Options "nosniff";
add_header X-XSS-Protection "1; mode=block";
```

## DevOps Use Cases

### 1. Blue-Green Deployments
```nginx
# Switch traffic by changing proxy_pass
upstream backend {
    server blue-env:8080;    # Current version
    # server green-env:8080; # New version (commented out)
}
```

### 2. Canary Releases
```nginx
# 90% traffic to stable, 10% to canary
upstream backend {
    server stable:8080 weight=9;
    server canary:8080 weight=1;
}
```

### 3. A/B Testing
```nginx
# Route based on cookie or IP
if ($cookie_version = "B") {
    proxy_pass http://version-b;
}
proxy_pass http://version-a;
```

## Key Takeaways

✅ Forward proxy sits in front of clients (client-side)  
✅ Reverse proxy sits in front of servers (server-side)  
✅ Reverse proxies handle: load balancing, SSL termination, caching  
✅ Nginx is the most popular reverse proxy  
✅ Always forward X-Real-IP and X-Forwarded-For headers  
✅ Caching at proxy level reduces backend load  
✅ Proxies provide single entry point for multiple services  
✅ Use reverse proxies for security and traffic management
