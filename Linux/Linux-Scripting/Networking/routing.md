# Networking Fundamentals: Routers, Routing, and Protocols

## 1. What is a Router?

A **router** is a networking device that enables machines on a network to communicate with each other and with other networks.

### Key Components

- **LAN ports**: Connect machines to the same local area network
- **Internet uplink port**: Connects to the Internet (sometimes labeled as WAN - Wide Area Network)

### Function

- Routes packets between multiple networks
- Decides where network packets go and which ones come in
- Facilitates communication from source host to destination host

### Mail Delivery Analogy

Routing works like mail delivery - when you send a letter to California, the post office sees the destination and puts it on the truck going to California. The letter then gets sent to San Francisco, then to specific zip codes, then to smaller address codes, until finally delivered to the exact address.

### Key Networking Concepts

- **Switching**: Receiving, processing, and forwarding data to the destination device
- **Routing**: Process of creating the routing table to enable better switching
- **Flooding**: Legacy method where packets are sent through every outgoing link except the one they arrived on (used before routing was implemented)

## 2. Routing Table

The routing table is a system's navigation guide across networks.

### Example Routing Table

```txt
Kernel IP routing table
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
0.0.0.0         192.168.224.2   0.0.0.0         UG    0      0        0 eth0
192.168.224.0   0.0.0.0         255.255.255.0   U     1      0        0 eth0
```

### Field Explanations

#### Destination

- **192.168.224.0**: Any packet going to this network goes out through eth0
- **0.0.0.0**: No address specified/unknown - packets go to the Gateway
- **Example**: If you're 192.168.224.5 wanting to reach 192.168.224.7, use eth0 directly

#### Gateway

- Address where packets not on the same network are sent
- Acts as a "Gateway to another network"
- **Example**: 192.168.224.2 in the table above

#### Genmask

- Subnet mask used to determine which IP addresses match which destination
- **Example**: 255.255.255.0 for local network

#### Flags

- **UG**: Network is Up and is a Gateway
- **U**: Network is Up

#### Iface

- Interface the packet will use to exit
- **eth0**: Usually the first Ethernet device on your system

## 3. Path of a Packet

### Within Local Network

1. Local machine compares destination IP to see if it's in the same subnet (using subnet mask)
2. Packet needs: source MAC, destination MAC, source IP, destination IP
3. Use **ARP (Address Resolution Protocol)** to broadcast and find destination MAC address
4. Packet successfully sent

### Outside Local Network

1. Local machine compares destination IP - it's outside the network
2. Can't use ARP (only works for locally connected hosts)
3. Packet consults routing table, doesn't know destination, sends to default gateway
4. Router receives packet, confirms destination MAC but sees it's not final destination
5. Router forwards packet, **stripping old MAC addresses and updating with new ones**
6. Process repeats until packet reaches same network as destination
7. Final ARP request to find destination MAC address

### Important Note

- MAC addresses change at each hop
- **IP addresses remain constant throughout the journey**

## 4. Routing Protocols

Routing protocols help systems adapt to network changes automatically, eliminating manual route configuration.

### Convergence

- When routers communicate and agree on network topology
- All routing tables map complete network topology
- Temporarily breaks when network changes occur until all routers are updated

## 5. Distance Vector Protocols

### Characteristics

- Determine paths using **hop count**
- Choose route with least hops
- **Example**: If network A is 3 hops away and network B is next to A, assume B is 4 hops away

### Pros and Cons

- **Good for**: Small networks
- **Limitations**:
  - Slow convergence (sends entire routing table periodically)
  - May not choose most efficient route (only considers hop count)

### Example Protocol: RIP (Routing Information Protocol)

- Broadcasts routing table to every router every **30 seconds**
- Hop count limit: **15 hops maximum**
- Resource intensive for large networks

## 6. Link State Protocols

### Link State Characteristics

- Ideal for large-scale networks
- More complex than distance vector protocols
- **Quick convergence** - only send updates to neighboring routes
- Use shortest path first algorithm
- Construct network topology as a graph

### Example Protocol: OSPF (Open Shortest Path First)

- Updates routing tables **only when network changes occur**
- **No hop limit**
- More efficient than distance vector protocols

## 7. Border Gateway Protocol (BGP)

### BGP Function

- **"How the Internet runs"**
- Collects and exchanges routing information among autonomous systems
- Routes **between** autonomous systems (not within them)

### Autonomous Systems

- Internet service providers
- Companies
- Universities
- Any organization with its own network

### Real-World Example

You're on your home network, someone works from Starbucks. To communicate:

1. Email packet travels through Starbucks network
2. Bounces through Starbucks routing tables
3. Reaches border of Starbucks network
4. Passes to **Border Gateway router**
5. Router contains information to leave Starbucks network and traverse other networks
6. Eventually reaches your home network

### Key Point

Without BGP, different autonomous systems would be "siloed off" and unable to communicate with each other.

## Summary of Key Concepts

- **Hops**: Measure of distance packets travel (intermediate devices like routers)
- **MAC addresses**: Change at each network segment
- **IP addresses**: Remain constant throughout packet journey
- **ARP**: Used to find MAC addresses within local networks
- **Routing protocols**: Automate route discovery and maintenance
- **BGP**: Enables Internet-wide communication between different organizations/networks
