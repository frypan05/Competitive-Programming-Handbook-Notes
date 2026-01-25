# TCP/IP & OSI Model

## What is TCP/IP?

TCP/IP (Transmission Control Protocol/Internet Protocol) is the fundamental communication protocol suite that powers the internet and most modern networks. It's not a single protocol but a collection of protocols working together at different layers.

### Why TCP/IP Exists

**The Problem**: Different computer systems and networks couldn't communicate because they used incompatible protocols.

**The Solution**: TCP/IP provides a standardized set of rules that all devices follow, enabling:
- Reliable data transmission across unreliable networks
- Interoperability between different systems and vendors
- End-to-end connectivity across multiple networks
- Error detection and recovery

### TCP/IP Protocol Suite

TCP/IP is actually a family of protocols:
- **Application Layer**: HTTP, FTP, SMTP, DNS, SSH
- **Transport Layer**: TCP, UDP
- **Internet Layer**: IP, ICMP, ARP
- **Link Layer**: Ethernet, WiFi

## The Two Main Protocols

### 1. TCP (Transmission Control Protocol)

TCP is a **connection-oriented** protocol that provides reliable, ordered delivery of data.

#### How TCP Ensures Reliability:

**Acknowledgments**: Every packet received is acknowledged
```
Sender: Sends packet #1
Receiver: Receives packet #1, sends ACK #1
Sender: Receives ACK #1, now sends packet #2
```

**Sequence Numbers**: Packets are numbered so they can be reassembled in order
```
Packet 1 (Seq: 1-100)
Packet 2 (Seq: 101-200)
Packet 3 (Seq: 201-300)

If packets arrive out of order (3, 1, 2), receiver reorders them.
```

**Retransmission**: If ACK not received within timeout, packet is resent
```
Sender: Sends packet #1
[Packet lost in network]
Sender: Timeout! Resends packet #1
Receiver: Receives packet #1, sends ACK #1
```

**Flow Control**: Prevents sender from overwhelming receiver
- Receiver advertises its **window size** (buffer space available)
- Sender limits transmission to window size
- Window size adjusts dynamically

**Congestion Control**: Prevents network overload
- **Slow Start**: Start with small window, gradually increase
- **Congestion Avoidance**: Carefully increase to find optimal speed
- **Fast Retransmit**: Quickly detect and recover from packet loss

#### TCP Characteristics:
- **Reliable**: Guarantees data delivery
- **Ordered**: Data arrives in the correct order
- **Connection-based**: Establishes connection before sending data
- **Error checking**: Detects corrupted data
- **Overhead**: More processing required
- **Use cases**: Web browsing (HTTP), email (SMTP), file transfers (FTP), SSH

**Real-world analogy**: Like a phone call - you establish a connection, verify the other person heard you, and maintain the conversation until you hang up.

### 2. UDP (User Datagram Protocol)

UDP is a **connectionless** protocol that provides fast but unreliable delivery.

#### How UDP Works:

**Fire and Forget**: Send data without establishing connection
```
Sender: Sends packet
[No waiting for ACK]
Sender: Sends next packet immediately
```

**No Ordering**: Packets may arrive out of order
```
Send: Packet 1, 2, 3
Receive: Packet 2, 3, 1 (application must handle this)
```

**No Retransmission**: Lost packets are not resent
```
Sender: Sends 100 packets
Receiver: Gets 98 packets (2 lost)
[Application deals with loss, not UDP]
```

#### UDP Characteristics:
- **Fast**: No connection setup overhead
- **Low latency**: No waiting for ACKs
- **Unreliable**: No guarantee of delivery
- **No ordering**: Packets may arrive out of order
- **No congestion control**: Sends at maximum rate
- **Lightweight**: Minimal protocol overhead
- **Use cases**: Video streaming, online gaming, DNS queries, VoIP

**Real-world analogy**: Like sending postcards - you send them and hope they arrive, but don't wait for confirmation.

## TCP Three-Way Handshake

Before data transfer, TCP establishes a connection using a three-way handshake. This ensures both sides are ready to communicate.

### The Handshake Process (Detailed):

```
Client                                    Server
  |                                         |
  |---------- SYN (Seq=100) -------------->|  
  | "I want to connect. My sequence        |
  | number starts at 100"                  |
  |                                         |
  |<----- SYN-ACK (Seq=300, Ack=101) ------|  
  |       "I agree! My sequence starts     |
  |       at 300. I got your 100."         |
  |                                         |
  |---------- ACK (Ack=301) -------------->|  
  | "Got it! I'm ready to send data"       |
  |                                         |
  |========== Data Transfer ===============>|
```

### Breaking Down Each Step:

**Step 1 - SYN (Synchronize)**:
- Client sends SYN packet with initial sequence number (ISN)
- ISN is randomly chosen for security
- SYN flag is set in TCP header
- No data is sent yet

**Step 2 - SYN-ACK (Synchronize-Acknowledge)**:
- Server responds with its own SYN (with its ISN)
- Also ACKs client's SYN (client's ISN + 1)
- Both SYN and ACK flags are set
- Server allocates resources for connection

**Step 3 - ACK (Acknowledge)**:
- Client acknowledges server's SYN (server's ISN + 1)
- ACK flag is set
- Client can start sending data in this packet
- Connection is now ESTABLISHED

### Why Three Steps?

**Two-way wouldn't work**:
- Both sides need to agree and acknowledge
- Prevents half-open connections
- Ensures both sides have allocated resources

**Four-way would be wasteful**:
- SYN-ACK combines server's SYN and ACK into one packet
- More efficient

### Connection Termination (Four-Way Handshake)

Closing a connection takes four steps:

```
Client                          Server
  |                               |
  |-------- FIN --------------->|  "I'm done sending"
  |                               |
  |<------- ACK ----------------|  "OK, got it"
  |                               |
  |<------- FIN ----------------|  "I'm done too"
  |                               |
  |-------- ACK --------------->|  "OK, bye!"
  |                               |
```

**Why Four Steps?**
- TCP is full-duplex (data flows both directions)
- Each direction must be closed independently
- Server may still have data to send after receiving FIN

**DevOps Example**: When you SSH into a server, the TCP handshake happens first, securing the connection before any SSH protocol communication begins.

## Ports

Ports are 16-bit numbers (0-65535) that help direct traffic to the right application on a host. Without ports, a computer wouldn't know whether incoming data is for the web server, email server, or SSH daemon.

### How Ports Work

When a packet arrives at a host:
1. **IP address** directs it to the correct computer
2. **Port number** directs it to the correct application

**Example**:
```
Packet arrives at server 192.168.1.10
- Port 80 → Web Server (nginx)
- Port 22 → SSH Daemon (sshd)
- Port 3306 → MySQL Database
- Port 5432 → PostgreSQL Database
```

### Socket: IP + Port

A **socket** is the combination of IP address and port number.

```
Socket = IP Address : Port

Examples:
- 192.168.1.10:80 (web server)
- 192.168.1.10:22 (SSH server)
- 192.168.1.10:3306 (MySQL server)
```

**Connection Identification**:
A TCP connection is uniquely identified by a 5-tuple:
1. Source IP
2. Source Port
3. Destination IP
4. Destination Port
5. Protocol (TCP/UDP)

Example: `192.168.1.5:54321 → 93.184.216.34:443 (TCP)`

### Port States

Applications can **listen** on ports:
- **LISTENING**: Application is waiting for connections
- **ESTABLISHED**: Active connection
- **TIME_WAIT**: Connection closed, waiting for delayed packets
- **CLOSE_WAIT**: Remote side closed connection

### Ephemeral Ports (Client Ports)

When your computer initiates a connection, it uses a temporary **ephemeral port** for its side:

```
Your Computer: 192.168.1.100:54321 (random ephemeral port)
     ↓
Web Server: 93.184.216.34:443 (well-known port)
```

**Ephemeral Port Range**:
- Linux: 32768-60999
- Windows: 49152-65535
- Can be configured

**Why Random?**
- Allows multiple simultaneous connections to same server
- Security: Harder for attackers to guess

### Common Ports

| Port | Service | Protocol |
|------|---------|----------|
| 22 | SSH | TCP |
| 80 | HTTP | TCP |
| 443 | HTTPS | TCP |
| 53 | DNS | UDP/TCP |
| 3306 | MySQL | TCP |
| 5432 | PostgreSQL | TCP |
| 6379 | Redis | TCP |
| 27017 | MongoDB | TCP |
| 3000 | Many dev servers | TCP |
| 8080 | Alternative HTTP | TCP |

### Port Ranges
- **0-1023**: Well-known ports (require root/admin)
- **1024-49151**: Registered ports
- **49152-65535**: Dynamic/private ports

## OSI Model (7 Layers)

The OSI model is a conceptual framework to understand networking:

| Layer | Name | What it Does | Example |
|-------|------|--------------|---------|
| 7 | Application | Apps and services | HTTP, FTP, SSH |
| 6 | Presentation | Data formatting | SSL/TLS, encryption |
| 5 | Session | Maintains connections | NetBIOS, RPC |
| 4 | Transport | End-to-end delivery | TCP, UDP |
| 3 | Network | Routing between networks | IP, ICMP |
| 2 | Data Link | Node-to-node transfer | Ethernet, MAC |
| 1 | Physical | Physical cables/signals | Cables, WiFi |

**Mnemonic**: "Please Do Not Throw Sausage Pizza Away"

## TCP/IP Model (4 Layers - More Practical)

| Layer | Equivalent OSI Layers | Protocols |
|-------|----------------------|-----------|
| Application | 5, 6, 7 | HTTP, DNS, SSH, FTP |
| Transport | 4 | TCP, UDP |
| Internet | 3 | IP, ICMP |
| Network Access | 1, 2 | Ethernet, WiFi |

## Real-World DevOps Examples

### Example 1: Deploying a Web Application
```
User Browser (Port 443) 
    ↓ TCP connection
Load Balancer (Port 443)
    ↓ TCP connection
Web Server (Port 8080)
    ↓ TCP connection
Database (Port 3306)
```

### Example 2: Kubernetes Service Communication
```
Frontend Pod:3000 → TCP → Backend Pod:8080 → TCP → Redis:6379
```

### Example 3: Debugging Connection Issues
```bash
# Check if a port is open
telnet example.com 80

# Better tool: netcat
nc -zv example.com 80

# Check which process is using a port
lsof -i :8080        # Mac/Linux
netstat -ano | findstr :8080  # Windows
```

## Understanding Network Traffic Flow

**When you visit https://example.com:**

1. **Application Layer**: Browser creates HTTP request
2. **Transport Layer**: TCP adds port numbers (443)
3. **Network Layer**: IP adds source and destination IP
4. **Data Link Layer**: Adds MAC addresses for next hop
5. **Physical Layer**: Converts to electrical signals

## Key Concepts for DevOps

### 1. Connection States
- `ESTABLISHED`: Active connection
- `LISTENING`: Waiting for connections
- `TIME_WAIT`: Connection closed, waiting for delayed packets

```bash
# View all connections
netstat -an
# or
ss -tuln
```

### 2. Firewalls and Ports
When deploying services, you need to open ports:
- AWS Security Groups
- Azure NSGs
- GCP Firewall Rules
- iptables on Linux

### 3. Health Checks
Load balancers often use TCP health checks:
```
Load Balancer → TCP SYN to backend:80
Backend → SYN-ACK (healthy) or timeout (unhealthy)
```

## Key Takeaways

✅ TCP is reliable but slower (HTTP, SSH)  
✅ UDP is fast but unreliable (streaming, DNS)  
✅ Ports identify specific services on a host  
✅ TCP handshake establishes connections  
✅ Understanding layers helps debug network issues  
✅ Most DevOps work happens at Layer 4 (Transport) and Layer 7 (Application)
