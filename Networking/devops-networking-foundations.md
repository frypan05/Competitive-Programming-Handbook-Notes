# DevOps Networking Foundations

> Aligning networking fundamentals with modern DevOps & cloud workflows.

## How to Use This Guide
- Treat this as a living reference: skim the overview sections, then dive deeper as projects demand.
- Each section includes action items, Linux/Cloud commands, and interview callouts.
- Pair this with labs (cloud free tiers, kind/Minikube, Docker, ngrok) to convert theory into muscle memory.

## Table of Contents
1. [Networking Mindset for DevOps](#networking-mindset-for-devops)
2. [Layered Models (OSI & TCP/IP)](#layered-models-osi--tcpip)
3. [IP Addressing, Subnetting, and CIDR](#ip-addressing-subnetting-and-cidr)
4. [Routing, NAT, and Firewalls](#routing-nat-and-firewalls)
5. [Ports, Protocols, and Traffic Flows](#ports-protocols-and-traffic-flows)
6. [Essential Linux & Container Networking Commands](#essential-linux--container-networking-commands)
7. [DNS, TLS, and Identity](#dns-tls-and-identity)
8. [Reverse Proxies, Ingress, and Edge Services](#reverse-proxies-ingress-and-edge-services)
9. [Cloud Networking Patterns](#cloud-networking-patterns)
10. [Container & Docker Networking](#container--docker-networking)
11. [Kubernetes Networking Deep Dive](#kubernetes-networking-deep-dive)
12. [Networking in CI/CD and DevOps Practices](#networking-in-cicd-and-devops-practices)
13. [Observability, Troubleshooting, and Incident Playbooks](#observability-troubleshooting-and-incident-playbooks)
14. [Security, Zero Trust, and Compliance Considerations](#security-zero-trust-and-compliance-considerations)
15. [Practical Labs & Interview Drills](#practical-labs--interview-drills)

---

## Networking Mindset for DevOps
- **Automation first**: Treat network constructs (VPCs, subnets, firewall rules, load balancers) as code using Terraform, Pulumi, or CloudFormation.
- **Observability baked in**: Export flow logs, capture metrics (latency, throughput), and wire logs (Nginx, Envoy) into Grafana/Loki/ELK from day one.
- **Security shared responsibility**: Networking decisions (private IP ranges, NAT gateways, ingress rules) define the blast radius.
- **Reproducibility**: Document every port, CIDR, and dependency as part of runbooks and README.md to reduce “tribal knowledge.”
- **Feedback loops**: Integrate network tests (smoke tests with `curl`, synthetic DNS queries, TLS expiry checks) into CI/CD.

### DevOps Networking Role Alignment
| Responsibility | Networking Tie-In | Tools/Concepts |
| --- | --- | --- |
| CI/CD | Artifact downloads, registry mirrors, caching proxies, firewall egress allowlists | Nexus, Artifactory, Squid, VPC endpoints |
| Infrastructure Automation | VPC design, subnet tiering, routing tables as code | Terraform `aws_vpc`, `azurerm_virtual_network`, Ansible `ec2_vpc_net` |
| Observability | Flow logs, packet captures, SLO alerts | VPC Flow Logs, CloudWatch, Datadog NPM, Wireshark, eBPF |
| Security | Zero trust, network segmentation, secrets delivery | Security Groups, NACLs, HashiCorp Vault, SPIFFE/SPIRE |
| Developer Experience | Tunnels, port-forwarding, remote dev environments | ngrok, Tailscale, Kubernetes `kubectl port-forward` |

## Layered Models (OSI & TCP/IP)
- **Memorize both models**; interviewers expect you to map protocols to layers quickly.
- OSI Layers: 7 Application, 6 Presentation, 5 Session, 4 Transport, 3 Network, 2 Data Link, 1 Physical.
- TCP/IP Stack: 4 Application, 3 Transport, 2 Internet, 1 Network Access.
- Key mapping cheat sheet:
| OSI Layer | TCP/IP Equivalent | DevOps Examples |
| --- | --- | --- |
| 7 Application | 4 Application | HTTP(S), gRPC, DNS, MQTT, Git, Docker Registry API |
| 6 Presentation | 4 Application | TLS termination, serialization (JSON, Protobuf), compression |
| 5 Session | 4 Application | mTLS sessions, WebSocket lifetimes |
| 4 Transport | 3 Transport | TCP, UDP, QUIC, SCTP (K8s control plane) |
| 3 Network | 2 Internet | IPv4/IPv6, ICMP, routing, pod CIDRs |
| 2 Data Link | 1 Network Access | Ethernet, VLANs, VXLAN overlays, ENIs |
| 1 Physical | 1 Network Access | NICs, Wi-Fi, fiber, AWS Nitro cards |
- **Interview hook**: “Explain how a Git clone travels across the layers” – be ready to narrate DNS -> TCP handshake -> TLS -> HTTP -> application.

## IP Addressing, Subnetting, and CIDR
### Private vs Public Ranges (RFC1918 + RFC6598)
| Range | CIDR | Size | Typical Use |
| --- | --- | --- | --- |
| 10.0.0.0 – 10.255.255.255 | 10.0.0.0/8 | ~16.7M hosts | Large enterprises, AWS VPC defaults |
| 172.16.0.0 – 172.31.255.255 | 172.16.0.0/12 | ~1M hosts | Mid-sized networks, AKS defaults |
| 192.168.0.0 – 192.168.255.255 | 192.168.0.0/16 | 65,536 hosts | Home/SMB, lab clusters |
| 100.64.0.0 – 100.127.255.255 | 100.64.0.0/10 | 4M hosts | Carrier Grade NAT (CGNAT), some cloud services |
- **Golden rule**: Never overlap CIDRs between on-prem, cloud, and partner networks; peering/VPNs break when ranges conflict.

### CIDR Quick Math
- `/n` indicates how many bits are fixed; remaining bits define host addresses.
- Cheats:
  - /32 = single host; /31 = point-to-point; /30 = 4 IPs (2 usable).
  - /24 = 256 IPs (254 usable); /20 ≈ 4,096 IPs; /16 = 65,536.
  - Hosts = 2^(32-n) - 2 (network + broadcast).
- Practice mental conversions and be ready to plan subnets for prod/stage/dev.

### IPv6 Essentials
- 128-bit addresses, hexadecimal, colon-separated (e.g., `2001:db8::/32`).
- No broadcasts; uses multicast and anycast.
- **Cloud reality**: Dual-stack Kubernetes, load balancer support, IPv6-only subnets in AWS (`ipv6_cidr_block`).
- Be familiar with SLAAC, DHCPv6, and NAT64/464XLAT for legacy IPv4 integration.

## Routing, NAT, and Firewalls
- **Routing tables** decide next hop based on longest prefix match.
- **Static vs dynamic**: Cloud VPCs mainly static; on-prem may use BGP/OSPF. Know when to advertise routes to Direct Connect/ExpressRoute.
- **NAT types**:
  - SNAT (Source NAT): Private -> public egress; AWS NAT Gateway, Kubernetes `spec.externalTrafficPolicy`.
  - DNAT (Destination NAT): Port forwarding, ingress controllers.
  - PAT (Port Address Translation): Home routers, AWS internet gateway.
- **Firewalls**: Layer 3/4 (Security Groups, Azure NSGs) vs Layer 7 (AWS WAF, Cloudflare).
- **Network ACL vs Security Group**: stateless vs stateful; default deny vs default allow mental model.
- **Bastion Hosts / Session Managers** reduce open SSH ports; prefer SSM Session Manager or Teleport.

## Ports, Protocols, and Traffic Flows
- TCP vs UDP vs QUIC: reliability vs latency. QUIC (HTTP/3) uses UDP; know MTU impact.
- **Well-known ports** (0-1023): 22 SSH, 53 DNS, 80 HTTP, 443 HTTPS, 3306 MySQL, 5432 Postgres, 6379 Redis, 6443 Kubernetes API, 10250 kubelet.
- **Ephemeral ports**: OS-specific ranges; important for firewall rules.
- **Svc to svc flows**: Document port matrices for microservices; codify as NetworkPolicies or service mesh intentions.
- **ngrok/Cloudflared tunnels**: Map ephemeral port to public endpoint without opening inbound firewall; useful for demos, GitHub webhooks.
- **Nginx/Envoy/HAProxy**: Understand listener->upstream mapping, keepalives, health checks, sticky sessions.

## Essential Linux & Container Networking Commands
| Goal | Command Patterns | Notes |
| --- | --- | --- |
| Interface inspection | `ip addr`, `ip link`, `nmcli device status` | Replace deprecated `ifconfig`; scriptable outputs. |
| Routing | `ip route`, `route -n` | Pair with `ip rule` for policy routing. |
| Connection tracking | `ss -tulpn`, `netstat -plnt` | `ss` faster; filter by state (ESTAB, LISTEN). |
| DNS checks | `dig`, `nslookup`, `resolvectl query` | Use `+trace` to follow delegations. |
| Packet capture | `tcpdump -i eth0 port 443`, `ngrep -d any 'Host:'` | Combine with Wireshark for GUI analysis. |
| Firewall | `nft list ruleset`, `iptables -S`, `ufw status` | Understand nftables priority in modern distros. |
| Throughput/latency | `iperf3`, `mtr`, `ping -f`, `ss -i` | Baseline before/after deployments. |
| Container namespaces | `nsenter -t <pid> -n`, `ip netns list` | Inspect Docker/K8s pod networking internals. |
| Service reachability | `curl -v`, `wget --spider`, `nc -vz host port` | Embed in readiness probes and CI smoke tests. |

## DNS, TLS, and Identity
- **DNS building blocks**: records (A, AAAA, CNAME, TXT, MX, SRV), zones, resolvers, TTLs.
- **Authoritative vs recursive**: Understand how split-horizon DNS serves different answers inside VPC vs public.
- **Private DNS**: AWS Route53 Private Hosted Zones, Azure Private DNS, Cloudflare for Teams.
- **Service discovery**: Consul, etcd, Eureka, Kubernetes CoreDNS (`svc.namespace.svc.cluster.local`).
- **DNS in CI/CD**: Automate record lifecycle with IaC to avoid orphaned entries.

### TLS/M-TLS Quick Reference
- TLS handshake: ClientHello -> ServerHello -> certificate exchange -> key exchange -> symmetric session.
- Certificates: X.509, CN/SAN, certificate chains, OCSP stapling.
- Automation: Let’s Encrypt/ACME, cert-manager on Kubernetes, AWS ACM.
- Mutual TLS: both client and server present certs; used by service meshes (Istio, Linkerd) and API gateways.
- **Health**: Monitor expiry, weak ciphers; automate renewal alerts in Prometheus or Opsgenie.

## Reverse Proxies, Ingress, and Edge Services
- **Nginx / Envoy / HAProxy**: Terminate TLS, implement path-based routing, rewrite headers, rate limit.
- **Ingress controllers**: Kubernetes objects that map host/path to services; integrate with cloud LBs or bare metal (`ingress-nginx`, `AWS Load Balancer Controller`, `Traefik`).
- **API gateways**: Kong, Tyk, AWS API Gateway, Apigee—add auth, quotas, transformations.
- **Tunnels**: ngrok, Cloudflare Tunnels, Tailscale Funnel—securely expose local services; enforce auth before traffic reaches dev clusters.
- **Content delivery**: CDNs (CloudFront, Cloudflare, Akamai) cache at edge; know how cache invalidation interacts with DNS TTLs.
- **WebSockets/HTTP2**: Ensure reverse proxies support protocol upgrades; configure sticky sessions for stateful websockets.

## Cloud Networking Patterns
### Core Constructs
| Cloud | Core Objects | Notes |
| --- | --- | --- |
| AWS | VPC, Subnet (public/private), Route Table, IGW, NAT GW, NACL, Security Group, ENI | Use separate subnets per AZ; enable VPC Flow Logs. |
| Azure | VNet, Subnet, UDR, NSG, Azure Firewall, Private Endpoint | Remember service endpoints vs private endpoints difference. |
| GCP | VPC (global), Subnet (regional), Cloud Router, Firewall rules, Cloud NAT | Hierarchical firewall priority; default allow egress. |
- **Design tips**:
  - Isolate workloads: frontend, app, data subnets.
  - Use PrivateLink/Private Service Connect for SaaS integrations.
  - Prefer managed VPN/peering over exposing services to public internet.
  - Document IPAM strategy; reserve future ranges.

### Hybrid Connectivity
- **Site-to-Site VPN**: IPsec tunnels; know IKE versions, BGP route exchange.
- **Direct Connect/ExpressRoute/Cloud Interconnect**: Dedicated circuits for predictable latency; plan redundant links + LOA/CFA lead time.
- **Transit Gateways / Hub-Spoke**: Centralize routing and inspection (firewalls, IDS).
- **Service Insertion**: Send traffic through inspection appliances (Palo Alto, Check Point) via VPC routing.

### Cloud Load Balancers
- L4 vs L7 (NLB vs ALB, TCP vs HTTP).
- Cross-zone load balancing, sticky sessions, health checks, connection draining.
- mTLS termination, Web Application Firewall (AWS WAF, Azure WAF).
- Autoscaling interplay: ensure target groups deregister instances gracefully.

## Container & Docker Networking
### Network Drivers
| Driver | Behavior | Use Case |
| --- | --- | --- |
| bridge | Default; NATed network on host; containers get private IPs (`172.17.0.0/16`). | Local dev, docker-compose. |
| host | Shares host network namespace (no isolation). | High-performance, needs port conflict awareness. |
| overlay | Multi-host networking over VXLAN; uses key-value store (Swarm) or orchestrator (K8s). | Docker Swarm, legacy microservices. |
| macvlan/ipvlan | Containers appear as physical devices on LAN. | Legacy appliances, network sniffers. |
| none | Manual network config. | Security hardening, custom stacks. |
- Understand port publishing: `-p 8080:80` (host:container).
- Inspect with `docker network inspect`, `docker container port`, `iptables -t nat -L`. 
- DNS inside containers provided by Docker embedded DNS (`127.0.0.11`).

### Docker Compose Considerations
- Declare explicit networks to control service isolation.
- Use `depends_on` + healthchecks to ensure upstream ready.
- Externalize certificates, CA bundles, and proxies via volumes/env.
- For local cloud emulation, pair Compose with tools like LocalStack/MinIO + ngrok for webhook testing.

## Kubernetes Networking Deep Dive
### Key Components
- **Pod networking (CNI)**: Flannel (VXLAN), Calico (BGP/ebpf), Cilium (eBPF), Amazon VPC CNI (native ENIs). Understand pod CIDRs and host routing.
- **Service types**:
  - `ClusterIP`: internal-only virtual IP via kube-proxy (iptables, IPVS, eBPF).
  - `NodePort`: exposes service on each node port (30000-32767 by default).
  - `LoadBalancer`: integrates with cloud LB.
  - `ExternalName`: DNS alias.
- **Endpoints & EndpointSlices**: Track pod IPs; watch for `EndpointsOverflow` warnings.
- **Ingress**: HTTP(S) routing with controllers; tie to cert-manager for TLS.
- **Network Policies**: Default deny -> allow minimal egress/ingress; integrate with CNI that enforces them.
- **Service Mesh**: Istio, Linkerd, Consul Connect. Adds mTLS, retries, traffic splitting; introduces sidecars and policy CRDs.
- **DNS in cluster**: CoreDNS config maps; stub domains; NodeLocal DNSCache.

### Troubleshooting Playbook
1. `kubectl get pods -o wide` -> confirm pod IP & node.
2. `kubectl exec` with `apt install -y iputils-ping curl` for connectivity tests.
3. Inspect iptables/ipvs rules on node: `sudo iptables-save | grep <svc-cluster-ip>`.
4. Check CNI daemonset logs (`kubectl logs -n kube-system calico-node-...`).
5. Validate kube-proxy mode: `kubectl -n kube-system get configmap kube-proxy -o yaml`.
6. Use `kubectl port-forward` for emergency access; prefer ingress/Service when stable.
7. Monitor `kubelet` and `containerd` logs for CNI ADD/DEL errors (common root cause: IP exhaustion).

## Networking in CI/CD and DevOps Practices
- **Artifact & dependency mirrors**: Cache registries inside VPC to reduce egress and failure domain.
- **Pipeline network policies**: Restrict runners to specific egress CIDRs; rotate credentials via short-lived tokens.
- **Secrets delivery**: Avoid passing secrets via env vars; use Vault Agents, AWS Secrets Manager via IMDSv2, Kubernetes Secrets encrypted at rest with KMS.
- **Supply chain security**: Sign images (Cosign), enforce admission policies on image sources/domains.
- **GitOps**: Network policies around Argo CD/Flux controllers; ensure only read-only outbound to Git provider.
- **Remote runners**: Provide NAT gateways or HTTP proxies for GitHub Actions self-hosted runners in private subnets.
- **Port management**: Standardize service port registry; enforce via CI linting (YAML schema).

## Observability, Troubleshooting, and Incident Playbooks
- **Metrics**: Latency (p50/p95/p99), throughput (req/s), saturation (conn count), errors (4xx/5xx). Publish via Prometheus/CloudWatch.
- **Logs**: Access logs (Nginx, Envoy), flow logs, firewall denies, DNS query logs.
- **Traces**: OpenTelemetry to track cross-service latency; tag with network metadata (client IP, AZ).
- **Packet captures on demand**: AWS VPC Traffic Mirroring, Azure vTap, GCP Packet Mirroring.
- **Chaos testing**: Inject DNS failures, packet loss (tc/netem), expired certs to validate resilience.
- **Runbooks**: Document “site slow”, “TLS expired”, “pod can’t reach DB” with decision trees and rollback steps.
- **Post-incident reviews**: Capture missing metrics, propose automation (e.g., auto quarantine compromised ENI).

## Security, Zero Trust, and Compliance Considerations
- **Principle of least privilege** at network layer: start with deny-all, open only required ports.
- **Microsegmentation**: Use Kubernetes NetworkPolicies, Calico GlobalNetworkPolicy, cloud security groups per tier.
- **Zero Trust**: Identity-aware proxies (Google IAP, Cloudflare Access), mutual TLS, device posture checks.
- **Encryption in transit**: TLS everywhere, IPsec between regions, service mesh sidecars for east-west.
- **Secrets over network**: Use short TTL certificates, auto-rotating API keys, hardware security modules (CloudHSM/KMS).
- **Compliance**: PCI DSS requires network segmentation; HIPAA mandates audit logs. Map controls to infrastructure-as-code modules.
- **Vulnerability management**: Regularly scan ingress/egress points, keep proxies/load balancers patched.

## Practical Labs & Interview Drills
1. **Home Lab VPC**: Use Terraform to build AWS VPC with 3 subnets, NAT GW, bastion. Document CIDRs and routing.
2. **Docker Networking Explorer**: Launch containers on bridge, host, overlay networks; inspect iptables rules.
3. **Kubernetes Traffic Flow**: Deploy `nginx` + `curl` pod; experiment with NetworkPolicies and Ingress.
4. **ngrok/Webhook Workflow**: Expose local GitHub webhook listener; secure with authtoken + ACL; capture request via `ngrok http 3000`.
5. **DNS Trace**: Use `dig +trace` for custom domain; add Route53 record via Terraform; verify propagation.
6. **Observability Drill**: Generate load with `hey` or `wrk`; visualize latency in Grafana; capture tcpdump to correlate.
7. **Incident Role-Play**: Simulate “503 due to exhausted ephemeral ports” and walk through investigation steps.

### Interview Rapid-Fire Topics
- Explain difference between Security Group and NACL.
- Design a multi-region active-active architecture with DNS-based failover.
- Troubleshoot: “Pod can’t reach RDS over TLS” – walk through VPC peering, SG rules, cert auth.
- Describe how service mesh implements mTLS without app changes.
- Compare ngrok vs self-hosted Nginx reverse proxy for demos.
- Explain why overlapping CIDRs break VPC peering and how to fix.
- Detail steps to rotate TLS certificates in Kubernetes ingress (cert-manager).

## Quick Reference Appendix
### Common Port Matrix
| Service | Default Port(s) | Notes |
| --- | --- | --- |
| SSH | 22/tcp | Automate bastions or SSM to avoid 0.0.0.0/0. |
| HTTP | 80/tcp | Redirect to HTTPS; use HSTS. |
| HTTPS | 443/tcp | Terminate at LB or ingress; enforce TLS1.2+. |
| DNS | 53/udp+tcp | TCP used for zone transfers, large responses. |
| NTP | 123/udp | Unsynchronized clocks break TLS and logs. |
| MySQL | 3306/tcp | Restrict to app subnets; enable SSL. |
| PostgreSQL | 5432/tcp | Use PgBouncer for pooling. |
| Redis | 6379/tcp | Disable `CONFIG` command; require AUTH/TLS. |
| etcd | 2379-2380/tcp | mTLS only; critical for Kubernetes control plane. |
| Kubernetes API | 6443/tcp | Protect with RBAC, audit logs, API server firewall. |
| Node Exporter | 9100/tcp | Scrape over private networks only. |
| Prometheus | 9090/tcp | RBAC + auth proxy; disable open internet access. |

### Handy Linux One-Liners
- `for p in 22 80 443; do nc -zv example.com $p; done` → quick port scan.
- `curl -s https://ifconfig.me` → check outbound IP.
- `openssl s_client -connect host:443 -servername host` → inspect TLS chain.
- `sudo nft add rule inet filter output tcp dport 443 counter` → temp egress logging.
- `kubectl run tmp --rm -it --image=wbitt/network-multitool` → Swiss-army pod.

### Cloud Pricing Watchpoints
- NAT Gateways and data transfer costs dominate network bills; consider using EC2-based NAT instances or PrivateLink.
- Inter-AZ transfer not always free; design data replication with costs in mind.
- Load balancer hourly + LCU/NU charges; right-size listeners/protocols.
- DNS query volume costs (Route53 $0.40/million) still matter at scale.

### Reading List
- AWS Well-Architected Framework (Networking Pillar).
- Google SRE Book chapters on load balancing and capacity planning.
- “Networking and Kubernetes” by James Strong & Vallery Lancey.
- Cloudflare blog deep dives on TLS, QUIC, and Zero Trust.
- `man 7 ip`, `man 7 tcp`, `man 7 socket` – foundational references.

## Action Plan Checklist
- [ ] Map every service to ports/CIDRs in a spreadsheet or IaC outputs.
- [ ] Enable flow logs + log shipping to SIEM.
- [ ] Automate TLS issuance/rotation (cert-manager, ACM).
- [ ] Enforce default-deny NetworkPolicies/security groups.
- [ ] Build “network smoke tests” job in CI (dns lookup, curl, traceroute).
- [ ] Document troubleshooting runbooks per environment.
- [ ] Schedule quarterly game days focusing on network failure modes.

---
