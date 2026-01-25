# IP Addressing & Subnetting

## What is an IP Address?

An IP address is like a postal address for your computer on a network. It helps devices find and communicate with each other.

An IP (Internet Protocol) address is a unique numerical label assigned to each device participating in a computer network. It serves two principal functions:
1. **Host or network interface identification** - Uniquely identifies a device
2. **Location addressing** - Provides the location of the device in the network

### How IP Addressing Works

When data travels across networks, it's broken into packets. Each packet contains:
- **Source IP address**: Where the packet came from
- **Destination IP address**: Where the packet is going
- **Payload**: The actual data being transmitted

Routers use IP addresses to determine the best path for packets to reach their destination. Think of it as a postal system where each house has a unique address, and the postal service uses these addresses to deliver mail.

### The Binary Nature of IP Addresses

IP addresses are actually binary numbers that humans read in decimal format for convenience.

**Example**: `192.168.1.1` is actually:
```
11000000.10101000.00000001.00000001
```

Each decimal number (octet) represents 8 bits, which is why:
- Minimum value: 0 (00000000 in binary)
- Maximum value: 255 (11111111 in binary)

Understanding this binary nature is crucial for understanding subnetting and network masks.

## Types of IP Addresses

### 1. IPv4 (Most Common)
- Format: `192.168.1.1`
- 4 numbers separated by dots
- Each number ranges from 0 to 255
- Example: `172.16.0.10`

### 2. IPv6 (Newer Version)
- Format: `2001:0db8:85a3:0000:0000:8a2e:0370:7334`
- Designed to replace IPv4 as we run out of addresses
- Example: `fe80::1`

## IP Address Classes (IPv4)

| Class | Range | Use Case | Example |
|-------|-------|----------|---------|
| A | 1.0.0.0 - 126.255.255.255 | Large networks | 10.0.0.0 |
| B | 128.0.0.0 - 191.255.255.255 | Medium networks | 172.16.0.0 |
| C | 192.0.0.0 - 223.255.255.255 | Small networks | 192.168.1.0 |

## Private vs Public IP Addresses

### Private IP Addresses (Used in your home/office)
- `10.0.0.0` - `10.255.255.255`
- `172.16.0.0` - `172.31.255.255`
- `192.168.0.0` - `192.168.255.255`

**Example**: Your laptop at home might have `192.168.1.5` - this is private and only visible within your home network.

### Public IP Addresses
- Unique across the entire internet
- Assigned by your ISP
- Example: `8.8.8.8` (Google's DNS server)

## Subnetting Basics

Subnetting divides a network into smaller networks (subnets). This is fundamental to network design and allows for:
- **Efficient IP address allocation**: Don't waste addresses
- **Network segmentation**: Separate departments, services, or security zones
- **Performance improvement**: Smaller broadcast domains = less network congestion
- **Security**: Isolate sensitive systems from general traffic

### Understanding Subnet Masks

A subnet mask is a 32-bit number that divides an IP address into network and host portions.

**How it works**:
- Binary `1` bits indicate the network portion
- Binary `0` bits indicate the host portion

**Example**: IP address `192.168.1.100` with subnet mask `255.255.255.0`

```
IP Address:    192.168.1.100    11000000.10101000.00000001.01100100
Subnet Mask:   255.255.255.0    11111111.11111111.11111111.00000000
                                 ^^^^^^^^^^^^^^^^^^^^^^^^^ ^^^^^^^^
                                 Network portion (24 bits) Host (8 bits)

Network Address: 192.168.1.0  (all host bits are 0)
Broadcast Address: 192.168.1.255 (all host bits are 1)
Usable hosts: 192.168.1.1 to 192.168.1.254
```

### Subnet Mask Calculation

- **Network portion**: Identifies the specific network
- **Host portion**: Identifies the specific device on that network

Common subnet masks:
  - `255.255.255.0` (/24) - 256 addresses, 254 usable hosts
  - `255.255.0.0` (/16) - 65,536 addresses, 65,534 usable hosts
  - `255.255.255.128` (/25) - 128 addresses, 126 usable hosts
  - `255.255.255.192` (/26) - 64 addresses, 62 usable hosts

**Why 254 usable and not 256?**
- First address (all 0s in host portion) = Network address
- Last address (all 1s in host portion) = Broadcast address
- Both are reserved and cannot be assigned to hosts

### Real-World Example

**Scenario**: You're setting up AWS VPC

```
VPC CIDR: 10.0.0.0/16
├── Public Subnet: 10.0.1.0/24 (for web servers)
├── Private Subnet: 10.0.2.0/24 (for app servers)
└── Database Subnet: 10.0.3.0/24 (for databases)
```

## CIDR Notation

CIDR (Classless Inter-Domain Routing) was introduced in 1993 to replace the older classful addressing system. It provides a more flexible way to allocate IP addresses.

### Why CIDR Was Needed

**Problem with Classful Addressing**:
- Class C gave only 254 hosts (too small for many organizations)
- Class B gave 65,534 hosts (too large, wasted addresses)
- No middle ground, leading to inefficient address allocation

**CIDR Solution**:
Allows network masks of any length, not just 8, 16, or 24 bits.

### Understanding CIDR Notation

The `/` notation indicates how many bits are used for the network portion.

- `192.168.1.0/24` means first 24 bits are network, last 8 bits are for hosts
- `/24` = 256 addresses (254 usable)
- `/16` = 65,536 addresses (65,534 usable)
- `/32` = single IP address (all 32 bits for network, 0 for host)
- `/0` = all IP addresses (used for default routes)

### CIDR Calculation Examples

**Example 1**: `10.0.0.0/8`
```
Network bits: 8
Host bits: 32 - 8 = 24
Number of addresses: 2^24 = 16,777,216
Subnet mask: 255.0.0.0
Range: 10.0.0.0 to 10.255.255.255
```

**Example 2**: `172.16.0.0/12`
```
Network bits: 12
Host bits: 32 - 12 = 20
Number of addresses: 2^20 = 1,048,576
Subnet mask: 255.240.0.0
Range: 172.16.0.0 to 172.31.255.255
```

**Example 3**: `192.168.1.0/26`
```
Network bits: 26
Host bits: 32 - 26 = 6
Number of addresses: 2^6 = 64
Usable hosts: 64 - 2 = 62
Subnet mask: 255.255.255.192
Range: 192.168.1.0 to 192.168.1.63
```

### Supernetting and Aggregation

CIDR also enables **route aggregation** (supernetting), where multiple networks can be summarized into a single route:

```
Instead of advertising:
- 192.168.1.0/24
- 192.168.2.0/24
- 192.168.3.0/24
- 192.168.4.0/24

Advertise single route:
- 192.168.0.0/22 (covers all four networks)
```

This reduces routing table size and improves routing efficiency

## Useful Commands

### Check your IP address:
```bash
# On Linux/Mac
ip addr show
# or
ifconfig

# On Windows
ipconfig
```

### Check if a host is reachable:
```bash
ping 8.8.8.8
```

## DevOps Real-World Use Cases

1. **Cloud VPC Design**: Creating subnets for different tiers (web, app, database)
2. **Container Networks**: Docker assigns IP addresses from a subnet (usually `172.17.0.0/16`)
3. **Kubernetes Pods**: Each pod gets its own IP address
4. **Security Groups**: Allow traffic from specific IP ranges (e.g., `10.0.1.0/24`)

## Key Takeaways

✅ IP addresses identify devices on a network  
✅ Private IPs are for internal networks  
✅ Public IPs are for internet communication  
✅ Subnetting helps organize and secure networks  
✅ CIDR notation is used everywhere in cloud/DevOps
