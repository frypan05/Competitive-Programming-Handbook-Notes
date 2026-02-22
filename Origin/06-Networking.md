# 06 — Networking

---

## SECTION 1 — OSI Model

| Layer | Name | Protocols / Examples |
|-------|------|----------------------|
| 7 | Application | HTTP, HTTPS, DNS, FTP, SSH, SMTP, gRPC |
| 6 | Presentation | TLS/SSL, JPEG, MPEG, encoding |
| 5 | Session | NetBIOS, RPC, session management |
| 4 | Transport | TCP, UDP |
| 3 | Network | IP, ICMP, OSPF, BGP |
| 2 | Data Link | Ethernet, ARP, MAC addresses, switches |
| 1 | Physical | Cables, fiber, NICs, hubs |

**Debugging approach:** Start at layer 1 (is the interface up?), work up to layer 7.
Tools by layer: `ping` (L3), `traceroute` (L3), `tcpdump` (L2-L7), `curl` (L7), `ss`/`netstat` (L4).

---

## SECTION 2 — TCP

**Q: Explain the TCP three-way handshake.**

```
1. SYN     — Client sends SYN (seq=X) to server. "I want to connect."
2. SYN-ACK — Server responds with SYN (seq=Y) + ACK (ack=X+1). "OK, I'm ready."
3. ACK     — Client sends ACK (ack=Y+1). Connection established.
```

---

**Q: Explain TCP connection teardown (four-way).**

```
1. Active closer sends FIN.                  "I'm done sending."
2. Passive closer sends ACK.                 "Got it."
3. Passive closer sends FIN.                 "I'm done too."
4. Active closer sends ACK.
   -> Active closer enters TIME_WAIT (2 x MSL ~= 60-120s).

TIME_WAIT: prevents old duplicate packets from corrupting new connections on the same port.
High TIME_WAIT count = high connection churn. Fix: connection pooling or tune tcp_tw_reuse.
```

---

**Q: TCP vs UDP — when to use each?**

```
TCP:
  - Reliable, ordered, error-checked delivery via ACKs and retransmission.
  - Connection-oriented (three-way handshake before data).
  - Congestion control: backs off when the network is congested.
  - Use for: HTTP/HTTPS, databases, SSH, file transfer — anything requiring reliability.
  - Overhead: connection setup, ACKs, retransmissions.

UDP:
  - Unreliable, unordered, no retransmission. Fire and forget.
  - Connectionless — no handshake overhead.
  - Use for: DNS, VoIP, video streaming, gaming, NTP.
  - Why UDP: lower latency, lower overhead; application can handle errors itself.
  - QUIC (HTTP/3) is UDP-based with reliability implemented at the application layer.
```

---

**Q: What is TCP backlog and why does it matter in production?**

A: The kernel maintains two queues for incoming TCP connections:
- **SYN queue**: incomplete connections (SYN received, waiting for final ACK from client).
- **Accept queue**: completed three-way handshakes waiting for the application to call `accept()`.

If the accept queue fills up, new connections are silently dropped (or RST sent).
This causes connection refused errors under high load.

```bash
# Tune these kernel parameters
sysctl net.core.somaxconn              # max size of accept queue
sysctl net.ipv4.tcp_max_syn_backlog    # max size of SYN queue

# Set permanently in /etc/sysctl.conf
net.core.somaxconn = 65535
net.ipv4.tcp_max_syn_backlog = 65535
```

---

**Q: What is a SYN flood attack?**

A: An attacker sends many SYN packets with spoofed source IPs. The server sends SYN-ACK to
fake IPs that never respond, filling the SYN queue until legitimate connections are dropped.
Mitigated by: **SYN cookies** (`net.ipv4.tcp_syncookies=1`) — the server encodes state in the
sequence number so the SYN queue is not needed until the handshake completes.

---

## SECTION 3 — DNS

**Q: Explain full DNS resolution for `api.example.com` from scratch.**

```
1.  Check local DNS cache (browser cache, OS resolver cache).
2.  Check /etc/hosts file.
3.  Ask the configured DNS resolver (e.g., 8.8.8.8, or VPC DNS at VPC_CIDR+2).
4.  Resolver checks its own cache. If hit, return immediately.
5.  Resolver asks a root nameserver (13 root server clusters globally):
    "Who handles .com?"
6.  Root server responds: "Ask a.gtld-servers.net" (TLD nameserver for .com).
7.  Resolver asks the .com TLD nameserver:
    "Who is authoritative for example.com?"
8.  TLD nameserver responds: "ns1.example.com is authoritative."
9.  Resolver asks ns1.example.com:
    "What is the A record for api.example.com?"
10. ns1.example.com responds: "1.2.3.4, TTL=300"
11. Resolver caches the result for 300 seconds, returns 1.2.3.4 to the client.
```

---

**Q: Common DNS record types — know all of these.**

```
A      — maps hostname to IPv4 address.
         api.example.com -> 1.2.3.4

AAAA   — maps hostname to IPv6 address.
         api.example.com -> 2001:db8::1

CNAME  — maps hostname to another hostname (alias).
         www.example.com -> example.com
         Cannot be used at the zone apex (root domain). Use ALIAS or ANAME instead.

MX     — mail exchange server with priority. Lower number = higher priority.
         example.com -> 10 mail.example.com

TXT    — arbitrary text data.
         Used for: SPF (email), DKIM (email signing), domain ownership verification.

NS     — nameservers authoritative for the zone.
         example.com -> ns1.example.com, ns2.example.com

PTR    — reverse DNS: maps IP address to hostname.
         1.2.3.4 -> api.example.com
         Used by: email servers, logging, security tools.

SOA    — Start of Authority. Zone metadata: primary nameserver, admin email,
         serial number, refresh/retry/expire/minimum TTL.

SRV    — service location record: hostname + port + priority + weight.
         Used by: Kubernetes (headless services), SIP, XMPP, LDAP.
         _http._tcp.example.com -> 10 5 8080 api.example.com
```

---

**Q: What is TTL in DNS and how does it affect deployments?**

A: TTL (Time To Live) tells DNS resolvers how long to cache the record (in seconds).

```
Low TTL (60-300s):
  + Changes propagate quickly.
  - More DNS queries hit your authoritative nameserver.
  - Higher latency for first-time lookups (no cache hit).

High TTL (3600-86400s):
  + Less load on authoritative nameserver.
  + Faster resolution (more likely to be cached).
  - DNS changes take a long time to propagate globally.
```

**Best practice for a planned DNS change (e.g., migrating an IP):**
1. Days before: lower TTL to 60 seconds (wait for old TTL to expire first).
2. Make the DNS change.
3. Wait 60 seconds (old TTL) — all cached entries have now expired.
4. Raise TTL back to normal (e.g., 3600).

---

**Q: What is a split-horizon DNS?**

A: Different DNS answers are returned for the same hostname depending on where the query
originates. Example: internal queries for `api.company.com` return a private IP (10.0.0.5),
external queries return the public IP or load balancer DNS name. Used in AWS VPCs with Route53
private hosted zones.

---

## SECTION 4 — HTTP / HTTPS

**Q: HTTP status codes — know every category and the key codes.**

```
1xx Informational:
  100 Continue           — client should continue sending the request body
  101 Switching Protocols — upgrading to WebSocket

2xx Success:
  200 OK                 — standard success
  201 Created            — resource created (POST/PUT)
  204 No Content         — success, no body (DELETE)
  206 Partial Content    — range request fulfilled

3xx Redirection:
  301 Moved Permanently  — resource has a new URL permanently (client should update bookmarks)
  302 Found              — temporary redirect
  304 Not Modified       — resource unchanged; use cached version (ETag/Last-Modified matched)

4xx Client Error:
  400 Bad Request        — malformed request syntax, invalid parameters
  401 Unauthorized       — NOT authenticated (must login first)
  403 Forbidden          — authenticated but NOT authorised for this resource
  404 Not Found          — resource does not exist
  405 Method Not Allowed — HTTP method not supported for this endpoint
  408 Request Timeout    — client took too long
  409 Conflict           — conflict with current state (e.g., duplicate resource)
  422 Unprocessable      — validation errors (common in REST APIs)
  429 Too Many Requests  — rate limit exceeded

5xx Server Error:
  500 Internal Server Error — unhandled exception in application code
  502 Bad Gateway           — upstream server returned an invalid response
  503 Service Unavailable   — server is down or overloaded (show maintenance page)
  504 Gateway Timeout       — upstream server did not respond within timeout
```

**Difference between 401 and 403:**
- 401: You are not logged in. Provide credentials.
- 403: You are logged in, but you don't have permission. Credentials won't help.

**Difference between 502 and 504:**
- 502: The upstream responded but with garbage/invalid data.
- 504: The upstream did not respond at all within the timeout.

---

**Q: What is the TLS handshake? (Simplified for interviews)**

```
1. ClientHello  — client sends: TLS version, list of supported cipher suites, random bytes.
2. ServerHello  — server sends: chosen cipher suite, random bytes, server certificate.
3. Certificate  — client verifies the certificate:
                  - Is it signed by a trusted CA in my trust store?
                  - Is the hostname in the cert's CN or SAN?
                  - Is it within the validity period?
                  - Has it been revoked (OCSP/CRL)?
4. Key Exchange — ECDHE: both sides independently compute the same session key using
                  elliptic-curve Diffie-Hellman. The private key is never transmitted.
5. Finished     — both sides send a "Finished" message encrypted with the session key.
                  This verifies the handshake was not tampered with.
6. Encrypted communication begins using symmetric encryption (AES-GCM).
```

Why ECDHE? It provides **Perfect Forward Secrecy** — even if the server's private key is
compromised later, past sessions cannot be decrypted because session keys are ephemeral.

---

**Q: HTTP/1.1 vs HTTP/2 vs HTTP/3?**

```
HTTP/1.1:
  - One active request per TCP connection (head-of-line blocking).
  - Browsers open 6 parallel TCP connections to work around this.
  - Text-based headers: verbose, uncompressed, repeated on every request.
  - Pipelining existed but was poorly supported.

HTTP/2:
  - Multiplexing: multiple requests and responses simultaneously on ONE TCP connection.
  - Header compression: HPACK algorithm — headers sent as binary diffs.
  - Server push: server can proactively send resources before client requests them.
  - Binary framing layer: more efficient than text parsing.
  - Still TCP under the hood — TCP-level packet loss causes head-of-line blocking for ALL streams.

HTTP/3:
  - Uses QUIC, which is built on UDP.
  - Packet loss only affects the one stream with the lost packet, not all streams.
  - Built-in TLS 1.3 — 0-RTT connection resumption (no separate TLS handshake).
  - Connection migration: maintains connection when changing networks (e.g., WiFi to 4G).
  - Deployed by: Cloudflare, Google, Facebook, Akamai.
```

---

## SECTION 5 — Load Balancers

**Q: Layer 4 vs Layer 7 load balancer — explain and give AWS examples.**

```
Layer 4 (Transport Layer / TCP/UDP):
  - Operates on IP addresses and ports only. Does NOT inspect the payload.
  - Cannot make routing decisions based on URL paths, hostnames, or headers.
  - Very fast — minimal processing overhead.
  - Supports TLS passthrough (encrypted traffic not terminated at the LB).
  - AWS: Network Load Balancer (NLB).
         Millions of requests per second. Static IP per AZ. Ultra-low latency.
  - Use for: TCP/UDP workloads, SMTP, FTP, gaming, IoT, extreme throughput requirements.

Layer 7 (Application Layer / HTTP/HTTPS):
  - Inspects HTTP headers, URL paths, hostnames, cookies, request body.
  - Can route requests based on:
      - Path: /api/* -> backend service, /static/* -> CDN origin
      - Host header: api.example.com vs admin.example.com
      - HTTP headers: X-User-Role, Authorization
      - Query parameters
  - Performs SSL/TLS termination (decrypts traffic, inspects it, re-encrypts or sends plain).
  - Integrates with: WAF (Web Application Firewall), sticky sessions, health checks.
  - AWS: Application Load Balancer (ALB).
         Path-based routing, host-based routing, WebSocket, HTTP/2, Lambda targets.
  - Use for: web applications, REST APIs, microservices, WebSocket.
```

---

**Q: Load balancing algorithms.**

```
Round Robin          — requests distributed sequentially to each backend in order.
                       Simple. Good when all backends have equal capacity and request cost.

Least Connections    — send the next request to the backend with the fewest active connections.
                       Best for: variable request durations (some requests take longer).

IP Hash              — hash the client's IP to always route to the same backend.
                       Provides session stickiness without cookies.
                       Problem: uneven distribution if few clients generate most traffic.

Weighted Round Robin — assign weights to backends; higher weight = more traffic.
                       Use when backends have different capacities (different instance types).

Least Response Time  — send to the backend with the fastest response time + fewest connections.
                       Used by: HAProxy, Nginx upstream with least_time directive.

Random               — pick a random backend. Surprisingly effective at scale.
```

---

**Q: What is connection draining / deregistration delay?**

A: When a backend is removed from the load balancer (deployment, scale-in, health failure),
existing in-flight requests are allowed to complete for a configurable period (AWS default: 300s).
During draining, no new requests are sent to the backend, but active requests are served out.
After the drain period, the backend is forcibly disconnected.

This is what makes zero-downtime deployments possible with load balancers.

---

**Q: What is a health check in a load balancer?**

A: The load balancer periodically sends probe requests to backends to verify they are alive and
capable of serving traffic. Unhealthy backends are removed from the rotation.

```
Types of health checks:
  TCP       — just attempts a TCP connection. Verifies the port is open.
  HTTP/HTTPS — sends a GET request to a path, checks for an expected status code.
  Custom    — check response body, not just status code.

Key parameters:
  HealthCheckPath           — /health or /healthz (must be lightweight, no DB calls)
  HealthCheckInterval       — how often to check (e.g., 30s)
  HealthyThresholdCount     — consecutive successes to mark healthy (e.g., 2)
  UnhealthyThresholdCount   — consecutive failures to mark unhealthy (e.g., 3)
  HealthCheckTimeout        — max response time (e.g., 5s)
```

---

## SECTION 6 — iptables & Linux Firewalling

**Q: What is iptables?**

A: Linux kernel's built-in packet filtering and NAT framework. Rules are organised into
tables, chains, and individual rules. Evaluated in order; first matching rule wins.

```
Tables:
  filter  — default table. Controls ACCEPT or DROP of packets.
  nat     — Network Address Translation. DNAT, SNAT, MASQUERADE.
  mangle  — modify packet headers (TTL, TOS, marks).
  raw     — exemptions from connection tracking.

Chains (built-in):
  filter:  INPUT (destined for localhost), OUTPUT (from localhost), FORWARD (through host)
  nat:     PREROUTING (before routing decision), POSTROUTING (after routing decision)

Targets (actions):
  ACCEPT      — allow the packet
  DROP        — silently discard (sender gets no response, times out)
  REJECT      — discard + send ICMP unreachable back (sender fails fast)
  LOG         — log matching packets to kernel log
  DNAT        — change destination IP/port (used for port forwarding, load balancing)
  SNAT        — change source IP (used for outbound NAT)
  MASQUERADE  — SNAT where the source IP is determined dynamically (for dynamic IPs)
```

Docker uses iptables DNAT rules for container port publishing (`-p 8080:80`).
Kubernetes kube-proxy programs iptables/ipvs rules for Service ClusterIP routing.

---

**Q: DROP vs REJECT — when to use each?**

```
DROP   — packet is silently discarded. The sender receives NO response.
         The sender waits until the connection times out (can take 30+ seconds).
         Use for: public-facing firewalls. Don't reveal the server's existence.
                  Slows down port scanners.

REJECT — packet is discarded AND an ICMP "port unreachable" or TCP RST is sent back.
         The sender fails immediately (no waiting for timeout).
         Use for: internal network rules where fast failure is desirable for debugging.
                  Avoids confusing "hanging" connections in development.
```

---

## SECTION 7 — CIDR Notation

**Q: What is CIDR notation? Calculate subnets.**

```
Format: IP_address/prefix_length
Example: 10.0.0.0/16

The prefix length specifies how many bits are fixed (the network part).
The remaining bits are the host addresses.

/8   = 2^24 = 16,777,216 addresses  (e.g., 10.0.0.0/8)
/16  = 2^16 =     65,536 addresses  (e.g., 10.0.0.0/16)
/24  = 2^8  =        256 addresses  (e.g., 10.0.0.0/24, hosts: .1 to .254)
/25  = 2^7  =        128 addresses  (e.g., 10.0.0.0/25, hosts: .1 to .126)
/28  = 2^4  =         16 addresses  (good for small subnets)
/30  = 2^2  =          4 addresses  (point-to-point links: 2 hosts)
/32  =               1 address      (a single specific IP)

Note: subtract 2 from usable hosts (network address + broadcast address).
So /24 has 256 - 2 = 254 usable host addresses.
```

**Subnetting a VPC example:**

```
VPC CIDR: 10.0.0.0/16 (65,536 addresses)

Split into /24 subnets per AZ:
  Public Subnet AZ-a:   10.0.1.0/24  (254 hosts)
  Public Subnet AZ-b:   10.0.2.0/24  (254 hosts)
  Private Subnet AZ-a:  10.0.10.0/24 (254 hosts)
  Private Subnet AZ-b:  10.0.11.0/24 (254 hosts)
  Database Subnet AZ-a: 10.0.20.0/24 (254 hosts)
  Database Subnet AZ-b: 10.0.21.0/24 (254 hosts)
```

---

## SECTION 8 — SSH

**Q: How does SSH key authentication work?**

```
1. The server has the client's public key in ~/.ssh/authorized_keys.
2. The client initiates an SSH connection.
3. The server generates a random challenge and encrypts it with the client's public key.
   Only the matching private key can decrypt this challenge.
4. The client decrypts the challenge using its private key and sends a proof back.
5. The server verifies the proof. Connection is established.
6. The private key is NEVER transmitted over the network.
```

---

**Q: How do you harden SSH configuration?**

```bash
# /etc/ssh/sshd_config — recommended settings

PasswordAuthentication no         # disable password login, key auth only
PermitRootLogin no                # never allow direct root login
AllowUsers ubuntu deploy          # whitelist specific users only
MaxAuthTries 3                    # limit brute-force attempts
LoginGraceTime 30                 # disconnect unauthenticated connections after 30s
ClientAliveInterval 300           # send keepalive every 5 min
ClientAliveCountMax 2             # disconnect after 2 missed keepalives (10 min idle)
X11Forwarding no                  # disable X11 forwarding (not needed on servers)
AllowTcpForwarding no             # disable tunneling unless needed
Port 2222                         # change from default 22 (reduces automated scan noise)
Protocol 2                        # SSH protocol v2 only
PubkeyAuthentication yes

# After changes, reload:
systemctl reload sshd
```

---

## SECTION 9 — Networking Debugging Scenarios

**Q: A pod in Kubernetes cannot reach an external API. How do you debug?**

```bash
# Step 1: Test connectivity from inside the pod
kubectl exec -it <pod-name> -- curl -v https://external-api.com

# Step 2: Check DNS resolution
kubectl exec -it <pod-name> -- nslookup external-api.com
kubectl exec -it <pod-name> -- cat /etc/resolv.conf

# Step 3: Check NetworkPolicy — is egress allowed?
kubectl get networkpolicy -n <namespace>
# A deny-all egress policy will block this

# Step 4: Check if the NODE can reach the API
# (SSH to node and curl — isolates whether it's pod-level or node-level)

# Step 5: Check cloud-level firewall
# AWS: check Security Group outbound rules, NACL, route table
# Does the private subnet have a route to a NAT Gateway?

# Step 6: Run a debug container with full networking tools
kubectl run netdebug \
  --image=nicolaka/netshoot \
  -it --rm --restart=Never \
  -n <namespace>
```

---

**Q: A service returns 502 Bad Gateway. What do you check?**

```
1. Is the upstream service running?
   kubectl get pods -n <namespace>

2. Are the pods passing health checks?
   kubectl describe pod <pod>   # check readiness probe failures

3. Is the Service selecting the right pods?
   kubectl get endpoints <service-name>   # should list pod IPs
   # If empty: label selector on Service doesn't match pod labels

4. Is the upstream listening on the correct port?
   kubectl exec -it <pod> -- ss -tlnp

5. Are there resource issues (OOM, CPU throttling)?
   kubectl top pods

6. Check application logs:
   kubectl logs <pod> --tail=100
```

---

**Q: Full request flow: Browser to containerised app on AWS with Kubernetes.**

```
Browser
  |-- DNS lookup: api.example.com
  |   -> Route53 resolves to ALB DNS name -> ALB's IP addresses

ALB (Application Load Balancer)
  |-- SSL termination (TLS handshake with browser)
  |-- Checks routing rules (path, host header)
  |-- Forwards to a registered target (K8s node IP : NodePort)

Kubernetes Node
  |-- iptables rule (managed by kube-proxy)
  |   DNAT: <NodeIP>:<NodePort> -> <PodIP>:<ContainerPort>

Pod
  |-- Container receives the HTTP request
  |-- Application processes and returns response

Response follows reverse path back to browser.
```

---

## SECTION 10 — Quick Reference Networking Commands

```bash
# Interface and IP
ip addr show                          # all interface addresses
ip link show                          # interface state (UP/DOWN)
ip route show                         # routing table
ip route get 8.8.8.8                  # which route is used to reach an IP

# Connectivity
ping -c 4 google.com                  # basic reachability (ICMP)
traceroute google.com                 # hop-by-hop path
mtr google.com                        # continuous traceroute (best for packet loss analysis)
curl -v https://example.com           # full HTTP request with headers
curl -o /dev/null -s -w "%{http_code} %{time_total}s\n" http://localhost:8080/health

# Ports and Sockets
ss -tlnp                              # listening TCP sockets with process
ss -tunap                             # all TCP+UDP with process
netstat -tlnp                         # older equivalent of ss
lsof -i :8080                         # what's using port 8080
fuser 8080/tcp                        # PID using port 8080

# DNS
nslookup api.example.com              # basic DNS lookup
dig api.example.com                   # detailed DNS lookup
dig api.example.com +trace            # full recursive resolution trace
dig -x 1.2.3.4                        # reverse DNS lookup
dig @8.8.8.8 api.example.com          # query specific DNS server

# Packet capture
tcpdump -i eth0 -nn port 8080         # capture traffic on port 8080
tcpdump -i eth0 -nn host 10.0.1.5     # capture traffic to/from specific IP
tcpdump -i any -w /tmp/capture.pcap   # save to file for Wireshark analysis

# HTTP testing
curl -I https://example.com           # headers only
curl -X POST -H "Content-Type: application/json" \
     -d '{"key":"value"}' https://api.example.com/endpoint
wget -q -O- http://localhost:8080/health
```
