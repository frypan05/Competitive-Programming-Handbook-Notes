# HTTP/HTTPS & Web Protocols

## What is HTTP?

HTTP (HyperText Transfer Protocol) is the foundation of data communication on the web. It's how your browser talks to web servers.

HTTP is an **application-layer protocol** that defines how messages are formatted and transmitted between clients (usually web browsers) and servers. It was invented by Tim Berners-Lee in 1989 and has evolved significantly since then.

### Core Characteristics of HTTP

**1. Client-Server Model**
```
Client (initiates)  ←→  Server (responds)
- Browser               - Web Server
- Mobile App            - API Server
- CLI tool (curl)       - Backend Service
```

**2. Request-Response Protocol**
- Client sends a **request**
- Server processes and sends a **response**
- Each transaction is independent (unless keep-alive is used)

**3. Stateless Protocol**
HTTP itself has no memory of previous requests:
```
Request 1: GET /page1 → Server responds
Request 2: GET /page2 → Server has no memory of Request 1
```

**Why Stateless?**
- **Simplicity**: Server doesn't need to maintain state
- **Scalability**: Any server can handle any request
- **Reliability**: No issues if server restarts

**Problem**: Web apps need state (shopping carts, login sessions)

**Solutions**:
- **Cookies**: Client stores state
- **Sessions**: Server stores state, client holds session ID
- **Tokens**: JWT, OAuth tokens carry state
- **Local Storage**: Client-side state

**4. Text-Based Protocol (HTTP/1.x)**
HTTP/1.x messages are human-readable text:
```
GET /index.html HTTP/1.1
Host: www.example.com
User-Agent: Mozilla/5.0
```

**Note**: HTTP/2 and HTTP/3 use binary format for efficiency.

### How HTTP Works (Request-Response Cycle)

```
Step 1: DNS Resolution
www.example.com → 93.184.216.34

Step 2: TCP Connection
Client establishes TCP connection to server:443

Step 3: HTTP Request
Client sends HTTP request over TCP connection

Step 4: Server Processing
Server processes request, accesses resources

Step 5: HTTP Response
Server sends response back to client

Step 6: Connection Handling
- HTTP/1.0: Connection closes
- HTTP/1.1+: Connection may stay open (keep-alive)
```

| HTTP | HTTPS |
|------|-------|
| Port 80 | Port 443 |
| Unencrypted | Encrypted (TLS/SSL) |
| Fast | Slightly slower (encryption overhead) |
| Insecure | Secure |
| `http://example.com` | `https://example.com` |

**Rule of thumb**: Always use HTTPS for production.

## How HTTP Works

```
Client                              Server
  |                                   |
  |------- HTTP Request ------------->|
  |  GET /api/users HTTP/1.1          |
  |  Host: api.example.com            |
  |                                   |
  |<------ HTTP Response -------------|
  |  HTTP/1.1 200 OK                  |
  |  Content-Type: application/json   |
  |  { "users": [...] }               |
```

## HTTP Methods

| Method | Purpose | Example Use Case |
|--------|---------|------------------|
| **GET** | Retrieve data | Fetch user profile |
| **POST** | Create new resource | Register new user |
| **PUT** | Update entire resource | Update user profile |
| **PATCH** | Partially update resource | Update user email only |
| **DELETE** | Delete resource | Delete user account |
| **HEAD** | Get headers only | Check if file exists |
| **OPTIONS** | Get allowed methods | CORS preflight |

## HTTP Status Codes

### 2xx - Success
- **200 OK**: Request succeeded
- **201 Created**: Resource created successfully
- **204 No Content**: Success, but no content to return

### 3xx - Redirection
- **301 Moved Permanently**: Resource moved (update your bookmarks)
- **302 Found**: Temporary redirect
- **304 Not Modified**: Use cached version

### 4xx - Client Errors
- **400 Bad Request**: Invalid request syntax
- **401 Unauthorized**: Authentication required
- **403 Forbidden**: You don't have permission
- **404 Not Found**: Resource doesn't exist
- **429 Too Many Requests**: Rate limit exceeded

### 5xx - Server Errors
- **500 Internal Server Error**: Server crashed
- **502 Bad Gateway**: Gateway/proxy error
- **503 Service Unavailable**: Server overloaded or down
- **504 Gateway Timeout**: Gateway/proxy timeout

## HTTP Request Structure

```
GET /api/users/123 HTTP/1.1
Host: api.example.com
User-Agent: Mozilla/5.0
Accept: application/json
Authorization: Bearer eyJhbGc...
Content-Type: application/json

{
  "name": "John Doe"
}
```

**Parts**:
1. Request line: Method, path, version
2. Headers: Metadata about request
3. Body: Data (for POST/PUT/PATCH)

## HTTP Response Structure

```
HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 156
Cache-Control: max-age=3600
Set-Cookie: sessionId=abc123

{
  "id": 123,
  "name": "John Doe",
  "email": "john@example.com"
}
```

## Important HTTP Headers

### Request Headers
```
Host: api.example.com                    # Target server
User-Agent: curl/7.64.1                  # Client info
Accept: application/json                 # Expected response format
Authorization: Bearer token123           # Authentication
Content-Type: application/json           # Body format
Cookie: sessionId=abc123                 # Session data
```

### Response Headers
```
Content-Type: application/json           # Response format
Content-Length: 1234                     # Size in bytes
Cache-Control: max-age=3600              # Caching rules
Set-Cookie: sessionId=xyz789             # Set cookie
Access-Control-Allow-Origin: *           # CORS policy
X-RateLimit-Remaining: 99                # Rate limit info
```

## HTTPS & TLS/SSL

### How HTTPS Works

HTTPS = HTTP + TLS (Transport Layer Security)

**The TLS Handshake Process**:

```
1. Client Hello
Client → Server: "I want HTTPS. I support these cipher suites..."
- TLS version supported
- List of cipher suites (encryption algorithms)
- Random number (for key generation)

2. Server Hello
Server → Client: "Let's use TLS 1.3 with AES-256-GCM"
- Chosen TLS version
- Chosen cipher suite
- Random number
- **SSL Certificate** (contains public key)

3. Certificate Verification
Client verifies certificate:
- Is it signed by trusted CA?
- Is it for the correct domain?
- Is it expired?
- Has it been revoked?

4. Key Exchange
Client generates pre-master secret:
- Encrypts it with server's public key (from certificate)
- Sends to server
- Only server can decrypt (has private key)

5. Session Keys Generated
Both sides independently generate session keys:
- Using: client random + server random + pre-master secret
- Results in same symmetric encryption key

6. Encrypted Communication Begins
All further communication encrypted with session key
```

### SSL Certificates Explained

An SSL certificate is a digital document that:
1. **Proves identity**: This server is really example.com
2. **Contains public key**: Used for initial encryption
3. **Is signed by CA**: Trusted third party vouches for it

**Certificate Contents**:
```
Subject: example.com
Issuer: Let's Encrypt Authority X3
Valid From: 2024-01-01
Valid Until: 2024-04-01
Public Key: [4096-bit RSA key]
Signature: [CA's digital signature]
```

**Certificate Chain**:
```
Your Certificate (example.com)
    ↓ Signed by
Intermediate Certificate (Let's Encrypt Authority)
    ↓ Signed by
Root Certificate (ISRG Root X1)
    ↓
Trusted by browsers (pre-installed)
```

**Types of Certificates**:

1. **Domain Validation (DV)**
   - Verifies domain ownership only
   - Free (Let's Encrypt)
   - Quick to obtain

2. **Organization Validation (OV)**
   - Verifies organization exists
   - Shows organization name
   - Moderate cost

3. **Extended Validation (EV)**
   - Rigorous verification process
   - Shows green bar in browser (older browsers)
   - Expensive
   - Example: Banks, financial institutions

**Wildcard Certificates**:
```
Certificate for: *.example.com
Covers:
- api.example.com
- www.example.com
- blog.example.com

Does NOT cover:
- example.com (apex domain)
- sub.api.example.com (nested subdomain)
```

### Why Encryption Performance Isn't a Concern Anymore

**Old Myth**: "HTTPS is slow because of encryption overhead"

**Reality** (Modern HTTPS):
1. **TLS 1.3**: Faster handshake (1 RTT vs 2 RTT)
2. **Session Resumption**: Reuse session keys (0 RTT)
3. **Hardware Acceleration**: CPUs have AES instructions
4. **HTTP/2**: Multiplexing compensates for any overhead
5. **CDNs**: Handle TLS termination at edge

**Actual Performance Impact**: < 1% with modern infrastructure

## HTTP Versions

### HTTP/1.0 (1996)
- **One request per connection**
- Connection closes after each response
- Very inefficient

```
Request 1: Open connection → GET /page.html → Close
Request 2: Open connection → GET /style.css → Close
Request 3: Open connection → GET /script.js → Close

Problem: Opening TCP connections is expensive!
```

### HTTP/1.1 (1997) - Most Common Until Recently

**Improvements over 1.0**:

**1. Persistent Connections (Keep-Alive)**
```
Connection opened
GET /page.html → Response
GET /style.css → Response
GET /script.js → Response
Connection closed (after timeout or explicit close)

Benefit: Reuse TCP connection, avoid handshake overhead
```

**2. Pipelining** (rarely used due to issues)
```
Send multiple requests without waiting:
GET /1 → GET /2 → GET /3 → Response 1 → Response 2 → Response 3

Problem: Head-of-line blocking
If Response 1 is slow, it blocks 2 and 3
```

**3. Chunked Transfer Encoding**
```
Server can send response in chunks:
- Useful when total size unknown
- Streaming content
```

**4. Virtual Hosting**
```
Host: www.example1.com → Server 1
Host: www.example2.com → Server 2

One IP address, multiple websites
```

**HTTP/1.1 Limitations**:
- Head-of-line blocking
- No request prioritization
- Plain text headers (overhead)
- Only client can initiate requests

### HTTP/2 (2015) - Modern Standard

**Major Improvements**:

**1. Multiplexing**
```
Single TCP Connection:
Request 1 (image, high priority)
Request 2 (css, medium priority)
Request 3 (analytics, low priority)
    ↓ Interleaved
All responses can be sent simultaneously
No head-of-line blocking at HTTP level
```

**2. Binary Protocol**
```
HTTP/1.1: Text-based, human-readable
GET /index.html HTTP/1.1\r\n
Host: example.com\r\n

HTTP/2: Binary frames
[00101011 01110101...] (efficient, but not human-readable)
```

**3. Header Compression (HPACK)**
```
HTTP/1.1: Send same headers repeatedly
Request 1: User-Agent: Mozilla/5.0... (100 bytes)
Request 2: User-Agent: Mozilla/5.0... (100 bytes)
Request 3: User-Agent: Mozilla/5.0... (100 bytes)

HTTP/2: Compress and reuse
Request 1: User-Agent: Mozilla/5.0... (100 bytes, indexed)
Request 2: User-Agent: [ref to index] (1 byte)
Request 3: User-Agent: [ref to index] (1 byte)
```

**4. Server Push**
```
Client: GET /index.html
Server: Here's index.html
        Also, I'm pushing style.css and script.js
        (you'll need them anyway)

Client: Thanks! (saves 2 round trips)
```

**5. Stream Prioritization**
```
Client tells server:
- Images: Priority 10
- CSS: Priority 20 (higher)
- JavaScript: Priority 15

Server sends high-priority resources first
```

**HTTP/2 Performance Gains**:
- 30-50% faster page loads
- Better on high-latency connections
- Reduces need for domain sharding

### HTTP/3 (2022) - Latest Standard

**Key Change: Uses QUIC (UDP) Instead of TCP**

**Why Move Away from TCP?**

TCP's Head-of-Line Blocking Problem:
```
HTTP/2 over TCP:
Packet 1 (Stream A) ✓ Received
Packet 2 (Stream B) ✗ Lost
Packet 3 (Stream C) ✓ Received (but must wait for packet 2)
Packet 4 (Stream D) ✓ Received (but must wait for packet 2)

TCP blocks all streams until lost packet is retransmitted!
```

**HTTP/3 (QUIC) Solution**:
```
HTTP/3 over QUIC (UDP):
Stream A: ✓ Delivered immediately
Stream B: ✗ Lost, but only Stream B waits
Stream C: ✓ Delivered immediately
Stream D: ✓ Delivered immediately

Each stream independent!
```

**HTTP/3 Benefits**:
1. **0-RTT Connection**: Even faster than HTTP/2
2. **Better Loss Recovery**: Per-stream, not connection-wide
3. **Connection Migration**: Switch networks (WiFi to 4G) without reconnection
4. **Built-in Encryption**: QUIC requires TLS 1.3

**HTTP/3 Adoption**: Growing, supported by major CDNs (Cloudflare, Google, Facebook)

## Real-World DevOps Examples

### Example 1: API Gateway
```
User → HTTPS:443 → API Gateway → HTTP:8080 → Backend Services
```
External traffic uses HTTPS, internal can use HTTP.

### Example 2: Health Check Endpoint
```bash
# Kubernetes liveness probe
GET /health HTTP/1.1
Host: app-service:8080

Response: 200 OK
```

### Example 3: Load Balancer Setup
```
Client → HTTPS:443 → Load Balancer → HTTP:8080 → App Servers
                    (SSL Termination)
```

## Common Commands

### Make HTTP requests:
```bash
# Simple GET request
curl https://api.example.com/users

# POST with JSON data
curl -X POST https://api.example.com/users \
  -H "Content-Type: application/json" \
  -d '{"name":"John","email":"john@example.com"}'

# See full request/response (debugging)
curl -v https://api.example.com

# Follow redirects
curl -L https://example.com

# Check response time
curl -w "@-" -o /dev/null -s https://example.com <<EOF
    time_total:  %{time_total}s
EOF
```

### Test SSL certificate:
```bash
# Check certificate details
openssl s_client -connect example.com:443

# Check certificate expiry
echo | openssl s_client -connect example.com:443 2>/dev/null | openssl x509 -noout -dates
```

## RESTful API Conventions

REST uses HTTP methods semantically:

```
GET    /api/users           # List all users
GET    /api/users/123       # Get user 123
POST   /api/users           # Create new user
PUT    /api/users/123       # Update user 123 (full)
PATCH  /api/users/123       # Update user 123 (partial)
DELETE /api/users/123       # Delete user 123
```

## CORS (Cross-Origin Resource Sharing)

Allows web pages to request resources from different domains.

```
# Browser sends preflight request
OPTIONS /api/data HTTP/1.1
Origin: https://frontend.com

# Server responds
HTTP/1.1 200 OK
Access-Control-Allow-Origin: https://frontend.com
Access-Control-Allow-Methods: GET, POST, PUT
Access-Control-Allow-Headers: Content-Type, Authorization
```

**DevOps Context**: Configure CORS in your API gateway or backend.

## Caching

### Cache-Control Header
```
Cache-Control: public, max-age=3600       # Cache for 1 hour
Cache-Control: private, no-cache          # Don't cache
Cache-Control: no-store                   # Never store
```

### ETag (Entity Tag)
```
# First request
Response: ETag: "abc123"

# Next request
Request: If-None-Match: "abc123"
Response: 304 Not Modified (use cached version)
```

## WebSockets

Real-time, bidirectional communication over a single TCP connection.

```
Client → Server: HTTP Upgrade request
Server → Client: 101 Switching Protocols
[Connection upgraded to WebSocket]
Client ⟷ Server: Real-time messages
```

**Use cases**: Chat apps, live dashboards, gaming, stock tickers

## Common Issues & Debugging

### Issue: 502 Bad Gateway
```bash
# Check if backend is running
curl http://localhost:8080

# Check backend logs
docker logs backend-container
```

### Issue: SSL Certificate Error
```bash
# Bypass SSL verification (testing only!)
curl -k https://example.com

# Check certificate
openssl s_client -connect example.com:443
```

### Issue: CORS Error
Add headers to your API:
```
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: GET, POST, OPTIONS
Access-Control-Allow-Headers: Content-Type
```

## Key Takeaways

✅ HTTP is stateless - each request is independent  
✅ HTTPS encrypts traffic - always use it in production  
✅ Status codes tell you what happened (2xx=success, 4xx=client error, 5xx=server error)  
✅ REST APIs use HTTP methods semantically  
✅ Headers carry metadata about requests/responses  
✅ Understanding HTTP is crucial for debugging API issues  
✅ Use curl for testing and debugging HTTP endpoints
