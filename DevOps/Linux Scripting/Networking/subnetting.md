# Networking Fundamentals - IPv4, Subnets, and Addressing

## 1. IPv4 Addresses

### Key Concepts

- **IP Address**: Unique network identifier for hosts
- **Structure**: Contains network portion + host portion
- **Format**: Four octets separated by periods
- **Octet**: 8 bits = 1 byte
- **Total**: IPv4 address = 4 bytes = 32 bits

### Examples

- **Basic IPv4**: `204.23.124.23`
- **Sample output from ifconfig**:

  ```txt
  eth0      Link encap:Ethernet  HWaddr 1d:3a:32:24:4d:ce  
            inet addr:192.168.1.129  Bcast:192.168.1.255  Mask:255.255.255.0
  ```

  - IPv4 address: `192.168.1.129`

### Command

- View IP address: `ifconfig -a`

---

## 2. Subnets (Subnetworks)

### Subnet Purpose

- Group hosts with similar IP addresses
- Enable easy data transfer between nearby hosts
- Segment networks and control traffic flow
- Prevent direct interaction between different subnets

### Network Identification Example

- **Host 1**: `123.45.67.8`
- **Host 2**: `123.45.67.9`
- **Network prefix**: `123.45.67` (common part)
- **Host portions**: `8` and `9`
- **Result**: Both hosts on same subnet

### Subnet Notation

- **Format**: `network_prefix/subnet_mask`
- **Example**: `123.234.0.0/255.255.0.0`

### Router Connection

- **Local network**: `192.168.1.129/24`
- **Router address**: `192.168.1.1` (typically address 1 of subnet)
- **Function**: Connects different subnets together

---

## 3. Subnet Masks

### Purpose

- Determine network portion vs. host portion of IP address
- Control subnet size and host capacity

### Common Subnet Mask

- **Example**: `255.255.255.0`
- **Binary equivalent**: `11111111.11111111.11111111.00000000`

### Binary Conversion

- **8 bits all 1s**: `11111111` = 255 in decimal
- **Octet range**: 0 to 255

### Subnet Math Example

**Given**:

- IP: `192.168.1.165`
- Subnet mask: `255.255.255.0`

**Binary representation**:

```txt
192.168.1.165  = 11000000.10101000.00000001.10100101
255.255.255.0  = 11111111.11111111.11111111.00000000
```

**Host calculation**:

- Available host bits: `00000000` (8 bits)
- Possible combinations: 2^8 = 256
- Subtract 2 (subnet + broadcast): 256 - 2 = 254 usable hosts
- **Host range**: `192.168.1.1` to `192.168.1.254`

---

## 4. Binary Conversion Shortcuts

### Base-2 Powers (Memorize)

```txt
2^1 = 2        2^7 = 128
2^2 = 4        2^8 = 256
2^3 = 8        2^9 = 512
2^4 = 16       2^10 = 1024
2^5 = 32       2^11 = 2048
2^6 = 64       2^12 = 4096
```

### Decimal to Binary Chart

```txt
Bit positions: 1   1  1  1  1 1 1 1
Values:       128 64 32 16 8 4 2 1
```

### Conversion Example: 192 to Binary

1. **192 - 128 = 64** → First bit = 1
2. **64 - 64 = 0** → Second bit = 1
3. **Remaining bits = 0**
4. **Result**: `11000000`

### Binary to Decimal Example: 11000000

- Add positions with 1s: **128 + 64 + 0 + 0 + 0 + 0 + 0 + 0 = 192**

---

## 5. CIDR (Classless Inter-Domain Routing)

### CIDR Purpose

- Compact representation of subnet masks
- Combines subnet prefix + subnet mask

### Notation Examples

- **Traditional**: `10.42.3.0/255.255.255.0`
- **CIDR**: `10.42.3.0/24`
- **Meaning**: First 24 bits are network prefix

### Host Calculation Example

**Given**: `123.12.24.0/23`

1. **Total IP bits**: 32
2. **Network bits**: 23
3. **Host bits**: 32 - 23 = 9
4. **Possible hosts**: 2^9 = 512
5. **Usable hosts**: 512 - 2 = 510

---

## 6. NAT (Network Address Translation)

### Function

- Single IP represents entire private network
- Router acts as intermediary with Internet
- Provides privacy and security

### Analogy

- **Router = Receptionist** in large office
- **Private IPs = Extension numbers**
- **Public IP = Main office number**

### Example Process

1. **Patty** wants to connect to `www.google.com`
2. **Router** receives request from Patty
3. **Router** opens its own connection to Google
4. **Router** forwards Patty's request
5. **Google** only sees the router, not Patty

---

## 7. IPv6

### IPv6 Purpose

- Address IPv4 address exhaustion
- Allow more Internet-connected devices
- Complement (not replace) IPv4

### Address Format

- **IPv6 example**: `2dde:1235:1256:3:200:f8ed:fe23:59cf`
- **Difference**: Hexadecimal notation with colons
- **Compatibility**: Can coexist with IPv4

### Adoption

- Currently slow adoption rate
- Designed for future Internet growth

---

## Quick Reference

### Key Commands

- `ifconfig -a` - View network interfaces and IP addresses

### Important Numbers

- **IPv4 address**: 32 bits (4 bytes)
- **Octet range**: 0-255
- **Subnet calculation**: Always subtract 2 from total possible hosts

### Common Subnet Masks

- `/24` or `255.255.255.0` - 254 hosts
- `/16` or `255.255.0.0` - 65,534 hosts
- `/8` or `255.0.0.0` - 16,777,214 hosts
