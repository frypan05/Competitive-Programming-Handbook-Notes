# Docker Networking

## What is Docker Networking?

Docker networking allows containers to communicate with each other and with the outside world.

### The Container Networking Problem

**Traditional VMs**:
```
Each VM has:
- Virtual network adapter
- Own IP address
- Acts like physical machine
- Full network stack

Simple, but heavy
```

**Containers**:
```
Containers share host kernel
Multiple containers on one host
Need network isolation between containers
But also need to communicate

Challenge: Provide isolation + connectivity efficiently
```

**Docker's Solution**:
```
Network Namespaces: Isolate network stack per container
Virtual Ethernet Pairs: Connect containers to networks
Bridge Networks: Software switches connecting containers
Port Mapping: NAT to expose containers to host
```

### Linux Network Namespaces

**What is a Network Namespace?**

A namespace is an isolated network stack:
```
Each namespace has its own:
- Network interfaces
- IP addresses
- Routing table
- Firewall rules
- Port space

Containers share kernel but have separate network namespaces
```

**How It Works**:
```
Host Network Namespace:
- eth0: 192.168.1.100
- Ports: SSH on 22, etc.

Container 1 Namespace:
- eth0: 172.17.0.2 (different from host eth0!)
- Ports: App on 80 (doesn't conflict with host)

Container 2 Namespace:
- eth0: 172.17.0.3
- Ports: App on 80 (doesn't conflict with container 1!)

Complete isolation: Each has its own "localhost"
```

### Virtual Ethernet Pairs (veth)

**The Connection Mechanism**:
```
veth pair = Two virtual network interfaces connected like a cable

One end in host namespace
Other end in container namespace

        Host Namespace          |    Container Namespace
                               |
    vethXXXX (host side) ←---cable---→ eth0 (container side)
         ↓                     |           ↓
    Docker Bridge             |       App in container
```

**Data Flow**:
```
1. Container sends packet to eth0
2. Packet travels through veth pair
3. Emerges at vethXXXX on host
4. Host processes/routes packet
```

**Example**:
```
$ docker inspect container1 | grep veth
Host side: veth4a5b6c

$ ip link show veth4a5b6c
veth4a5b6c@if12: connected to docker0 bridge
```

## Docker Network Drivers

Docker provides different network drivers for different use cases. Each implements networking differently.

### 1. Bridge (Default)

**What It Is**: Software network switch (Linux bridge)

**How It Works**:
```
Docker creates a bridge interface on host: docker0

        Host
         |
    docker0 bridge (172.17.0.1)
         |
    ┌────┴────┬────────┐
    |         |        |
  veth1     veth2    veth3
    |         |        |
Container1 Container2 Container3
172.17.0.2 172.17.0.3 172.17.0.4
```

**Bridge = Layer 2 Switch**:
```
Works like physical Ethernet switch:
- Learns MAC addresses
- Forwards frames between ports
- Isolates broadcast domains
```

**Packet Flow (Container to Container)**:
```
1. Container1 sends to Container2 (172.17.0.3)
2. Packet → Container1's eth0
3. → veth pair → Host's vethXXX
4. → docker0 bridge (looks up MAC address)
5. → vethYYY (Container2's veth)
6. → veth pair → Container2's eth0
7. Container2 receives packet

All happens at Layer 2 (MAC addresses)
Very fast - no routing needed
```

**Packet Flow (Container to Internet)**:
```
1. Container1 sends to 8.8.8.8
2. Packet → docker0 bridge
3. docker0: "Not a local container, send to host"
4. Host: NAT translation (SNAT)
   Source: 172.17.0.2:54321 → 192.168.1.100:54321
5. Host routes to internet
6. Response comes back
7. Host: Reverse NAT (DNAT)
   Dest: 192.168.1.100:54321 → 172.17.0.2:54321
8. Packet → docker0 → Container1

NAT allows containers to access internet
Internet sees host IP, not container IP
```

**Default Bridge Limitations**:
```
- No automatic DNS resolution (can't use container names)
- All containers on same network (no isolation)
- Legacy features

Solution: User-defined bridges (recommended)
```

**User-Defined Bridge**:
```
docker network create my-network

Benefits:
- Automatic DNS resolution (container name → IP)
- Better isolation
- On-the-fly container addition/removal
- Configurable (subnet, gateway, etc.)
```

**Bridge Network Characteristics**:
- **Isolation**: Containers on different bridges can't communicate
- **Performance**: Fast (all Layer 2 switching)
- **Use Case**: Single host, multiple containers

### 2. Host

**What It Is**: Container shares host's network namespace (no isolation)

**How It Works**:
```
Normal Container:
Container Namespace → veth → Host Namespace

Host Network:
Container uses Host Namespace directly (no veth, no bridge)
```

**Configuration**:
```
docker run --network host nginx

Container's eth0 = Host's eth0
Container's localhost = Host's localhost
Container sees all host's network interfaces
```

**Implications**:
```
Container starts service on port 80:
- Service binds to host's port 80
- No port mapping needed (-p flag ignored)
- Conflicts with host services on port 80
- Other containers can't use port 80
```

**Performance**:
```
Bridge Network:
App → Container namespace → veth → bridge → veth → Network

Host Network:
App → Direct to network interface

Result: ~10% faster (no namespace/veth overhead)
```

**Advantages**:
- Maximum performance (no NAT, no veth)
- No IP address management needed
- Direct access to host network

**Disadvantages**:
- No network isolation (security concern)
- Port conflicts between containers
- Can't run multiple containers with same port
- Less portable

**Use Case**:
- Network performance critical (high throughput)
- Network monitoring tools (need to see host traffic)
- Simple single-container deployments

### 3. None

**What It Is**: No networking at all (complete isolation)

**Configuration**:
```
docker run --network none my-app

Container has:
- Only loopback interface (lo)
- No eth0
- No external connectivity
- localhost works (127.0.0.1)
```

**Use Cases**:
- Maximum security (airgapped containers)
- Containers that only process local files
- Batch jobs that don't need network
- Testing network-less scenarios

**Custom Networking**:
```
Start with none, then manually configure:

docker run --network none --name app my-app

ip link add veth0 type veth peer name veth1
ip link set veth1 netns container-namespace
# Manual network configuration

Full control, but complex
```

### 4. Overlay

**What It Is**: Multi-host network using VXLAN tunneling

**The Multi-Host Problem**:
```
Host A (10.0.1.10):
  Container1 (172.18.0.2)
  
Host B (10.0.1.20):
  Container2 (172.18.0.3)

Problem: How can Container1 and Container2 communicate?
They're on different physical hosts!
```

**Overlay Solution**:
```
Creates virtual Layer 2 network across hosts:

Container1 → Overlay Network → Container2
(Host A)    (VXLAN tunnel)    (Host B)

Container1 thinks Container2 is on same network
Reality: Packets tunneled through host network
```

**How It Works (VXLAN)**:
```
1. Container1 sends frame to Container2
   Frame: [Src: 172.18.0.2][Dst: 172.18.0.3][Data]

2. Host A encapsulates in VXLAN:
   [Host A IP][UDP Header][VXLAN][Original Frame]
   
3. Packet sent to Host B over physical network

4. Host B decapsulates:
   Extracts: [Original Frame]
   
5. Delivers to Container2

From containers' perspective: Same Layer 2 network
Reality: Tunneled through Layer 3
```

**Overlay Network Architecture**:
```
        Container1              Container2
        (Host A)                (Host B)
            ↓                       ↓
        Overlay                 Overlay
        Driver                  Driver
            ↓                       ↓
    VXLAN Encap            VXLAN Decap
            ↓                       ↓
        ═══════ UDP Tunnel ═══════
                (Internet)
```

**Key-Value Store** (Required for Overlay):
```
Overlay requires distributed state:
- Which containers are on which hosts?
- What are their IPs/MACs?
- Where to route traffic?

Docker Swarm: Built-in key-value store
Manual: etcd, Consul, Zookeeper

Without this: Hosts don't know where to send packets
```

**Advantages**:
- Containers on different hosts communicate seamlessly
- Scalable (add more hosts easily)
- Isolated overlay networks
- Built into Docker Swarm

**Disadvantages**:
- More complex
- Slightly slower (encapsulation overhead)
- Requires clustering/orchestration
- Higher CPU usage

**Use Case**:
- Docker Swarm
- Multi-host container deployments
- Microservices across servers

### 5. Macvlan

**What It Is**: Container gets own MAC address on physical network

**How It Works**:
```
Traditional Bridge:
Container (172.17.0.2) → NAT → Host (192.168.1.100) → Network

Macvlan:
Container (192.168.1.150) → Directly on physical network

Container appears as physical device on LAN
No NAT, no bridge
```

**Configuration**:
```
Physical network: 192.168.1.0/24
Physical interface: eth0

docker network create -d macvlan \
  --subnet=192.168.1.0/24 \
  --gateway=192.168.1.1 \
  -o parent=eth0 macvlan-net

Container gets IP from physical subnet: 192.168.1.150
```

**Packet Flow**:
```
Container sends packet:
1. Packet → Physical network directly (no NAT)
2. Other devices see container as separate device
3. Router sees container's MAC address
4. No indication it's a container

External view: Another device on network
Reality: Container on host
```

**Limitations**:
```
- Host can't communicate with own macvlan containers (by design)
- Requires promiscuous mode on physical NIC (some environments don't allow)
- Limited MAC addresses per interface (switch limits)
- More complex troubleshooting
```

**Use Cases**:
- Legacy applications expecting physical network
- Need containers to be first-class network citizens
- DHCP services in containers
- Network monitoring/scanning tools
- When NAT is unacceptable

## Default Docker Networks

```bash
# List networks
docker network ls

# Output:
NETWORK ID     NAME      DRIVER    SCOPE
abc123         bridge    bridge    local
def456         host      host      local
ghi789         none      null      local
```

## Creating Custom Networks

### Create a bridge network:
```bash
# Basic network
docker network create my-network

# With specific subnet
docker network create \
  --subnet=172.20.0.0/16 \
  my-custom-network

# Inspect network
docker network inspect my-network
```

### Run containers on custom network:
```bash
# Start container on custom network
docker run -d --name web --network my-network nginx

# Start another container on same network
docker run -d --name api --network my-network node:14
```

## Container Communication

### By Container Name (DNS)
Containers on the same user-defined network can communicate by name.

```bash
# Create network
docker network create app-network

# Start database
docker run -d \
  --name postgres-db \
  --network app-network \
  postgres:14

# Start application (connects to database by name)
docker run -d \
  --name my-app \
  --network app-network \
  -e DATABASE_URL=postgresql://postgres-db:5432/mydb \
  my-app-image
```

Inside `my-app` container:
```
# Can ping by name
ping postgres-db  # Works! Resolves to container IP
```

### By IP Address
```bash
# Get container IP
docker inspect postgres-db | grep IPAddress

# Use IP in another container
# But using names is better (IPs can change)
```

## Port Mapping

### Publishing Ports

```bash
# Map container port to host port
docker run -p 8080:80 nginx
# Host:8080 → Container:80

# Map to random host port
docker run -P nginx
# Docker assigns random port

# Specific interface
docker run -p 127.0.0.1:8080:80 nginx
# Only accessible from localhost

# Multiple ports
docker run -p 80:80 -p 443:443 nginx
```

### How Port Mapping Works

```
External Request → Host:8080
                    ↓ (iptables NAT)
                Container:80 (nginx)
```

## Real-World Examples

### Example 1: Web App with Database
```bash
# Create network
docker network create myapp-network

# Start PostgreSQL
docker run -d \
  --name postgres \
  --network myapp-network \
  -e POSTGRES_PASSWORD=secret \
  postgres:14

# Start Redis
docker run -d \
  --name redis \
  --network myapp-network \
  redis:7

# Start application
docker run -d \
  --name webapp \
  --network myapp-network \
  -p 3000:3000 \
  -e DATABASE_URL=postgresql://postgres:5432/mydb \
  -e REDIS_URL=redis://redis:6379 \
  my-webapp
```

Access: `http://localhost:3000`

### Example 2: Microservices Architecture
```bash
# Create network
docker network create microservices

# User service
docker run -d --name user-service --network microservices -p 8001:8000 user-service-image

# Order service  
docker run -d --name order-service --network microservices -p 8002:8000 order-service-image

# API Gateway
docker run -d --name api-gateway --network microservices -p 80:8000 \
  -e USER_SERVICE_URL=http://user-service:8000 \
  -e ORDER_SERVICE_URL=http://order-service:8000 \
  api-gateway-image
```

Services communicate by name internally.

### Example 3: Docker Compose (Best Practice)
```yaml
# docker-compose.yml
version: '3.8'

services:
  web:
    image: nginx
    ports:
      - "80:80"
    networks:
      - frontend
      - backend
  
  api:
    image: my-api
    networks:
      - backend
    environment:
      - DATABASE_URL=postgresql://db:5432/mydb
  
  db:
    image: postgres:14
    networks:
      - backend
    environment:
      - POSTGRES_PASSWORD=secret
    volumes:
      - db-data:/var/lib/postgresql/data

networks:
  frontend:
  backend:

volumes:
  db-data:
```

Start with: `docker-compose up -d`

## Container DNS Resolution

Docker provides automatic DNS resolution for container names.

```
Container: web
    ↓ (queries)
Docker DNS (127.0.0.11)
    ↓ (resolves)
Container: api (172.18.0.3)
```

### Check DNS inside container:
```bash
# Enter container
docker exec -it web sh

# Check DNS resolver
cat /etc/resolv.conf
# nameserver 127.0.0.11

# Test DNS resolution
ping api
# Resolves to api's container IP
```

## Network Isolation

Containers on different networks can't communicate (by default).

```
Network: frontend
  - web container

Network: backend
  - database container

web can't reach database (isolated)
```

### Connect container to multiple networks:
```bash
# Create networks
docker network create frontend
docker network create backend

# Run container on both networks
docker run -d --name app \
  --network frontend \
  my-app

# Connect to second network
docker network connect backend app
```

## Common Networking Commands

```bash
# List networks
docker network ls

# Create network
docker network create my-network

# Inspect network (see connected containers)
docker network inspect my-network

# Remove network
docker network rm my-network

# Remove all unused networks
docker network prune

# Connect running container to network
docker network connect my-network my-container

# Disconnect container from network
docker network disconnect my-network my-container

# Check container's networks
docker inspect my-container | grep -A 10 Networks
```

## Troubleshooting

### Issue: Containers Can't Communicate

**Check if on same network**:
```bash
docker network inspect my-network
# Look for both containers in "Containers" section
```

**Test connectivity**:
```bash
docker exec container1 ping container2

# If name doesn't resolve, use IP
docker exec container1 ping 172.18.0.3
```

### Issue: Port Already in Use
```
Error: bind: address already in use
```

**Solution**:
```bash
# Find process using port
lsof -i :8080
# or
netstat -tulpn | grep 8080

# Kill process or use different port
docker run -p 8081:80 nginx
```

### Issue: Can't Access Container from Host
```bash
# Ensure port is published
docker ps
# Check PORTS column

# Try localhost
curl http://localhost:8080

# Try host IP
curl http://192.168.1.100:8080

# Check if service is listening inside container
docker exec my-container netstat -tuln
```

## Docker Network Security

### 1. Isolate Services
```bash
# Frontend network (public-facing)
docker network create frontend

# Backend network (internal only)
docker network create backend

# Database only on backend
docker run --network backend postgres
```

### 2. Disable Inter-Container Communication
```bash
# Create isolated network
docker network create --internal private-network

# Containers can't reach external networks
```

### 3. Use Custom Subnets
```bash
# Avoid conflicts with existing networks
docker network create \
  --subnet=172.25.0.0/16 \
  my-isolated-network
```

## Docker Network Performance

### Best Practices:

**1. Use Host Network for High Performance**
```bash
docker run --network host nginx
# No network translation overhead
# But: Less isolation
```

**2. Use Overlay Networks for Multi-Host**
```bash
# Docker Swarm automatically uses overlay
docker service create --name web --network my-overlay nginx
```

**3. Limit Container Networks**
```bash
# Don't connect containers to unnecessary networks
# Each network adds overhead
```

## Common Patterns

### Pattern 1: Frontend-Backend Separation
```
Internet → Frontend Network → Web Container
           Backend Network  → API Container
                             → Database Container
```

### Pattern 2: Service Mesh (Multiple Networks)
```
Container A:
  - frontend network
  - backend network
  - monitoring network
```

### Pattern 3: Zero-Trust (Isolated by Default)
```
Each service on its own network
Explicit connections only when needed
```

## Key Takeaways

✅ Docker creates isolated networks for containers  
✅ Bridge is the default network driver  
✅ Containers on same network can communicate by name  
✅ Use custom networks instead of default bridge  
✅ Port mapping: `-p host:container`  
✅ Docker Compose automatically creates networks  
✅ DNS resolution works for container names  
✅ Network isolation improves security  
✅ Use `docker network` commands to manage networks  
✅ Always use user-defined networks for production
