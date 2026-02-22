# 03 — Kubernetes (K8s)

---

## SECTION 1 — Architecture

**Q: Explain the control plane components.**

```
kube-apiserver          — front-end of the control plane. ALL communication goes through it.
                          Validates and processes API requests, writes state to etcd.
                          Horizontally scalable. Exposes the Kubernetes REST API.

etcd                    — distributed key-value store. Single source of truth for cluster state.
                          Every object (pod, deployment, secret, configmap) is stored here.
                          Strongly consistent via Raft consensus. Back this up regularly.

kube-scheduler          — watches for unscheduled pods (no nodeName assigned).
                          Assigns them to nodes based on:
                            - Resource requests vs node capacity
                            - Node affinity / anti-affinity rules
                            - Taints and tolerations
                            - Pod topology spread constraints

kube-controller-manager — runs controllers as goroutines in one process:
                            ReplicaSet controller, Deployment controller,
                            Node controller, Job controller, ServiceAccount controller.
                          Each controller watches API state and reconciles actual -> desired.

cloud-controller-manager — interfaces with cloud provider APIs for:
                            Load balancer provisioning, node lifecycle, storage provisioning.
                            Separates cloud-specific code from core Kubernetes.
```

---

**Q: Explain the worker node components.**

```
kubelet           — agent running on every node. Receives PodSpecs from the API server.
                    Ensures containers described in those PodSpecs are running and healthy.
                    Talks to container runtime via CRI (Container Runtime Interface).
                    Reports node and pod status back to the API server.

kube-proxy        — runs on every node. Manages iptables or ipvs rules to implement
                    Services. When traffic hits a Service ClusterIP, kube-proxy rules
                    forward it to one of the backing pod IPs (load balancing).

container runtime — actually pulls images and runs containers.
                    containerd (most common), CRI-O.
                    Docker is no longer directly supported as a runtime (since K8s 1.24).
```

---

## SECTION 2 — Core Workload Objects

**Q: What is a Pod?**

A: The smallest deployable unit in Kubernetes. A Pod wraps one or more containers that share:
- Network namespace (same IP address, same localhost, ports must not conflict)
- Storage volumes
- Pod spec (resource limits, environment variables, service account)

Pods are ephemeral. They are not self-healing. Never manage them directly in production —
use higher-level controllers: Deployment, StatefulSet, DaemonSet, Job.

---

**Q: Deployment vs StatefulSet vs DaemonSet — differences and when to use each?**

```
Deployment:
  - Manages stateless application replicas. Pods are interchangeable.
  - Supports rolling updates and rollbacks.
  - Pods are assigned random names (myapp-7d9f4b-xk2pz).
  - No stable identity, no persistent storage tied to individual pods.
  - Use for: REST APIs, web servers, stateless workers, frontend apps.

StatefulSet:
  - For stateful applications requiring stable identity.
  - Each pod gets a stable, ordered hostname: pod-0, pod-1, pod-2.
  - Each pod gets its own PersistentVolumeClaim that survives pod restarts.
  - Ordered startup (pod-0 before pod-1) and ordered shutdown (reverse order).
  - Use for: PostgreSQL, MongoDB, Kafka, Zookeeper, Elasticsearch.

DaemonSet:
  - Ensures exactly one pod runs on every node (or a selected subset of nodes).
  - When nodes are added to the cluster, the pod is automatically scheduled on them.
  - When nodes are removed, the pod is garbage collected.
  - Use for: log collectors (Fluentd, Filebeat), metrics agents (node-exporter),
             network plugins (Calico, Cilium), storage daemons.
```

---

**Q: What are Services? Explain all types.**

A: A Service provides a stable virtual IP and DNS name for a set of pods. The pods it targets
are determined by a label selector. Services decouple consumers from pod IP changes.

```
ClusterIP (default):
  - Allocates a stable virtual IP reachable only inside the cluster.
  - DNS: my-service.my-namespace.svc.cluster.local
  - Use for: internal service-to-service communication.

NodePort:
  - Exposes the service on a static port (30000-32767) on every node's IP.
  - External traffic: <NodeIP>:<NodePort> -> Service -> Pod.
  - Not recommended for production (port management is painful, exposes all nodes).
  - Use for: development, demos, when no cloud load balancer is available.

LoadBalancer:
  - Provisions a cloud load balancer (AWS ALB/NLB, GCP LB, Azure LB).
  - Gets an external IP automatically.
  - Most expensive (one LB per service). Use Ingress to share one LB.
  - Use for: production external exposure.

ExternalName:
  - Maps a Kubernetes service name to an external DNS name.
  - No proxying — just a CNAME DNS record.
  - Use for: abstracting external databases or services behind a K8s service name.
             Lets you change the external endpoint without updating app config.

Headless (clusterIP: None):
  - No virtual IP, no load balancing.
  - DNS query returns individual pod IPs directly.
  - Use for: StatefulSets where apps need to address specific pods (Kafka, Cassandra).
             Client-side load balancing.
```

---

**Q: What is an Ingress?**

A: An API object that manages external HTTP/HTTPS access to services within the cluster.
An Ingress Controller (nginx-ingress, Traefik, AWS ALB Ingress Controller) reads Ingress
resources and configures a reverse proxy / load balancer accordingly.

Benefits over LoadBalancer Services:
- One load balancer for many services (cost saving)
- Path-based and host-based routing
- TLS termination
- Middleware (rate limiting, authentication, redirects)

```yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: api-ingress
  namespace: production
  annotations:
    nginx.ingress.kubernetes.io/rewrite-target: /
    nginx.ingress.kubernetes.io/rate-limit: "100"
spec:
  ingressClassName: nginx
  rules:
  - host: api.example.com
    http:
      paths:
      - path: /v1
        pathType: Prefix
        backend:
          service:
            name: api-service
            port:
              number: 80
      - path: /v2
        pathType: Prefix
        backend:
          service:
            name: api-v2-service
            port:
              number: 80
  - host: admin.example.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: admin-service
            port:
              number: 80
  tls:
  - hosts:
    - api.example.com
    - admin.example.com
    secretName: tls-secret
```

---

## SECTION 3 — Configuration & Secrets

**Q: ConfigMap vs Secret — when to use each?**

```
ConfigMap:
  - Non-sensitive configuration: environment names, feature flags, config files.
  - Stored as plaintext in etcd.
  - Can be mounted as a volume (file) or exposed as environment variables.

Secret:
  - Sensitive data: passwords, API keys, TLS certificates, tokens.
  - Base64-encoded in etcd (NOT encrypted by default — base64 is not encryption!).
  - To actually encrypt: enable etcd encryption at rest, or use:
      - Sealed Secrets (encrypts secret YAML for safe Git storage)
      - External Secrets Operator (syncs from AWS Secrets Manager / HashiCorp Vault)
      - CSI Secret Store Driver
```

```yaml
# ConfigMap example
apiVersion: v1
kind: ConfigMap
metadata:
  name: app-config
data:
  LOG_LEVEL: "info"
  APP_PORT: "8080"
  config.yaml: |
    database:
      max_connections: 20
      idle_timeout: 300s
    feature_flags:
      dark_mode: true

---
# Secret example
apiVersion: v1
kind: Secret
metadata:
  name: db-secret
type: Opaque
data:
  username: cG9zdGdyZXM=    # echo -n 'postgres' | base64
  password: c2VjcmV0MTIz    # echo -n 'secret123' | base64
stringData:
  # stringData is base64-encoded automatically — easier to work with
  api_key: "my-plain-text-api-key"
```

Using ConfigMap and Secret in a Pod:

```yaml
spec:
  containers:
  - name: app
    image: myapp:v1
    # Individual env vars from ConfigMap and Secret
    env:
    - name: LOG_LEVEL
      valueFrom:
        configMapKeyRef:
          name: app-config
          key: LOG_LEVEL
    - name: DB_PASSWORD
      valueFrom:
        secretKeyRef:
          name: db-secret
          key: password
    # All keys from ConfigMap as env vars
    envFrom:
    - configMapRef:
        name: app-config
    # Mount config file as a volume
    volumeMounts:
    - name: config-volume
      mountPath: /etc/app
      readOnly: true
  volumes:
  - name: config-volume
    configMap:
      name: app-config
```

---

## SECTION 4 — Health Probes

**Q: Liveness vs Readiness vs Startup probe — explain each.**

```
Liveness probe:
  QUESTION: Is the container still alive and functional?
  FAIL ACTION: kubelet KILLS and RESTARTS the container.
  USE FOR: detecting deadlocks, hung goroutines, corrupted in-memory state.
  IMPORTANT: Do NOT check dependencies (DB, external APIs) in liveness probes.
             If the DB goes down, you don't want all your pods restarting.

Readiness probe:
  QUESTION: Is the container ready to serve traffic RIGHT NOW?
  FAIL ACTION: Pod is REMOVED from the Service's Endpoints list. Traffic stops flowing to it.
               The container is NOT restarted.
  USE FOR: waiting for DB connections, cache warm-up, initial data loading, rolling updates.
  IMPORTANT: This is how zero-downtime deployments work. New pods don't get traffic until ready.

Startup probe:
  QUESTION: Has the application finished starting up?
  FAIL ACTION: kubelet kills and restarts the container if startup takes too long.
  PURPOSE: Disables liveness and readiness probes until this probe succeeds.
           Prevents liveness probes from killing slow-starting containers.
  USE FOR: legacy applications that take several minutes to initialize.
```

```yaml
livenessProbe:
  httpGet:
    path: /healthz    # should return 200, NOT check external dependencies
    port: 8080
  initialDelaySeconds: 15   # wait before first check (give app time to start)
  periodSeconds: 20          # check every 20 seconds
  timeoutSeconds: 5          # fail if no response within 5 seconds
  failureThreshold: 3        # restart after 3 consecutive failures

readinessProbe:
  httpGet:
    path: /ready      # can check DB connection, cache loaded, etc.
    port: 8080
  initialDelaySeconds: 5
  periodSeconds: 10
  failureThreshold: 3

startupProbe:
  httpGet:
    path: /healthz
    port: 8080
  failureThreshold: 30   # 30 x 10s = allows up to 5 minutes to start
  periodSeconds: 10
```

---

## SECTION 5 — Resources & Autoscaling

**Q: Resource requests vs limits — explain the difference.**

```
requests:
  - The GUARANTEED amount of CPU/memory reserved for the container.
  - The scheduler uses requests to decide which node has enough capacity.
  - A node will only be assigned a pod if it has enough allocatable resources.

limits:
  - The MAXIMUM amount the container is allowed to use.
  - CPU over limit: container is CPU-throttled (slowed down, not killed).
  - Memory over limit: container is OOMKilled (killed immediately by the kernel).
```

```yaml
resources:
  requests:
    memory: "128Mi"
    cpu: "250m"        # 250 millicores = 0.25 vCPU core
  limits:
    memory: "512Mi"
    cpu: "1"           # 1 full vCPU core
```

---

**Q: What are QoS (Quality of Service) classes?**

```
Guaranteed  — requests == limits for ALL containers in the pod.
              Highest priority. Last to be evicted under memory pressure.
              Example: requests.memory == limits.memory AND requests.cpu == limits.cpu

Burstable   — at least one container has requests != limits.
              Medium priority. Evicted after BestEffort pods.

BestEffort  — no resource requests or limits set on ANY container.
              Lowest priority. First to be evicted under memory pressure.
```

---

**Q: What is HPA (Horizontal Pod Autoscaler)?**

A: Automatically scales the number of pod replicas based on observed CPU utilisation, memory
utilisation, or custom metrics. The HPA controller queries the Metrics Server every 15 seconds.

```yaml
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: api-hpa
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: api
  minReplicas: 2
  maxReplicas: 20
  metrics:
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 70     # scale up if average CPU > 70%
  - type: Resource
    resource:
      name: memory
      target:
        type: Utilization
        averageUtilization: 80
  behavior:
    scaleDown:
      stabilizationWindowSeconds: 300   # wait 5 min before scaling down
      policies:
      - type: Percent
        value: 10
        periodSeconds: 60
```

---

## SECTION 6 — Networking

**Q: How does pod-to-pod communication work in Kubernetes?**

A: Kubernetes requires a flat network model: every pod can reach every other pod directly by
IP address without NAT. This is implemented by CNI (Container Network Interface) plugins.

```
Each node gets a subnet (e.g., node1: 10.244.1.0/24, node2: 10.244.2.0/24).
Pods on a node get IPs from the node's subnet.
The CNI plugin configures routing so pods on different nodes can reach each other.
```

CNI Plugin comparison:

```
Flannel  — simple VXLAN overlay. Easy to set up. Limited features. No NetworkPolicy.
Calico   — BGP routing (no overlay on supported networks). NetworkPolicy support.
           Production-grade. Most popular for on-prem.
Cilium   — eBPF-based. Very high performance. L7 NetworkPolicies. Deep observability.
           Best for large-scale, security-sensitive environments.
```

---

**Q: How does a Service route traffic to pods?**

A: When a Service is created, the Endpoints controller builds an Endpoints object containing
all pod IPs that match the selector. kube-proxy on each node watches for Endpoint changes and
updates iptables/ipvs rules. When traffic hits the Service's ClusterIP, these rules DNAT
(Destination NAT) the traffic to one of the pod IPs.

---

**Q: What are NetworkPolicies?**

A: L3/L4 firewall rules for pod traffic. By default Kubernetes has no network isolation — all
pods can communicate with all pods. NetworkPolicies restrict ingress and/or egress.

Note: NetworkPolicies require a CNI plugin that supports them (Calico, Cilium, Weave — NOT Flannel).

```yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: backend-policy
  namespace: production
spec:
  podSelector:
    matchLabels:
      app: backend      # this policy applies to backend pods
  policyTypes:
  - Ingress
  - Egress
  ingress:
  - from:
    - podSelector:
        matchLabels:
          app: frontend  # only allow traffic from frontend pods
    - namespaceSelector:
        matchLabels:
          name: monitoring  # allow from monitoring namespace
    ports:
    - protocol: TCP
      port: 8080
  egress:
  - to:
    - namespaceSelector:
        matchLabels:
          name: database
    ports:
    - protocol: TCP
      port: 5432
  - to:                    # allow DNS resolution
    - namespaceSelector: {}
    ports:
    - protocol: UDP
      port: 53
```

---

## SECTION 7 — Storage

**Q: PersistentVolume, PersistentVolumeClaim, and StorageClass?**

```
PersistentVolume (PV):
  - A piece of storage provisioned in the cluster (AWS EBS, NFS, GCE PD, local disk).
  - A cluster-level resource (not namespaced).
  - Can be provisioned statically (admin creates it) or dynamically (StorageClass).

PersistentVolumeClaim (PVC):
  - A request for storage by a user/pod.
  - Specifies: size, access mode, storage class.
  - Kubernetes binds it to a matching PV.
  - Namespaced resource.

StorageClass:
  - Defines a "class" of storage with specific provisioner and parameters.
  - Enables dynamic provisioning: PVC requests a class -> PV automatically created.
  - Examples: gp3 (AWS EBS), pd-ssd (GCP), standard (minikube).
```

Access Modes:

```
ReadWriteOnce (RWO)  — can be mounted read-write by a single node at a time.
                       Most block storage (EBS, GCE PD).
ReadOnlyMany (ROX)   — can be mounted read-only by many nodes simultaneously.
ReadWriteMany (RWX)  — can be mounted read-write by many nodes simultaneously.
                       Requires network storage (NFS, EFS, CephFS).
```

```yaml
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: postgres-pvc
spec:
  accessModes:
  - ReadWriteOnce
  storageClassName: gp3
  resources:
    requests:
      storage: 50Gi
```

---

## SECTION 8 — Rolling Updates & Rollbacks

**Q: How does a Deployment rolling update work?**

A: When you update the pod template (image, env vars, etc.), the Deployment controller creates
a new ReplicaSet and gradually scales it up while scaling down the old one.

```yaml
strategy:
  type: RollingUpdate
  rollingUpdate:
    maxSurge: 1         # max pods ABOVE desired count during update (can be %)
    maxUnavailable: 0   # max pods BELOW desired count during update
                        # 0 means no pod is taken down until new one is ready
minReadySeconds: 10     # wait 10s after pod is ready before marking it available
```

With `maxUnavailable: 0` and `maxSurge: 1`:
1. Bring up 1 extra new pod (surge)
2. Wait for it to pass readiness probe
3. Take down 1 old pod
4. Repeat until all pods are updated

---

**Q: How do you roll back a deployment?**

```bash
kubectl rollout history deployment/myapp                  # see revision history
kubectl rollout history deployment/myapp --revision=3     # see specific revision details
kubectl rollout undo deployment/myapp                     # roll back to previous
kubectl rollout undo deployment/myapp --to-revision=3     # roll back to specific revision
kubectl rollout status deployment/myapp                   # watch rollout progress
```

---

## SECTION 9 — Debugging Scenarios

**Q: A pod is in CrashLoopBackOff. How do you debug?**

```bash
# Step 1: Look at events and pod description
kubectl describe pod <pod-name> -n <namespace>
# Look at: State, Last State, Exit Code, Events section

# Step 2: Get logs from the crashed container
kubectl logs <pod-name> --previous -n <namespace>
kubectl logs <pod-name> -c <container-name> --previous

# Step 3: Try running the container with a different command to explore
kubectl run debug --image=<same-image> -it --rm -- sh
```

Common causes and exit codes:

```
Exit Code 1    — application error on startup (check logs)
Exit Code 137  — OOMKilled (memory limit exceeded) or SIGKILL
Exit Code 139  — segfault
Exit Code 143  — SIGTERM (graceful shutdown requested but app exited with error)

Other checks:
  - Is the liveness probe too aggressive? (failureThreshold too low, initialDelaySeconds too short)
  - Is a required environment variable or secret missing?
  - Is the image wrong or not pullable? (ImagePullBackOff is a separate state)
  - Is the app trying to bind to a port already in use?
```

---

**Q: A deployment rollout is stuck. What do you check?**

```bash
kubectl rollout status deployment/myapp   # this will hang and show the reason
kubectl get pods                          # are new pods Pending or CrashLoopBackOff?
kubectl describe pod <new-pod>            # check Events

# Common reasons a rollout stalls:
#   - New pods stuck in Pending: insufficient CPU/memory on nodes
#   - New pods stuck in Pending: PVC not bound, missing PV
#   - New pods in CrashLoopBackOff: app fails to start (broken image, bad config)
#   - Readiness probe never passes: app starts but /ready returns non-200
#   - ImagePullBackOff: wrong image name, missing registry credentials

# Roll back if it's critical
kubectl rollout undo deployment/myapp
```

---

**Q: How do you achieve zero-downtime deployments?**

```
1. Correct readiness probes — K8s will NOT route traffic to a pod until readiness passes.
                              This is the single most important thing.

2. RollingUpdate with maxUnavailable: 0 — never reduce below desired pod count.

3. Graceful shutdown — app must handle SIGTERM:
                       - Stop accepting new connections
                       - Finish in-flight requests
                       - Close DB connections
                       - Exit with code 0

4. preStop hook — gives the app time to drain before SIGTERM:
   lifecycle:
     preStop:
       exec:
         command: ["/bin/sh", "-c", "sleep 5"]

5. terminationGracePeriodSeconds — give enough time for graceful shutdown (default 30s).

6. PodDisruptionBudget — ensures minimum availability during node drains and updates:
   minAvailable: 2   (always keep at least 2 pods running)
```

---

## SECTION 10 — Essential kubectl Commands

```bash
# Context management
kubectl config get-contexts
kubectl config use-context <context-name>
kubectl config current-context

# Viewing resources
kubectl get pods -n <namespace> -o wide          # with node and IP
kubectl get pods -A                              # all namespaces
kubectl get all -n <namespace>                   # pods, services, deployments
kubectl get events --sort-by='.lastTimestamp' -n <namespace>
kubectl top pods -n <namespace>                  # CPU/memory (needs metrics-server)
kubectl top nodes

# Describing & Logging
kubectl describe pod <pod-name>
kubectl describe node <node-name>
kubectl logs <pod-name> -f --tail=200
kubectl logs <pod-name> -c <container> --previous

# Executing
kubectl exec -it <pod-name> -- bash
kubectl exec -it <pod-name> -c <container> -- sh

# Deployments
kubectl rollout status deployment/myapp
kubectl rollout history deployment/myapp
kubectl rollout undo deployment/myapp
kubectl scale deployment myapp --replicas=5
kubectl set image deployment/myapp myapp=myapp:v2

# Apply / Delete
kubectl apply -f manifest.yaml
kubectl apply -f ./k8s/                          # apply all files in directory
kubectl delete -f manifest.yaml
kubectl diff -f manifest.yaml                    # show what would change before applying

# Port forwarding (for local testing)
kubectl port-forward svc/myapp 8080:80
kubectl port-forward pod/<pod-name> 5432:5432

# Debugging
kubectl run debug --image=nicolaka/netshoot -it --rm --restart=Never
kubectl cp <pod-name>:/path/to/file ./local-file
```

---

## SECTION 11 — Helm

**Q: What is Helm and why is it useful?**

A: Helm is the package manager for Kubernetes. It templates Kubernetes YAML manifests,
making it easy to install, upgrade, and manage complex applications.

```
Chart      — a package of pre-configured Kubernetes resources.
Release    — an instance of a chart deployed to a cluster.
Repository — collection of charts (like a package registry).
values.yaml — default configuration values for a chart.
```

```bash
# Basic workflow
helm repo add bitnami https://charts.bitnami.com/bitnami
helm repo update

helm search repo bitnami/postgresql              # find a chart
helm show values bitnami/postgresql              # inspect default values

helm install my-pg bitnami/postgresql \
  --namespace database \
  --create-namespace \
  --set auth.postgresPassword=secret123 \
  --set primary.persistence.size=50Gi

helm upgrade my-pg bitnami/postgresql \
  --set primary.replicaCount=3

helm list -A                                     # all releases across namespaces
helm history my-pg                               # release history
helm rollback my-pg 2                            # rollback to revision 2
helm uninstall my-pg -n database

# Dry run (render manifests without applying)
helm template ./mychart --values prod-values.yaml
helm install my-app ./mychart --dry-run --debug
```
