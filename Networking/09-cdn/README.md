# CDN (Content Delivery Network)

## What is a CDN?

A CDN is a geographically distributed network of servers that delivers content to users from the nearest location.

### The Distance Problem

**The Fundamental Issue**: Speed of light is finite, and data can't travel faster than it.

**Physics of Network Latency**:
```
Light speed in fiber: ~200,000 km/second (2/3 speed in vacuum)

Distance: New York to Los Angeles = 4,000 km
Theoretical minimum latency: 4000 / 200000 = 0.02 seconds = 20ms

Real-world latency: 70-80ms (due to routing, switches, etc.)

Problem: Every round trip doubles this:
- DNS lookup: 80ms
- TCP handshake: 80ms  
- TLS handshake: 80ms
- HTTP request/response: 80ms
Total: 320ms before content even loads!
```

**CDN Solution**:
```
User in Los Angeles
      ↓ 5ms latency
CDN Server in Los Angeles (has cached content)
      ↓
User gets content in 5ms instead of 80ms
```

### How CDN Fundamentally Works

**Architecture**:
```
                Origin Server (Your Server)
                   |
                   | Content distributed
                   |
        ┌──────────┼──────────┐
        |          |          |
    Edge PoP   Edge PoP   Edge PoP
    (New York) (London)  (Tokyo)
        |          |          |
    Caching    Caching    Caching
        |          |          |
    Users      Users      Users
```

**PoP (Point of Presence)**:
- Physical location with CDN servers
- Connected to local ISPs
- Stores cached content
- Major CDNs have 100-300+ PoPs globally

### CDN Request Flow (Detailed)

**First Request (Cache Miss)**:
```
1. User in London types: www.example.com
2. DNS Resolution:
   - Browser queries DNS
   - DNS returns: cdn-london.example.com (nearest edge)
   
3. User connects to London Edge Server (cdn-london.example.com)
4. Edge Server checks: Do I have /image.jpg cached?
5. Cache Miss! Edge doesn't have it
6. Edge Server → Origin Server: "Give me /image.jpg"
7. Origin Server → Edge Server: Here's /image.jpg
8. Edge Server:
   - Stores /image.jpg in cache
   - Returns /image.jpg to user
   
Total Time: 100ms (includes origin fetch)
```

**Subsequent Requests (Cache Hit)**:
```
1. Another user in London requests: www.example.com/image.jpg
2. DNS returns: cdn-london.example.com
3. User connects to London Edge Server
4. Edge Server checks: Do I have /image.jpg cached?
5. Cache Hit! Edge has it
6. Edge Server → User: Here's /image.jpg (from cache)

Total Time: 10ms (no origin fetch needed)
Origin Server: Not contacted at all
```

### CDN Geographic Routing

**DNS-Based Routing** (Most Common):
```
User IP: 203.0.113.5 (detected as Los Angeles)
DNS Query: www.example.com

DNS responds with IP of nearest PoP:
- 192.0.2.10 (Los Angeles edge server)

Next user IP: 198.51.100.10 (detected as London)
DNS responds:
- 198.51.100.50 (London edge server)

Same domain, different IP based on user location
```

**Anycast Routing** (Advanced):
```
All edge servers advertise same IP: 203.0.113.10

User in Tokyo connects to 203.0.113.10
Internet routing: Delivers to Tokyo PoP (closest)

User in Paris connects to 203.0.113.10  
Internet routing: Delivers to Paris PoP (closest)

Same IP, routes to nearest server automatically
No DNS tricks needed
```

### CDN Caching Theory

**Cache Hierarchy**:
```
Level 1: Edge Cache (closest to users)
         - 100-500 servers globally
         - Small cache (1-10 TB per server)
         - Very fast SSD storage
         
Level 2: Regional Cache (mid-tier)
         - 10-50 servers per region
         - Large cache (100+ TB)
         - If edge misses, checks regional
         
Level 3: Origin Shield (last before origin)
         - Protects origin from thundering herd
         - Consolidates requests to origin
         
Level 4: Origin Server (your server)
         - Only hit on cache miss
         - Handles 1-10% of traffic (with good CDN)
```

**Cache Eviction Policies**:

CDNs must decide what to keep when cache is full.

**LRU (Least Recently Used)**:
```
Cache full? Remove least recently accessed item

Example:
Items in cache: A (accessed 1h ago), B (2h ago), C (30m ago)
New item arrives: Remove B (least recently used)

Most popular items stay in cache
```

**LFU (Least Frequently Used)**:
```
Cache full? Remove least frequently accessed item

Tracks access count per item:
A: 100 accesses
B: 10 accesses  ← Remove this
C: 50 accesses
```

**TTL-Based**:
```
Each cached item has expiration:
Item A: Cached at 10:00, TTL=3600s, expires 11:00
Item B: Cached at 10:30, TTL=300s, expires 10:35

At 10:35: B automatically removed (expired)
At 11:00: A automatically removed (expired)
```

**Adaptive Algorithm** (Modern CDNs):
```
Combines multiple factors:
- Popularity (access frequency)
- Recency (last access time)
- Size (cost to cache)
- TTL (time to expiration)
- Origin load (cost to fetch again)

Machine learning predicts what to keep
```

### CDN Cache Control

**Cache-Control Header** (Server tells CDN what to cache):

```
Cache-Control: public, max-age=31536000, immutable

public: Can be cached by CDN (not private to user)
max-age=31536000: Cache for 1 year (31,536,000 seconds)
immutable: Content never changes (don't revalidate)
```

**Common Patterns**:

```
Static Assets (CSS, JS, Images with hash):
Cache-Control: public, max-age=31536000, immutable
Reason: Content hash in filename, never changes

Images (without hash):
Cache-Control: public, max-age=86400
Reason: May update, revalidate daily

HTML:
Cache-Control: public, max-age=300, must-revalidate
Reason: Changes frequently, revalidate after 5 minutes

API Responses:
Cache-Control: private, no-cache
Reason: User-specific, don't cache at CDN

Sensitive Data:
Cache-Control: private, no-store
Reason: Never cache anywhere
```

**Vary Header** (Cache Different Versions):
```
Vary: Accept-Encoding

CDN stores separate cache entries for:
- gzip compressed version
- brotli compressed version
- uncompressed version

User with gzip support → gets gzip version
User without → gets uncompressed version
```

### Cache Hit Ratio

**Definition**: Percentage of requests served from cache (not origin)

```
Cache Hit Ratio = (Cache Hits / Total Requests) × 100

Example:
Total Requests: 1,000,000
Cache Hits: 950,000
Cache Misses: 50,000

Hit Ratio: 95% (excellent)
```

**Why It Matters**:
```
95% Hit Ratio:
- Origin handles 50,000 requests
- CDN handles 950,000 requests
- Origin load reduced by 95%

80% Hit Ratio:
- Origin handles 200,000 requests
- CDN handles 800,000 requests
- Origin load reduced by 80%

Difference: 4x more origin load at 80% vs 95%
```

**Improving Hit Ratio**:

1. **Increase TTL**:
```
Old: max-age=300 (5 minutes)
New: max-age=3600 (1 hour)
Result: Content cached longer, more hits
```

2. **Query String Consistency**:
```
Bad: /image.jpg?v=123&timestamp=1234567890
     /image.jpg?timestamp=0987654321&v=123
     (Different order = different cache entries)

Good: /image.jpg?v=123
      (Same parameters = same cache entry)
```

3. **Normalize Headers**:
```
Don't vary cache on unnecessary headers:
Vary: User-Agent (bad - thousands of user agents)
Vary: Accept-Encoding (good - only a few encodings)
```

4. **Prefetch Popular Content**:
```
Push popular content to edge before requests:
- New product launch
- Breaking news
- Popular video

Guarantees cache hit from first request
```

### CDN Performance Optimization

**TCP Connection Reuse**:
```
Without CDN:
User → Origin (3-way handshake, TLS handshake)
Repeated for every asset

With CDN:
User → Edge (handshake once)
Edge → Origin (persistent connection, reused)

Result: Fewer handshakes, faster loading
```

**HTTP/2 & HTTP/3**:
```
Modern CDNs support:
- HTTP/2: Multiplexing, header compression
- HTTP/3: QUIC protocol, 0-RTT

Even if origin only supports HTTP/1.1:
User → HTTP/2 → CDN → HTTP/1.1 → Origin

User benefits from modern protocol
```

**Brotli Compression**:
```
Gzip: 70% compression
Brotli: 20-25% better than gzip

File size: 100 KB
- Uncompressed: 100 KB
- Gzip: 30 KB
- Brotli: 22-24 KB

CDN compresses once, serves to many users
```

## Popular CDN Providers

| Provider | Features | Use Case |
|----------|----------|----------|
| **Cloudflare** | Free tier, DDoS protection | General purpose |
| **AWS CloudFront** | Integrated with AWS | AWS users |
| **Azure CDN** | Integrated with Azure | Azure users |
| **Akamai** | Enterprise-grade | Large enterprises |
| **Fastly** | Real-time purging | High-control needs |
| **Google Cloud CDN** | Integrated with GCP | GCP users |

## What Content Should Use CDN?

### ✅ Great for CDN:
- Static files (images, CSS, JavaScript)
- Videos and media
- Software downloads
- Public API responses
- Fonts and libraries

### ❌ Not for CDN:
- User-specific data (personalized content)
- Frequently changing data
- Private/authenticated content (unless configured carefully)

## CDN Caching

### Cache-Control Headers

```
# Cache for 1 year (static assets)
Cache-Control: public, max-age=31536000, immutable

# Cache for 1 hour
Cache-Control: public, max-age=3600

# Don't cache
Cache-Control: no-cache, no-store, must-revalidate

# Cache but revalidate
Cache-Control: public, max-age=0, must-revalidate
```

### Cache Hierarchy

```
1. Browser Cache (user's computer)
        ↓
2. CDN Edge Cache (nearest CDN server)
        ↓
3. CDN Regional Cache (regional CDN server)
        ↓
4. Origin Server (your server)
```

## Real-World Examples

### Example 1: Static Website
```html
<!-- Before CDN -->
<img src="https://example.com/images/logo.png">
<link rel="stylesheet" href="https://example.com/css/style.css">
<script src="https://example.com/js/app.js"></script>

<!-- After CDN -->
<img src="https://cdn.example.com/images/logo.png">
<link rel="stylesheet" href="https://cdn.example.com/css/style.css">
<script src="https://cdn.example.com/js/app.js"></script>
```

### Example 2: Video Streaming
```
User plays video
     ↓
CDN serves video chunks from nearest edge
     ↓
Smooth playback, no buffering (if properly cached)
```

### Example 3: E-commerce Site
```
Product images → CDN (cached for days)
Product prices → Origin (dynamic, not cached)
CSS/JS files → CDN (cached forever with versioning)
```

### Example 4: Software Distribution
```
User downloads app installer
     ↓
CDN serves from nearest location
     ↓
Fast download, reduced origin bandwidth
```

## CDN Configuration Examples

### Cloudflare Setup
```
1. Add your domain to Cloudflare
2. Update nameservers to Cloudflare's
3. Enable "Proxy" (orange cloud) for DNS records
4. Configure cache rules:
   - Cache everything on /static/*
   - Bypass cache for /api/*
```

### AWS CloudFront
```
1. Create CloudFront distribution
2. Set origin: your S3 bucket or web server
3. Configure behaviors:
   - /images/* → Cache for 1 day
   - /videos/* → Cache for 7 days
   - /*.html → Cache for 1 hour
4. Set custom domain (CNAME)
5. Add SSL certificate
```

### Nginx as Simple CDN Cache
```nginx
proxy_cache_path /var/cache/nginx levels=1:2 keys_zone=cdn:100m max_size=10g;

server {
    listen 80;
    server_name cdn.example.com;

    location / {
        proxy_cache cdn;
        proxy_cache_valid 200 7d;
        proxy_cache_valid 404 1m;
        
        proxy_pass http://origin-server;
        proxy_set_header Host $host;
        
        # Add cache status header
        add_header X-Cache-Status $upstream_cache_status;
    }
}
```

## Cache Invalidation (Purging)

When you update content, you need to remove old cached versions.

### Methods:

**1. Cache Purging/Invalidation**
```bash
# Cloudflare API
curl -X POST "https://api.cloudflare.com/client/v4/zones/{zone_id}/purge_cache" \
     -H "Authorization: Bearer {token}" \
     -H "Content-Type: application/json" \
     --data '{"files":["https://example.com/style.css"]}'

# AWS CloudFront CLI
aws cloudfront create-invalidation \
    --distribution-id DISTRIBUTION_ID \
    --paths "/images/*" "/css/style.css"
```

**2. Versioning (Recommended)**
```html
<!-- Old version -->
<link rel="stylesheet" href="/css/style.css?v=1">

<!-- New version -->
<link rel="stylesheet" href="/css/style.css?v=2">

<!-- Better: Hash-based -->
<link rel="stylesheet" href="/css/style.a7f3c5.css">
```

**3. Cache-Control Headers**
```
# Short TTL for frequently changing content
Cache-Control: max-age=300  # 5 minutes

# Long TTL for static assets
Cache-Control: max-age=31536000, immutable
```

## CDN Performance Optimization

### 1. Enable Compression
```
# CDN automatically compresses (gzip/brotli)
# Reduces file sizes by 70-90%
```

### 2. Image Optimization
```
Cloudflare: Auto-optimize images
CloudFront: Use Lambda@Edge for on-the-fly optimization
```

### 3. HTTP/2 & HTTP/3
```
Modern CDNs enable HTTP/2 by default
- Multiplexing
- Header compression
- Faster page loads
```

### 4. Minification
```
# CDN can auto-minify CSS/JS
Original: 100KB
Minified: 25KB
```

## CDN Security Features

### 1. DDoS Protection
```
Malicious traffic → CDN (absorbs and filters) → Origin (protected)
```

### 2. Web Application Firewall (WAF)
```
CDN can block:
- SQL injection attempts
- XSS attacks
- Known malicious IPs
```

### 3. SSL/TLS
```
CDN provides free SSL certificates
- User → HTTPS → CDN → HTTPS → Origin
- Or: User → HTTPS → CDN → HTTP → Origin (SSL termination)
```

### 4. Access Control
```
# Restrict by geography
Block all traffic from certain countries

# Restrict by IP
Allow only specific IP ranges

# Signed URLs (time-limited access)
https://cdn.example.com/video.mp4?token=abc&expires=1234567890
```

## Monitoring CDN Performance

### Key Metrics:

**Cache Hit Ratio**
```
Cache Hits / Total Requests × 100

Example: 95% cache hit ratio
- 95 requests served from cache
- 5 requests went to origin
```

**Bandwidth Savings**
```
CDN bandwidth used vs Origin bandwidth saved

Example: 
- 10TB delivered by CDN
- 500GB from origin
- Savings: 95%
```

**Response Time**
```
Monitor: Time to first byte (TTFB)
Goal: < 100ms for cached content
```

## Common Commands

### Check if CDN is working:
```bash
# Check which server responded
curl -I https://cdn.example.com/image.jpg
# Look for: X-Cache: HIT (from CDN) or MISS (from origin)

# Check with different locations (using VPN/proxy)
curl -I --resolve cdn.example.com:443:1.1.1.1 https://cdn.example.com/
```

### Test CDN performance:
```bash
# Measure download time
time curl -o /dev/null https://cdn.example.com/large-file.zip

# Check response headers
curl -I https://cdn.example.com/image.jpg | grep -i cache
```

## Troubleshooting

### Issue: Changes Not Reflected
**Cause**: Content is cached.

**Solution**:
```bash
# Option 1: Purge cache at CDN
# Option 2: Use versioned URLs
# Option 3: Wait for cache to expire
```

### Issue: Slow First Load (Cache Miss)
**Normal behavior**: First request always goes to origin.

**Solution**: Pre-warm cache for popular content.

### Issue: Different Content in Different Regions
**Cause**: Caches not synced yet.

**Solution**: Wait for TTL to expire globally, or invalidate cache.

## CDN Best Practices

### 1. Use Long Cache Times for Static Assets
```
CSS/JS with hash: Cache for 1 year
Images: Cache for 1 month
HTML: Cache for a few minutes
```

### 2. Version Your Static Assets
```
style.v1.css → style.v2.css (force new fetch)
# Or use build tools to generate hashed filenames
```

### 3. Optimize Images Before Uploading
```
# Don't rely solely on CDN optimization
# Pre-optimize: WebP format, compressed
```

### 4. Monitor Cache Hit Ratio
```
Goal: > 90% cache hit ratio
If lower: Check cache headers and TTLs
```

### 5. Use Separate Domain for CDN
```
# Better
https://cdn.example.com/images/logo.png

# Avoid
https://example.com/images/logo.png
```
Reason: Cookieless domain, better performance.

## Cost Considerations

### CDN Pricing (Typical)
```
- Data transfer: $0.02 - $0.15 per GB
- Requests: $0.005 - $0.01 per 10,000 requests
- Free tiers available (Cloudflare, AWS free tier)
```

### Cost Optimization:
1. Use CDN for static content only
2. Higher cache hit ratio = lower costs
3. Compress files before uploading
4. Free tier options: Cloudflare, BunnyCDN free tier

## Key Takeaways

✅ CDNs serve content from geographically distributed servers  
✅ Reduces latency by serving from nearest location  
✅ Great for static content (images, CSS, JS, videos)  
✅ Cache invalidation is important when content changes  
✅ Versioned URLs are better than cache purging  
✅ CDNs provide DDoS protection and improved security  
✅ Monitor cache hit ratio (aim for >90%)  
✅ Use separate domain for CDN content (cookieless)  
✅ Most modern websites use CDNs for performance
