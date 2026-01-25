# Load Balancing

## What is Load Balancing?

Load balancing distributes incoming traffic across multiple servers to ensure no single server gets overwhelmed.

Load balancing is a fundamental concept in distributed systems that solves several critical problems:

1. **Scalability**: One server has limits; multiple servers scale horizontally
2. **Availability**: If one server fails, others handle the traffic
3. **Performance**: Distributing load prevents bottlenecks
4. **Resource Optimization**: Efficiently utilize all available servers

**The Core Problem**:
```
Without Load Balancer:
All traffic → Single Server
- Server gets 1000 req/s
- Server can handle 500 req/s
- Result: 500 req/s dropped, slow responses, server crash

With Load Balancer:
Traffic → Load Balancer → Server 1 (333 req/s)
                       → Server 2 (333 req/s)
                       → Server 3 (333 req/s)
- Each server within capacity
- Fast responses
- System remains stable
```

### Load Balancing Theory

**Horizontal Scaling vs Vertical Scaling**:

**Vertical Scaling** (Scale Up):
```
Upgrade one server: More CPU, RAM, Disk
- Limited by hardware maximums
- Expensive
- Single point of failure
- Downtime during upgrades
```

**Horizontal Scaling** (Scale Out):
```
Add more servers of same size
- Nearly unlimited scaling
- Cost-effective (commodity hardware)
- No single point of failure
- No downtime to add servers
- Requires load balancing
```

Load balancing enables horizontal scaling.

### Types of Load

```
                    Load Balancer
                         |
        ┌────────────────┼────────────────┐
        |                |                |
    Server 1         Server 2         Server 3
    (Active)         (Active)         (Active)
```

**Flow**:
1. User sends request to load balancer
2. Load balancer picks a server (using an algorithm)
3. Request is forwarded to chosen server
4. Server processes and responds
5. Load balancer returns response to user

## Load Balancing Algorithms

Load balancing algorithms determine how traffic is distributed. Each algorithm optimizes for different scenarios.

### 1. Round Robin

Distributes requests equally in sequence, like dealing cards.

**Algorithm**:
```
servers = [Server1, Server2, Server3]
current = 0

for each request:
    send to servers[current]
    current = (current + 1) % number_of_servers
```

**Example**:
```
Request 1 → Server 1
Request 2 → Server 2
Request 3 → Server 3
Request 4 → Server 1 (cycle repeats)
Request 5 → Server 2
```

**Advantages**:
- Simple to implement
- Fair distribution over time
- No overhead

**Disadvantages**:
- Doesn't consider server load
- Doesn't consider request complexity
- Treats all requests as equal

**Use case**: All servers have similar capacity and all requests take similar time.

### 2. Least Connections

Sends traffic to server with fewest active connections.

**Algorithm**:
```
for each request:
    find server with minimum active_connections
    send request to that server
    increment active_connections for that server
    
when response completes:
    decrement active_connections
```

**Example**:
```
Server 1: 5 active connections
Server 2: 2 active connections ← Choose this
Server 3: 8 active connections

New request → Server 2 (now has 3 connections)
```

**Theory**:
- **Assumption**: More connections = more load
- **Reality**: Works well when request times vary
- **Problem**: Doesn't consider connection "weight" (some connections do more work)

**Advantages**:
- Better than round-robin for varying request times
- Adapts to actual server load
- Prevents overloading of servers

**Disadvantages**:
- More complex
- Requires tracking connection count
- May not reflect actual CPU/memory usage

**Use case**: Long-lived connections with varying processing times (database queries, API calls).

### 3. Weighted Round Robin

Servers with higher capacity get more requests.

**Algorithm**:
```
servers = [
    {server: Server1, weight: 3},
    {server: Server2, weight: 2},
    {server: Server3, weight: 1}
]

total_weight = 6
Server1 gets 3/6 = 50% of traffic
Server2 gets 2/6 = 33% of traffic
Server3 gets 1/6 = 17% of traffic
```

**Example**:
```
Server 1 (weight: 3) → Gets requests: 1, 2, 3, 7, 8, 9
Server 2 (weight: 2) → Gets requests: 4, 5, 10, 11
Server 3 (weight: 1) → Gets requests: 6, 12
```

**Why Weights?**
```
Server 1: 32 GB RAM, 16 CPUs → Weight 3
Server 2: 16 GB RAM, 8 CPUs  → Weight 2
Server 3: 8 GB RAM, 4 CPUs   → Weight 1

Distribute proportionally to capacity
```

**Use case**: Servers with different capacities (mixed hardware, spot instances).

### 4. IP Hash

Routes same client IP to same server.

**Algorithm**:
```
hash_value = hash(client_ip)
server_index = hash_value % number_of_servers
send to servers[server_index]
```

**Example**:
```
Client 203.0.113.5:
hash(203.0.113.5) = 12345
12345 % 3 = 0
Always goes to Server 0

Client 198.51.100.10:
hash(198.51.100.10) = 67890
67890 % 3 = 0
Always goes to Server 0 (collision, but consistent)
```

**Advantages**:
- Provides **session affinity** without cookies
- Same client always hits same server (useful for caching)
- Stateless (no server-side session tracking)

**Disadvantages**:
- Uneven distribution (hash collisions)
- Adding/removing servers changes all mappings
- Many clients behind same NAT hit same server

**Use case**: Session persistence without sticky sessions, local caching on servers.

### 5. Least Response Time

Routes to server with fastest response time and fewest connections.

**Algorithm**:
```
for each request:
    for each server:
        score = active_connections * average_response_time
    send to server with lowest score
```

**Example**:
```
Server 1: 5 connections, 100ms avg → Score: 500
Server 2: 2 connections, 50ms avg  → Score: 100 ← Choose this
Server 3: 3 connections, 200ms avg → Score: 600
```

**Theory**:
Combines both connection count and actual performance into decision.

**Advantages**:
- Most intelligent algorithm
- Adapts to actual server performance
- Considers both load and speed

**Disadvantages**:
- Most complex
- Requires monitoring response times
- Overhead of calculation

**Use case**: Heterogeneous environments, performance-critical applications.

### 6. Random

Randomly selects a server.

**Algorithm**:
```
server_index = random(0, number_of_servers - 1)
send to servers[server_index]
```

**Surprisingly Effective**:
- With enough requests, distribution becomes even
- Law of large numbers
- No state needed

**Advantages**:
- Simplest algorithm
- No synchronization needed
- Works well at scale

**Disadvantages**:
- Short-term unevenness
- No consideration of server state

**Use case**: Stateless microservices with many small requests

## Types of Load Balancers

Load balancers operate at different layers of the OSI model, with different capabilities.

### Layer 4 (Transport Layer) - TCP/UDP Load Balancing

**What it does**:
- Routes based on IP address and port number
- No understanding of application protocol
- Looks at TCP/UDP headers only

**How it works**:
```
Client connects to: 203.0.113.10:80
Load Balancer sees:
- Source: 198.51.100.5:54321
- Destination: 203.0.113.10:80
- Protocol: TCP

Decisions based only on:
- IP addresses
- Port numbers
- Connection count
- No inspection of HTTP headers or payload
```

**Packet Flow**:
```
1. Client sends TCP SYN to load balancer
2. Load balancer chooses backend server
3. Load balancer forwards SYN to backend
4. Backend responds with SYN-ACK
5. Load balancer forwards SYN-ACK to client
6. Connection established through load balancer
```

**Advantages**:
- **Fast**: Minimal processing per packet
- **Protocol agnostic**: Works with any TCP/UDP application
- **Low latency**: Simple packet forwarding
- **High throughput**: Can handle millions of connections
- **Secure**: Doesn't need to decrypt HTTPS

**Disadvantages**:
- **No content-based routing**: Can't route by URL or header
- **No caching**: Doesn't understand content
- **Limited health checks**: TCP connection only
- **No SSL termination**: Passes through encrypted traffic

**Use cases**:
- High-performance requirements
- Non-HTTP protocols (databases, game servers, SSH)
- Simple load distribution
- Pass-through TLS/SSL

**Example**: AWS NLB (Network Load Balancer), HAProxy in TCP mode

### Layer 7 (Application Layer) - HTTP/HTTPS Load Balancing

**What it does**:
- Routes based on HTTP content (URLs, headers, cookies)
- Full understanding of HTTP protocol
- Can modify requests/responses

**How it works**:
```
Client → Load Balancer (terminates HTTP connection)

Load Balancer examines:
- HTTP method (GET, POST)
- URL path (/api/users, /api/orders)
- Headers (Host, User-Agent, Cookie)
- Query parameters
- Body content (JSON, XML)

Makes routing decision based on content
Opens new connection to backend
```

**Request Inspection**:
```
GET /api/users/123 HTTP/1.1
Host: api.example.com
Cookie: session=abc123
User-Agent: Mozilla/5.0

Load Balancer can route based on:
- Path starts with /api/users → User Service
- Path starts with /api/orders → Order Service
- Cookie contains session → Sticky session
- User-Agent contains "Mobile" → Mobile backend
- Header contains "Admin" → Admin backend
```

**Advantages**:
- **Smart routing**: Route by URL, header, cookie, method
- **SSL termination**: Decrypt once at load balancer, backend uses HTTP
- **Caching**: Cache responses at load balancer
- **Compression**: Compress responses
- **Security**: WAF, rate limiting, authentication
- **Content modification**: Rewrite headers, inject content

**Disadvantages**:
- **Slower**: Must parse HTTP, more processing
- **SSL overhead**: Must decrypt/re-encrypt (if backend HTTPS)
- **Connection termination**: Breaks end-to-end connection
- **More complex**: More configuration options
- **Higher cost**: Requires more resources

**Use cases**:
- Microservices (route by path)
- A/B testing (route by cookie/header)
- Geographic routing (route by IP)
- Content-based routing
- SSL offloading

**Example**: AWS ALB (Application Load Balancer), Nginx, Apache

### Layer 4 vs Layer 7 Comparison

**Performance**:
```
Layer 4: 1,000,000+ connections/second
Layer 7: 100,000+ connections/second

Layer 4 is 10x faster but less intelligent
```

**Routing Capabilities**:
```
Layer 4:
- IP:Port → Backend pool

Layer 7:
- example.com/api/* → API servers
- example.com/static/* → Static file servers
- example.com (Header: Mobile) → Mobile servers
- example.com (Cookie: premium) → Premium servers
```

**Health Checks**:
```
Layer 4:
- TCP connection succeeds → Healthy
- TCP connection fails → Unhealthy

Layer 7:
- HTTP GET /health returns 200 → Healthy
- Check response body, check response time
- Much more detailed health assessment
```

**When to Use Each**:

**Use Layer 4 When**:
- Maximum performance needed
- Simple load distribution sufficient
- Non-HTTP protocols
- Don't need to inspect traffic
- Want minimal latency

**Use Layer 7 When**:
- Need content-based routing
- Multiple services behind one IP
- Want SSL termination
- Need caching or compression
- Microservices architecture
- Need advanced features (WAF, rate limiting)

## Real-World Examples

### Example 1: Basic Web Application
```
Internet
   |
   ↓
Load Balancer (nginx/HAProxy)
   |
   ├→ Web Server 1 (app:8080)
   ├→ Web Server 2 (app:8080)
   └→ Web Server 3 (app:8080)
```

### Example 2: Microservices Architecture
```
API Gateway (Load Balancer)
   |
   ├→ /users/* → User Service (3 instances)
   ├→ /orders/* → Order Service (2 instances)
   └→ /products/* → Product Service (4 instances)
```

### Example 3: Multi-Region Setup
```
Global Load Balancer (Route53, Cloudflare)
   |
   ├→ US East → Regional Load Balancer → Servers
   ├→ EU West → Regional Load Balancer → Servers
   └→ Asia Pacific → Regional Load Balancer → Servers
```

## Health Checks

Load balancers regularly check if servers are healthy and able to handle traffic. This is critical for high availability.

### Why Health Checks Matter

**Without Health Checks**:
```
Server 1: ✓ Running
Server 2: ✗ Crashed
Server 3: ✓ Running

Load Balancer still sends traffic to Server 2
→ 33% of requests fail
→ Users see errors
```

**With Health Checks**:
```
Server 1: ✓ Running (Healthy)
Server 2: ✗ Crashed (Unhealthy) ← Removed from pool
Server 3: ✓ Running (Healthy)

Load Balancer only sends to Server 1 and Server 3
→ 0% of requests fail
→ Users don't notice Server 2 is down
```

### Types of Health Checks

### 1. TCP Health Check (Layer 4)

**How it works**:
```
Every X seconds:
  Load Balancer → TCP SYN to Server:Port
  
  If Server responds with SYN-ACK:
    Server is Healthy
  
  If timeout or connection refused:
    Server is Unhealthy
```

**Example**:
```
Check: Connect to 10.0.1.5:8080
Success → Healthy (server is listening)
Timeout → Unhealthy (server down or port closed)
```

**Advantages**:
- Fast and simple
- Low overhead
- Works for any TCP service

**Disadvantages**:
- Only checks if port is open
- Doesn't verify application is working
- False positives possible

**Use case**: Basic connectivity checks, non-HTTP services.

### 2. HTTP/HTTPS Health Check (Layer 7)

**How it works**:
```
Every X seconds:
  Load Balancer → GET /health HTTP/1.1
  
  Server responds:
    HTTP/1.1 200 OK
    {"status": "healthy"}
  
  Load Balancer checks:
    - Status code (is it 200?)
    - Response time (< timeout?)
    - Response body (matches expected?)
  
  If all pass:
    Server is Healthy
  Else:
    Server is Unhealthy
```

**Example Health Check Endpoint**:
```json
GET /health

Response (Healthy):
HTTP/1.1 200 OK
{
  "status": "healthy",
  "database": "connected",
  "cache": "connected",
  "disk_space": "sufficient"
}

Response (Unhealthy):
HTTP/1.1 503 Service Unavailable
{
  "status": "unhealthy",
  "database": "disconnected",
  "cache": "connected"
}
```

**Advantages**:
- Application-level check
- Can verify dependencies (database, cache, etc.)
- Can check application logic
- Detailed status information

**Disadvantages**:
- Higher overhead
- More complex to implement
- Slower than TCP check

**Use case**: Web applications, APIs, microservices.

### Health Check Parameters

**Interval**: Time between checks
```
Interval: 10 seconds
Load balancer checks every 10 seconds

Shorter interval:
  ✓ Faster detection of failures
  ✗ More overhead

Longer interval:
  ✓ Less overhead
  ✗ Slower detection
```

**Timeout**: Maximum wait time for response
```
Timeout: 5 seconds
If server doesn't respond in 5 seconds → Unhealthy

Short timeout:
  ✓ Quick failure detection
  ✗ May mark slow (but working) servers as unhealthy

Long timeout:
  ✓ Tolerates slow responses
  ✗ Slow to detect failures
```

**Unhealthy Threshold**: Consecutive failures before marking unhealthy
```
Unhealthy Threshold: 3
Server must fail 3 consecutive checks to be marked unhealthy

Why not 1?
- Prevents false positives
- Network blips don't take server out
- Temporary CPU spike doesn't remove server
```

**Healthy Threshold**: Consecutive successes before marking healthy
```
Healthy Threshold: 2
Server must pass 2 consecutive checks to be marked healthy

Why not 1?
- Prevents flapping
- Ensures server is stable
- Gives server time to warm up
```

### Health Check State Machine

```
         Initial State
              |
              ↓
         [Healthy] ←──────────────┐
              |                   |
    Fail (count = 1)         Pass (count = 1)
              ↓                   |
    Fail (count = 2)         Pass (count = 2)
              ↓                   |
    Fail (count = 3)         [Healthy Threshold Met]
              ↓                   
        [Unhealthy] ────────────→
```

### Advanced Health Checks

**1. Active vs Passive Health Checks**

**Active**:
```
Load balancer proactively sends health check requests
Regular intervals (e.g., every 10 seconds)
Predictable overhead
```

**Passive** (Traffic-based):
```
Load balancer monitors actual traffic
If server returns errors → Mark unhealthy
No extra requests
Only works when traffic exists
```

**2. Graceful Degradation**

```
Health endpoint can return different statuses:
- 200: Fully healthy (accept all traffic)
- 429: Degraded (accept reduced traffic)
- 503: Unhealthy (remove from pool)

Advanced load balancers respect this
```

**3. Dependency Checks**

Health endpoint checks critical dependencies:
```python
def health_check():
    checks = {
        "database": check_database(),
        "cache": check_cache(),
        "external_api": check_external_api()
    }
    
    if all(checks.values()):
        return 200, "healthy"
    elif checks["database"]:  # Database is critical
        return 503, "unhealthy"
    else:  # Non-critical service down
        return 429, "degraded"
```

### Best Practices

**1. Separate Health Check Endpoint**
```
✓ /health or /healthz
✗ / (homepage, too expensive to render)
```

**2. Lightweight Health Checks**
```
✓ Quick database query (SELECT 1)
✗ Full database scan
✓ Ping cache
✗ Populate entire cache
```

**3. Include Warmup Time**
```
Server starts → 30 seconds to warm up caches
Health threshold: 3 checks
Check interval: 15 seconds

Takes 45 seconds before marked healthy
Gives server time to prepare
```

**4. Monitor Health Check Metrics**
```
Track:
- Health check success rate
- Response times
- Flapping (healthy ↔ unhealthy rapidly)
```

**5. Different Checks for Different Needs**
```
Liveness: Is the process running? (restart if fails)
Readiness: Is it ready for traffic? (remove from pool if fails)
Startup: Has it finished initializing? (wait before checking)
```

## Session Persistence (Sticky Sessions)

Keep user connected to same server for entire session.

### Cookie-Based Sticky Sessions
```
First Request:
Client → Load Balancer → Server 2
Response: Set-Cookie: SERVER_ID=server2

Subsequent Requests:
Client (Cookie: SERVER_ID=server2) → Load Balancer → Server 2
```

**Pros**: User session data stays on one server  
**Cons**: Uneven load distribution, no failover

## Popular Load Balancers

### Software Load Balancers
- **Nginx**: Web server + reverse proxy + load balancer
- **HAProxy**: High-performance load balancer
- **Traefik**: Modern cloud-native load balancer
- **Envoy**: Cloud-native proxy (used in service mesh)

### Cloud Load Balancers
- **AWS**: ALB (Layer 7), NLB (Layer 4), ELB (Classic)
- **Azure**: Application Gateway, Load Balancer
- **GCP**: Cloud Load Balancing

### Kubernetes
- **Service**: Internal load balancing
- **Ingress**: External HTTP/HTTPS routing
- **Service Mesh**: Advanced traffic management (Istio, Linkerd)

## Simple Nginx Load Balancer Config

```nginx
upstream backend {
    # Load balancing algorithm (default: round-robin)
    # least_conn;
    # ip_hash;
    
    server 10.0.1.10:8080;
    server 10.0.1.11:8080;
    server 10.0.1.12:8080;
}

server {
    listen 80;
    
    location / {
        proxy_pass http://backend;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

## Load Balancing Patterns

### 1. Active-Active
All servers handle traffic simultaneously.
```
Load Balancer → [Server 1, Server 2, Server 3] (all active)
```

### 2. Active-Passive
One server handles traffic, others are standby.
```
Load Balancer → Server 1 (active)
                Server 2 (standby)
```

### 3. Geographic Load Balancing
Route users to nearest region.
```
User in USA → US Load Balancer
User in Europe → EU Load Balancer
```

## Auto-Scaling with Load Balancers

```
1. Load increases → Metrics trigger scaling
2. New servers are launched
3. Load balancer adds them to pool (after health check)
4. Traffic distributed to all servers
5. Load decreases → Servers removed
```

## Common Commands & Tools

### Test load balancing:
```bash
# Send multiple requests and see which server responds
for i in {1..10}; do
  curl -s http://loadbalancer.example.com | grep "Server ID"
done

# Test with different IPs (if using IP hash)
curl --interface eth0 http://loadbalancer.example.com
curl --interface eth1 http://loadbalancer.example.com
```

### Check load balancer status:
```bash
# HAProxy stats page
curl http://loadbalancer:9000/stats

# Check backend servers
curl -I http://backend-server:8080/health
```

## Troubleshooting

### Issue: Uneven Load Distribution
**Possible causes**:
- Sticky sessions enabled
- Long-lived connections
- IP hash with few clients

**Solution**: Change algorithm or disable sticky sessions.

### Issue: Server Marked Unhealthy
```bash
# Check if server is actually running
curl http://backend-server:8080/health

# Check health check configuration
# Ensure timeout and interval are appropriate
```

### Issue: 502 Bad Gateway
**Possible causes**:
- All backend servers are down
- Backend servers timing out
- Network issue between LB and servers

**Solution**: Check backend logs and connectivity.

## Key Takeaways

✅ Load balancers distribute traffic across servers  
✅ Round-robin is simple, least-connections adapts to load  
✅ Layer 4 is fast, Layer 7 offers more features  
✅ Health checks ensure traffic only goes to healthy servers  
✅ Session persistence can cause uneven distribution  
✅ Auto-scaling works hand-in-hand with load balancing  
✅ Always have multiple backend servers for high availability
