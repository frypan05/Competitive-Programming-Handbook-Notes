# Firewalls & Security Groups

## What is a Firewall?

A firewall is a security system that monitors and controls incoming and outgoing network traffic based on predetermined rules.

A firewall acts as a barrier between trusted internal networks and untrusted external networks (like the internet). The term "firewall" comes from physical firewalls in buildings that prevent fire from spreading.

### The Purpose of Firewalls

**Network Security Fundamentals**:
In networking, security is based on controlling what traffic is allowed:
- **Default Deny**: Block everything, then explicitly allow what's needed
- **Default Allow**: Allow everything, then block specific threats

Firewalls implement the "Default Deny" principle.

**What Firewalls Protect Against**:
1. **Unauthorized Access**: Prevent external attackers from reaching internal systems
2. **Port Scanning**: Hide which services are running
3. **Exploitation**: Block traffic to vulnerable services
4. **Data Exfiltration**: Control what data leaves the network
5. **DDoS Attacks**: Rate limit and filter malicious traffic
6. **Malware Communication**: Block malware from calling home

### Firewall Evolution

**1st Generation - Packet Filtering** (1980s):
```
Check each packet independently:
- Source IP
- Destination IP
- Source Port
- Destination Port
- Protocol (TCP/UDP)

Allow or Deny based on simple rules
```

**2nd Generation - Stateful Inspection** (1990s):
```
Track connection state:
- Remember outbound connections
- Automatically allow return traffic
- Understand TCP handshake
- Much smarter than packet filtering
```

**3rd Generation - Application Layer** (2000s):
```
Inspect application data:
- Understand HTTP, FTP, DNS protocols
- Deep packet inspection
- Block specific URLs, file types
- Intrusion Prevention System (IPS)
```

**4th Generation - Next-Gen Firewalls** (2010s+):
```
Advanced capabilities:
- SSL/TLS decryption
- User identity awareness
- Cloud integration
- Machine learning threat detection
```

## Firewall Concepts

### Stateful vs Stateless

**Stateless Firewall** (Packet Filter):

**How it works**:
```
Each packet evaluated independently:

Inbound Packet:
  From: 203.0.113.5:54321
  To: 192.168.1.10:80
  Protocol: TCP
  
  Check rules:
  Rule 1: Allow TCP to port 80 → MATCH → ALLOW
```

**Problem**:
```
You need TWO rules:

Rule 1 (Inbound): Allow TCP from anywhere to port 80
Rule 2 (Outbound): Allow TCP from port 80 to anywhere

Without Rule 2, server can't respond!
```

**Characteristics**:
- Fast (simple comparison)
- No memory of connections
- Requires rules for both directions
- Can't distinguish legitimate responses from attacks

**Stateful Firewall**:

**How it works**:
```
Tracks connection state:

1. Outbound request:
   Your PC:54321 → Web Server:443
   Firewall remembers: "Connection from 192.168.1.10:54321 to 93.184.216.34:443"
   
2. Inbound response:
   Web Server:443 → Your PC:54321
   Firewall checks: "Is this a response to an existing connection?"
   Yes → ALLOW (even without explicit rule)
```

**State Table Example**:
```
Connection Table:
[192.168.1.10:54321 ↔ 93.184.216.34:443] State: ESTABLISHED
[192.168.1.10:54322 ↔ 1.1.1.1:53]         State: ESTABLISHED
[192.168.1.10:54323 ↔ 10.0.0.5:3306]      State: ESTABLISHED
```

**Advantages**:
- Smarter security (understands context)
- Fewer rules needed
- Automatically allows legitimate return traffic
- Can detect connection hijacking
- Understands TCP connection states

**Connection States**:
```
NEW: New connection attempt
ESTABLISHED: Active connection
RELATED: Related to existing connection (e.g., FTP data channel)
INVALID: Malformed or unexpected packet
```

**Stateful Rule Example**:
```
# One rule is enough:
Allow outbound to port 443

Firewall automatically:
- Tracks outbound connection
- Allows return traffic
- Drops unsolicited inbound traffic
```

### Inbound vs Outbound Rules

**Inbound (Ingress) Rules**:
Traffic coming TO your server/network from outside.

**Example Scenario**:
```
External Client → Your Web Server

Inbound Rule Needed:
- Allow TCP port 80 (HTTP)
- Allow TCP port 443 (HTTPS)
- Allow TCP port 22 from specific IP (SSH)
```

**Default Policy**: Usually DENY (for security)

**Outbound (Egress) Rules**:
Traffic going FROM your server/network to outside.

**Example Scenario**:
```
Your Server → External API
Your Server → Update servers
Your Server → Database
```

**Default Policy**: Often ALLOW (to not break applications)

**Why Control Outbound?**:
1. **Data Exfiltration**: Prevent stolen data from leaving
2. **Malware Communication**: Block malware from calling command & control servers
3. **Compliance**: Some regulations require outbound filtering
4. **Cost**: In cloud, outbound bandwidth costs money

**Strict Outbound Example**:
```
Default: DENY all outbound

Explicit Allow:
- Port 443 to apt.ubuntu.com (package updates)
- Port 443 to specific APIs
- Port 3306 to 10.0.2.5 (internal database)
```

## Common Firewall Rules

Firewall rules are evaluated in order. First match wins (in most firewalls).

### Rule Structure

A firewall rule typically contains:
```
1. Priority/Order: Which rule to evaluate first
2. Action: ALLOW or DENY
3. Protocol: TCP, UDP, ICMP, or ALL
4. Source: IP address or range
5. Source Port: Usually ANY
6. Destination: IP address or range
7. Destination Port: Specific port or range
8. State: NEW, ESTABLISHED, RELATED (stateful only)
```

**Example Rule**:
```
Priority: 100
Action: ALLOW
Protocol: TCP
Source: 0.0.0.0/0 (anywhere)
Destination: 192.168.1.10 (web server)
Port: 443
State: NEW, ESTABLISHED
```

### Rule Evaluation Order

```
Traffic arrives → Check Rule 1
                 ↓ No match
                Check Rule 2
                 ↓ No match
                Check Rule 3
                 ↓ MATCH → Apply action → DONE
                (Remaining rules not evaluated)
                
If no rules match → Default Policy (usually DENY)
```

**Why Order Matters**:
```
❌ Bad Order:
Rule 1: DENY all from 203.0.113.0/24
Rule 2: ALLOW TCP port 80 from 203.0.113.5

Result: 203.0.113.5 is blocked (Rule 1 matches first)

✓ Good Order:
Rule 1: ALLOW TCP port 80 from 203.0.113.5
Rule 2: DENY all from 203.0.113.0/24

Result: 203.0.113.5 is allowed (Rule 1 matches first)
```

### Common Firewall Rule Patterns

### Allow Web Traffic
```
Inbound:
  Port 80 (HTTP)     from 0.0.0.0/0 (anywhere)
  Port 443 (HTTPS)   from 0.0.0.0/0 (anywhere)
```

### Allow SSH (Restricted)
```
Inbound:
  Port 22 (SSH)      from 203.0.113.0/24 (your office IP range)
```

### Allow Database (Internal Only)
```
Inbound:
  Port 3306 (MySQL)  from 10.0.1.0/24 (app servers subnet)
```

### Allow All Outbound (Common)
```
Outbound:
  All traffic        to 0.0.0.0/0 (anywhere)
```

## Real-World Examples

### Example 1: Web Server Setup
```
┌─────────────────────────────────┐
│ Web Server Security Group       │
├─────────────────────────────────┤
│ Inbound:                        │
│  - Port 80   from 0.0.0.0/0     │
│  - Port 443  from 0.0.0.0/0     │
│  - Port 22   from 1.2.3.4/32    │ ← Your office IP
│                                 │
│ Outbound:                       │
│  - All traffic to 0.0.0.0/0     │
└─────────────────────────────────┘
```

### Example 2: Three-Tier Application
```
Internet → Load Balancer → Web Servers → App Servers → Database

Load Balancer:
  Inbound: 443 from 0.0.0.0/0

Web Servers:
  Inbound: 8080 from Load Balancer only
  Outbound: 5000 to App Servers

App Servers:
  Inbound: 5000 from Web Servers only
  Outbound: 3306 to Database

Database:
  Inbound: 3306 from App Servers only
  Outbound: None (or minimal)
```

### Example 3: Microservices in Kubernetes
```
Frontend Pod:
  - Allow inbound from Ingress Controller
  - Allow outbound to Backend Service

Backend Pod:
  - Allow inbound from Frontend only
  - Allow outbound to Database

Database Pod:
  - Allow inbound from Backend only
  - No external access
```

## Linux Firewall (iptables/ufw)

### Using UFW (Ubuntu/Debian - Easy)

```bash
# Enable firewall
sudo ufw enable

# Allow SSH (do this first!)
sudo ufw allow 22/tcp

# Allow HTTP and HTTPS
sudo ufw allow 80/tcp
sudo ufw allow 443/tcp

# Allow specific IP to specific port
sudo ufw allow from 192.168.1.100 to any port 3306

# Allow subnet to port
sudo ufw allow from 10.0.1.0/24 to any port 5432

# Deny specific IP
sudo ufw deny from 203.0.113.0

# Check status
sudo ufw status verbose

# Delete rule
sudo ufw delete allow 80/tcp

# Disable firewall
sudo ufw disable
```

### Using iptables (More Control)

```bash
# List current rules
sudo iptables -L -n -v

# Allow SSH
sudo iptables -A INPUT -p tcp --dport 22 -j ACCEPT

# Allow HTTP and HTTPS
sudo iptables -A INPUT -p tcp --dport 80 -j ACCEPT
sudo iptables -A INPUT -p tcp --dport 443 -j ACCEPT

# Allow from specific IP
sudo iptables -A INPUT -s 192.168.1.100 -j ACCEPT

# Block specific IP
sudo iptables -A INPUT -s 203.0.113.0 -j DROP

# Allow established connections
sudo iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT

# Drop everything else
sudo iptables -P INPUT DROP

# Save rules (Ubuntu/Debian)
sudo iptables-save > /etc/iptables/rules.v4
```

## AWS Security Groups

### Example: Web Application
```
Security Group: web-servers
├── Inbound Rules:
│   ├── HTTP (80)     Source: 0.0.0.0/0
│   ├── HTTPS (443)   Source: 0.0.0.0/0
│   └── SSH (22)      Source: sg-admin (admin security group)
│
└── Outbound Rules:
    └── All traffic   Destination: 0.0.0.0/0

Security Group: app-servers
├── Inbound Rules:
│   └── Custom TCP (8080)   Source: sg-web-servers
│
└── Outbound Rules:
    └── MySQL (3306)   Destination: sg-database

Security Group: database
├── Inbound Rules:
│   └── MySQL (3306)   Source: sg-app-servers
│
└── Outbound Rules:
    └── None (deny all)
```

## Best Practices

### 1. Principle of Least Privilege
Only allow what's necessary.

❌ **Bad**:
```
Allow all traffic from 0.0.0.0/0
```

✅ **Good**:
```
Allow port 443 from 0.0.0.0/0
Allow port 22 from 1.2.3.4/32 (specific IP)
```

### 2. Use Security Groups, Not IP Addresses (Cloud)
Instead of hardcoding IPs, reference other security groups.

```
Database:
  Allow 3306 from sg-app-servers (not from 10.0.1.5)
```

### 3. Document Your Rules
```
Rule: Allow 8080
Purpose: Application API endpoint
Source: Load balancer security group
Added by: John (2024-01-15)
```

### 4. Regular Audits
- Review rules quarterly
- Remove unused rules
- Check for overly permissive rules (0.0.0.0/0)

### 5. Use Separate Security Groups by Role
```
sg-web
sg-app
sg-database
sg-admin
sg-monitoring
```

### 6. Block by Default
Start with "deny all" and explicitly allow what's needed.

## Common Firewall Patterns

### Bastion Host (Jump Server)
```
Internet → Bastion (SSH:22 from your IP)
               ↓
           Private Servers (SSH:22 from Bastion only)
```

### DMZ (Demilitarized Zone)
```
Internet → Firewall → DMZ (Public servers)
                      Firewall → Internal Network (Private servers)
```

### Network Segmentation
```
10.0.1.0/24 - Web tier
10.0.2.0/24 - App tier
10.0.3.0/24 - Data tier

Each tier has firewall rules controlling access
```

## Troubleshooting

### Issue: Can't Connect to Server
```bash
# 1. Check if port is open locally
sudo netstat -tuln | grep 8080

# 2. Check if firewall is blocking
sudo ufw status
# or
sudo iptables -L -n

# 3. Test from external machine
telnet server-ip 8080
# or
nc -zv server-ip 8080

# 4. Check cloud security groups (AWS/Azure/GCP)
```

### Issue: Connection Times Out vs Connection Refused
- **Timeout**: Firewall is blocking (packet dropped)
- **Refused**: Port is closed, but firewall allows it through

### Common Mistakes

❌ **Locking yourself out via SSH**:
```bash
# Always allow SSH before enabling firewall!
sudo ufw allow 22
sudo ufw enable
```

❌ **Forgetting to save rules**:
```bash
# Rules are lost on reboot unless saved
sudo iptables-save > /etc/iptables/rules.v4
```

❌ **Opening unnecessary ports**:
```bash
# Don't do this unless you really need it
sudo ufw allow from 0.0.0.0/0 to any
```

## Cloud Firewall Commands

### AWS CLI
```bash
# List security groups
aws ec2 describe-security-groups

# Create security group
aws ec2 create-security-group \
  --group-name web-sg \
  --description "Web servers"

# Add inbound rule
aws ec2 authorize-security-group-ingress \
  --group-id sg-123456 \
  --protocol tcp \
  --port 80 \
  --cidr 0.0.0.0/0
```

## Key Takeaways

✅ Firewalls control inbound and outbound traffic  
✅ Default deny, explicitly allow what's needed  
✅ Use security groups to reference other groups (cloud)  
✅ Separate security groups by role/tier  
✅ Always allow SSH before enabling firewall (to avoid lockout)  
✅ Regular audits to remove unnecessary rules  
✅ Test connectivity after making changes  
✅ Document why each rule exists
