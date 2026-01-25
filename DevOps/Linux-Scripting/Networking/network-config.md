# Linux Networking Commands - Study Notes

## 1. Network Interfaces

### Overview

- Network interfaces link the software side of networking to the hardware side
- Common interface names:
  - `eth0` - First Ethernet card
  - `wlan0` - Wireless interface
  - `lo` - Loopback interface (represents your computer, loops back to yourself)

### The ifconfig Command

**View all interfaces:**

```bash
ifconfig -a
```

**Example output:**

```txt
eth0      Link encap:Ethernet  HWaddr 1d:3a:32:24:4d:ce  
          inet addr:192.168.1.129  Bcast:192.168.1.255  Mask:255.255.255.0
          inet6 addr: fd60::21c:29ff:fe63:5cdc/64 Scope:Link
```

**Key information in output:**

- **HWaddr** - MAC address of the interface
- **inet addr** - IPv4 address
- **inet6** - IPv6 address
- **Mask** - Subnet mask
- **Bcast** - Broadcast address

**Create and configure an interface:**

```bash
ifconfig eth0 192.168.2.1 netmask 255.255.255.0 up
```

**Bring interfaces up/down:**

```bash
ifup eth0
ifdown eth0
```

### The ip Command

**Show interface information:**

```bash
ip link show
```

**Show interface statistics:**

```bash
ip -s link show eth0
```

**Show IP addresses:**

```bash
ip address show
```

**Bring interfaces up/down:**

```bash
ip link set eth0 up
ip link set eth0 down
```

**Add IP address to interface:**

```bash
ip address add 192.168.1.1/24 dev eth0
```

## 2. Routing (route command)

### Managing Routes with route Command

**Add a new route:**

```bash
sudo route add -net 192.168.2.1/23 gw 10.11.12.3
```

**Delete a route:**

```bash
sudo route del -net 192.168.2.1/23
```

### Managing Routes with ip Command

**Add a route:**

```bash
ip route add 192.168.2.1/23 via 10.11.12.3
```

**Delete a route:**

```bash
ip route delete 192.168.2.1/23 via 10.11.12.3
```

or

```bash
ip route delete 192.168.2.1/23
```

## 3. DHCP Client (dhclient)

### DHCP Client Overview

- dhclient starts on boot and configures network interfaces using DHCP
- Gets network interface list from `dhclient.conf`
- Tracks leases in `dhclient.leases` file across reboots

**Obtain fresh IP address:**

```bash
sudo dhclient
```

## 4. Network Manager

### Network Manager Overview

- NetworkManager daemon automatically configures networks
- Appears as GUI applet on desktop taskbar
- Gathers hardware info on startup and activates connections

### nm-tool Command

**Check NetworkManager state:**

```bash
nm-tool
```

**Example output:**

```txt
NetworkManager Tool

State: connected (global)

- Device: eth0  [Wired connection 1] -------------------------------------------
  Type:              Wired
  Driver:            pcnet32
  State:             connected
  Default:           yes
  HW Address:        12:3D:45:56:7D:CC

  Capabilities:
    Carrier Detect:  yes

  Wired Properties
    Carrier:         on

  IPv4 Settings:
    Address:         192.168.22.1
    Prefix:          24 (255.255.255.0)
    Gateway:         192.168.22.2
    DNS:             192.168.22.2
```

### nmcli Command

- Command-line tool to control and modify NetworkManager
- Refer to manpage for detailed usage

## 5. ARP (Address Resolution Protocol)

### ARP Overview

- ARP cache stores IP-to-MAC address mappings
- Cache is empty on boot, populated as packets are sent

### ARP Process

1. Source host creates Ethernet frame with ARP request
2. Source host broadcasts frame to entire network
3. Host with correct MAC address sends reply packet
4. Source host adds IP-to-MAC mapping to ARP cache
5. Packet transmission proceeds

### View ARP Cache

**Using arp command:**

```bash
arp
```

**Example output:**

```txt
Address                  HWtype  HWaddress           Flags Mask            Iface
192.168.22.1            ether   00:12:24:fc:12:cc   C                     eth0
192.168.22.254          ether   00:12:45:f2:84:64   C                     eth0
```

**Using ip command:**

```bash
ip neighbour show
```

## Key Configuration Files

- `/etc/network/interfaces` - Interface configuration
- `dhclient.conf` - DHCP client configuration
- `dhclient.leases` - DHCP lease tracking

## Quick Reference Summary

| Task | ifconfig/route | ip command |
|------|----------------|------------|
| Show interfaces | `ifconfig -a` | `ip link show` |
| Show IP addresses | `ifconfig` | `ip address show` |
| Bring interface up | `ifup eth0` | `ip link set eth0 up` |
| Add IP to interface | `ifconfig eth0 192.168.1.1/24` | `ip address add 192.168.1.1/24 dev eth0` |
| Add route | `route add -net 192.168.2.1/23 gw 10.11.12.3` | `ip route add 192.168.2.1/23 via 10.11.12.3` |
| Delete route | `route del -net 192.168.2.1/23` | `ip route delete 192.168.2.1/23` |
