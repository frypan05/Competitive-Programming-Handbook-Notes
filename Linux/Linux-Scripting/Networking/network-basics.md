# Networking Fundamentals Notes

## 1. Network Basics

### Key Components of a Home Network

- **ISP (Internet Service Provider)**: The company you pay for internet access
- **Router**: Connects all devices to the internet (wireless or Ethernet)
- **WAN (Wide Area Network)**: Network between router and broader internet
- **WLAN (Wireless Local Area Network)**: Network between router and wireless devices
- **LAN (Local Area Network)**: Network between router and wired devices
- **Hosts**: Each machine on a network

### Data Transmission

- Data travels through networks as **packets**
- Packets contain the information being transmitted between hosts

---

## 2. OSI Model

### Overview

- **OSI (Open Systems Interconnection)**: Theoretical networking model
- Shows packet traversal through **7 layers**
- Foundation for the TCP/IP model used today
- More theoretical than practical for modern networking

---

## 3. TCP/IP Model

### Four-Layer Structure

The TCP/IP model is the actual implementation of networking that the Internet uses.

#### Application Layer (Top Layer)

**Purpose**: Interface between computer programs and transport layer
**Protocols**:

- **HTTP (Hypertext Transfer Protocol)**: Used for webpages
- **SMTP (Simple Mail Transfer Protocol)**: Used for email transmission

#### Transport Layer

**Purpose**: Determines how data is transmitted, ensures data integrity
**Protocols**:

- **TCP (Transmission Control Protocol)**: Reliable data delivery
- **UDP (User Datagram Protocol)**: Unreliable but faster data delivery

#### Network Layer

**Purpose**: Moves packets between hosts and across networks
**Protocols**:

- **IP (Internet Protocol)**: Routes packets between machines
- **ICMP (Internet Control Message Protocol)**: Error messages and debugging

#### Link Layer (Bottom Layer)

**Purpose**: Sends data across physical hardware
**Examples**: Ethernet, fiber optic cables

---

## 4. Network Addressing

### Three Types of Network Identifiers

#### MAC Addresses

- **Purpose**: Unique hardware identifier that never changes
- **Format**: 00:C4:B5:45:B2:43
- **Example**: Dell devices have OUI 00-14-22, so a Dell adapter might be 00-14-22-34-B2-C2
- **Location**: Network interface card (NIC)

#### IP Addresses

- **Purpose**: Software-based device identification on networks
- **Format**: 10.24.12.4 (IPv4 example)
- **Characteristics**: Can change, unique across the Internet
- **Example**: 192.12.41.4

#### Hostnames

- **Purpose**: Human-readable names for IP addresses
- **Example**: Instead of remembering 192.12.41.4, use myhost.com

---

## 5. Application Layer Deep Dive

### Email Example: Sending to Patty

**Process**:

1. Email client uses SMTP protocol
2. Data sent through **port 25** (SMTP port)
3. Application layer encapsulates data
4. Data passed to Transport layer for segmentation

### Packet Structure

- **Header**: Contains routing information (where packet goes/comes from)
- **Payload**: Actual data being transferred
- **Terminology**: Called "packet" generally, "segment" in transport layer, "frame" in link layer

---

## 6. Transport Layer Deep Dive

### Segmentation

- Breaks data into **segments** for easier transport
- Segments reassembled in correct order at destination

### Ports

- **Purpose**: Direct data to specific processes/services
- **Example**: HTTP uses port 80, SMTP uses port 25
- Transport layer attaches source and destination ports

### UDP vs TCP

#### UDP (User Datagram Protocol)

- **Characteristics**: Unreliable, faster
- **Use Case**: Media streaming (okay to lose some frames for speed)

#### TCP (Transmission Control Protocol)

- **Characteristics**: Reliable, connection-oriented
- **TCP Handshake Process**:
  1. Client sends **SYN** segment to server
  2. Server sends **SYN-ACK** to acknowledge
  3. Client sends **ACK** to complete connection
- **Features**: Uses sequence numbers to ensure correct order

### Email Example Continued

Transport layer attaches destination port (25) and source port for the email data.

---

## 7. Network Layer Deep Dive

### Routing Function

- Determines packet routing from source to destination
- Internet consists of many interconnected **subnets**
- **Example**: Enables access to <www.google.com> even though it's on a different network

### IP Packet Creation

- Encapsulates transport layer segment in IP packet
- Attaches source and destination IP addresses
- Packet now knows where it's going and where it came from

---

## 8. Link Layer Deep Dive

### Frame Creation

- Encapsulates packet into a **frame**
- Adds source and destination MAC addresses
- Includes checksums and packet separators

### ARP (Address Resolution Protocol)

**Purpose**: Finds MAC address associated with an IP address
**Process**:

1. Check ARP lookup table first
2. If not found, send broadcast message to network
3. Host with requested IP responds with MAC address
**Example**: "Which host has IP 10.10.1.4?" - broadcast to all network hosts

---

## 9. Complete Packet Traversal Example

### Pete Sends Email to Patty

#### Outbound Journey (Pete's side)

1. **Application**: Email data sent to transport layer
2. **Transport**: Data encapsulated in TCP segment with ports
3. **Network**: Segment encapsulated in IP packet with IP addresses
4. **Link**: Packet encapsulated in frame with MAC addresses

#### Inbound Journey (Patty's side)

1. **Link**: Frame received, integrity checked, de-encapsulated
2. **Network**: IP packet examined, destination IP verified, de-encapsulated
3. **Transport**: Segment de-encapsulated, port numbers checked
4. **Application**: Final email message presented to Patty

---

## 10. DHCP (Dynamic Host Configuration Protocol)

### Purpose

Automatically assigns network configuration to devices:

- IP addresses
- Subnet masks
- Gateways

### Analogy

Like a phone carrier assigning phone numbers - you get a "lease" on an IP address for a specific time period.

### DHCP Process (4-Step Handshake)

1. **DHCP DISCOVER**: Client broadcasts search for DHCP server
2. **DHCP OFFER**: Server responds with IP address, subnet mask, lease time
3. **DHCP REQUEST**: Client broadcasts acceptance of specific offer
4. **DHCP ACK**: Server sends acknowledgment

### Typical Setup

- **Home networks**: Router usually acts as DHCP server
- **Benefits**: Prevents duplicate IP addresses, reduces admin workload

---

## Key Takeaways

1. **Layered Approach**: Each layer adds specific information to packets
2. **Dual Addressing**: Both MAC (hardware) and IP (software) addresses needed
3. **Packet Journey**: Data travels through all layers twice (send and receive)
4. **Automatic Configuration**: DHCP handles network setup automatically
5. **Protocol Cooperation**: Multiple protocols work together at each layer
