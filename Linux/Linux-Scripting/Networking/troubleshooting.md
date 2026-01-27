# Network Troubleshooting Tools - Study Notes

## 1. ICMP (Internet Control Message Protocol)

### Overview

- Part of the TCP/IP protocol suite
- Used to send updates and error messages
- Extremely useful for debugging network issues like failed packet delivery

### ICMP Message Structure

Each ICMP message contains:

- **Type field**: Type of ICMP message
- **Code field**: Sub-type providing more information about the message
- **Checksum field**: Used to detect message integrity issues

### Common ICMP Types

- **Type 0**: Echo Reply
- **Type 3**: Destination Unreachable
- **Type 8**: Echo Request
- **Type 11**: Time Exceeded

### Type 3 (Destination Unreachable) Codes

When a packet can't reach its destination, Type 3 ICMP message is generated with 16 possible code values:

- **Code 0**: Network Unreachable
- **Code 1**: Host Unreachable
- *etc.*

---

## 2. ping Command

### Purpose of ping

- Simplest networking tool for testing packet delivery
- Tests whether a packet can reach a host
- Works by sending ICMP echo request (Type 8) packets and waiting for ICMP echo reply (Type 0)

### Example Usage (netstat)

```bash
pete@icebox:~$ ping -c 3 www.google.com

PING www.google.com (74.125.239.112) 56(84) bytes of data.
64 bytes from nuq05s01-in-f16.1e100.net (74.125.239.112): icmp_seq=1 ttl=128 time=29.0 ms
64 bytes from nuq05s01-in-f16.1e100.net (74.125.239.112): icmp_seq=2 ttl=128 time=23.7 ms
64 bytes from nuq05s01-in-f16.1e100.net (74.125.239.112): icmp_seq=3 ttl=128 time=15.1 ms
```

### Key Fields Explained

#### icmp_seq (Sequence Number)

- Shows sequence number of packets sent
- Missing sequence numbers indicate connectivity issues
- Out-of-order sequences suggest slow connections (packets exceeding 1-second default)

#### ttl (Time To Live)

- Used as a hop counter
- Decrements by one at each hop
- Packet dies when TTL reaches 0
- Gives packets a lifespan to prevent infinite routing

#### time

- Roundtrip time from sending echo request to receiving echo reply
- Measured in milliseconds

---

## 3. traceroute Command

### Purpose

- Shows how packets are getting routed to their destination
- Builds a list of routers that packets traverse

### How It Works

1. Sends packets with increasing TTL values, starting with 1
2. First router decrements TTL by 1, dropping the packet (TTL=0)
3. Router sends back ICMP Time Exceeded message
4. Next packet has TTL=2, passes first router but gets dropped at second
5. Process continues until destination is reached and ICMP Echo Reply is received

### Example Output

```bash
$ traceroute google.com                                                                          
traceroute to google.com (216.58.216.174), 30 hops max, 60 byte packets                          
 1  192.168.4.254 (192.168.4.254)  0.028 ms  0.009 ms  0.008 ms                                  
 2  100.64.1.113 (100.64.1.113)  1.227 ms  1.226 ms 0.920 ms
 3  100.64.0.20 (100.64.0.20)  1.501 ms 1.556 ms  0.855 ms
```

### Output Interpretation

- Each line represents a router/machine between source and destination
- Shows target name, IP address, and three round-trip times
- Default: sends three packets along each route

---

## 4. netstat Command

### Well-Known Ports

Common ports found in `/etc/services`:

```txt
ftp             21/tcp
ssh             22/tcp
smtp            25/tcp 
domain          53/tcp  # DNS
http            80/tcp
https           443/tcp
```

### Key Concepts

- **Socket**: Interface allowing programs to send/receive data
- **Port**: Identifies which application should send/receive data
- **Socket Address**: Combination of IP address and port
- Each connection requires a unique socket

### Example Usage

```bash
pete@icebox:~$ netstat -at

Active Internet connections (servers and established)
Proto Recv-Q Send-Q Local Address           Foreign Address         State      
tcp        0      0 icebox:domain           *:*                     LISTEN     
tcp        0      0 localhost:ipp           *:*                     LISTEN     
tcp        0      0 icebox.lan:44468        124.28.28.50:http       TIME_WAIT  
tcp        0      0 icebox.lan:34751        124.28.29.50:http       TIME_WAIT  
tcp        0      0 icebox.lan:34604        economy.canonical.:http TIME_WAIT  
tcp6       0      0 ip6-localhost:ipp       [::]:*                  LISTEN     
tcp6       1      0 ip6-localhost:35094     ip6-localhost:ipp       CLOSE_WAIT 
tcp6       0      0 ip6-localhost:ipp       ip6-localhost:35094     FIN_WAIT2
```

### Column Descriptions

- **Proto**: Protocol used (TCP or UDP)
- **Recv-Q**: Data queued to be received
- **Send-Q**: Data queued to be sent
- **Local Address**: Locally connected host
- **Foreign Address**: Remotely connected host
- **State**: Socket state

### Common Socket States

- **LISTENING**: Socket listening for incoming connections
- **SYN_SENT**: Socket actively attempting to establish connection
- **ESTABLISHED**: Socket has established connection
- **CLOSE_WAIT**: Remote host shutdown, waiting for socket to close
- **TIME_WAIT**: Socket waiting after close to handle remaining network packets

---

## 5. Packet Analysis with tcpdump

### Packet Analyzer Overview

- Popular packet analyzer (along with Wireshark)
- Scans network interfaces, captures packet activity
- Parses packages and outputs information for analysis
- Provides low-level network analysis capabilities

### Installation

```bash
sudo apt install tcpdump
```

### Basic Usage

```bash
pete@icebox:~$ sudo tcpdump -i wlan0

tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
listening on wlan0, link-type EN10MB (Ethernet), capture size 65535 bytes

11:28:23.958840 IP icebox.lan > nuq04s29-in-f4.1e100.net: ICMP echo request, id 1901, seq 2, length 64
11:28:23.970928 IP nuq04s29-in-f4.1e100.net > icebox.lan: ICMP echo reply, id 1901, seq 2, length 64
11:28:24.960464 IP icebox.lan > nuq04s29-in-f4.1e100.net: ICMP echo request, id 1901, seq 3, length 64
11:28:24.979299 IP nuq04s29-in-f4.1e100.net > icebox.lan: ICMP echo reply, id 1901, seq 3, length 64
11:28:25.961869 IP icebox.lan > nuq04s29-in-f4.1e100.net: ICMP echo request, id 1901, seq 4, length 64
11:28:25.976176 IP nuq04s29-in-f4.1e100.net > icebox.lan: ICMP echo reply, id 1901, seq 4, length 64
11:28:26.963667 IP icebox.lan > nuq04s29-in-f4.1e100.net: ICMP echo request, id 1901, seq 5, length 64
11:28:26.976137 IP nuq04s29-in-f4.1e100.net > icebox.lan: ICMP echo reply, id 1901, seq 5, length 64
11:28:30.674953 ARP, Request who-has 172.254.1.0 tell ThePickleParty.lan, length 28
11:28:31.190665 IP ThePickleParty.lan.51056 > 192.168.86.255.rfe: UDP, length 306
```

### Understanding tcpdump Output

Example line breakdown:

```txt
11:28:23.958840 IP icebox.lan > nuq04s29-in-f4.1e100.net: ICMP echo request, id 1901, seq 2, length 64
```

- **11:28:23.958840**: Timestamp of network activity
- **IP**: Protocol information
- **icebox.lan > nuq04s29-in-f4.1e100.net**: Source and destination addresses
- **seq**: TCP packet's starting and ending sequence number
- **length**: Length in bytes

### Saving Output to File

```bash
sudo tcpdump -w /some/file
```

### Additional Notes

- Different packet types output different information
- This covers only basic packet analysis
- Advanced analysis includes Hex and ASCII output
- Many online resources available for deeper learning
- Consider Wireshark for more advanced packet analysis needs
