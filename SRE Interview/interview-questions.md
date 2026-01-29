# SRE Intern Interview Preparation
 Guide

## Round 1 - Technical Round with SRE Team
**Duration:** 1 hr 15 mins

### 1) Types of AWS EC2 Instance Types

AWS EC2 instances are categorized based on their use cases and resource configurations:

- **General Purpose (T, M series)**: Balanced compute, memory, and networking. Good for web servers and small databases.
  - *Example*: t3.medium, m5.large

- **Compute Optimized (C series)**: High-performance processors for compute-intensive tasks like batch processing and gaming servers.
  - *Example*: c5.xlarge

- **Memory Optimized (R, X series)**: Fast performance for workloads that process large datasets in memory, like databases and cache.
  - *Example*: r5.large

- **Storage Optimized (I, D series)**: High sequential read/write access to large datasets on local storage.
  - *Example*: i3.xlarge

- **Accelerated Computing (P, G series)**: Hardware accelerators (GPUs) for machine learning and graphics rendering.
  - *Example*: p3.2xlarge

### 2) What is SLA, SLO, SLI

These are key metrics for measuring service reliability:

- **SLI (Service Level Indicator)**: A quantitative measure of service performance
  - *Example*: Request latency, error rate, system throughput
  - Think of it as: "What am I measuring?"

- **SLO (Service Level Objective)**: Target value or range for an SLI
  - *Example*: 99.9% of requests should complete in under 200ms
  - Think of it as: "What is my goal?"

- **SLA (Service Level Agreement)**: A formal contract with consequences if SLOs aren't met
  - *Example*: If uptime falls below 99.9%, customer gets 10% refund
  - Think of it as: "What happens if I miss my goal?"

**Simple analogy**: If you're delivering pizzas - SLI is delivery time, SLO is "deliver within 30 minutes", SLA is "free pizza if we're late."

### 3) What is MTTR, MTTA, MTTD

These metrics measure how quickly teams respond to and resolve incidents:

- **MTTD (Mean Time To Detect)**: Average time to discover an incident has occurred
  - *Example*: From when the server crashes to when monitoring alerts fire

- **MTTA (Mean Time To Acknowledge)**: Average time for the team to acknowledge and start investigating
  - *Example*: From alert firing to engineer saying "I'm on it"

- **MTTR (Mean Time To Repair/Resolve)**: Average time to fix the issue and restore service
  - *Example*: From incident start to full service restoration

**Goal**: Keep all these metrics as low as possible for better reliability.

### 4) What is Incident Management

Incident Management is the structured process of handling unplanned service disruptions to restore normal operations quickly.

**Key phases**:
1. **Detection**: Identify something is wrong (via monitoring, alerts, user reports)
2. **Response**: Acknowledge and assign appropriate team members
3. **Diagnosis**: Investigate root cause while communicating with stakeholders
4. **Resolution**: Implement fix and verify service restoration
5. **Post-mortem**: Document what happened, why, and how to prevent it

**Best practices**:
- Clear escalation paths
- Communication channels (status pages, Slack)
- Defined severity levels (P0, P1, P2, etc.)
- Blameless post-mortems

### 5) How would you manage your time if you got 5 P0 (highest priority) incidents

**Approach**:

1. **Don't panic - Assess quickly**: Take 30 seconds to understand each incident's impact

2. **Triage based on**:
   - Customer impact (how many users affected?)
   - Business impact (revenue loss, SLA violations?)
   - Dependencies (is one incident causing others?)

3. **Delegate and parallelize**:
   - Call for backup immediately
   - Assign incidents to available team members
   - If alone, focus on highest impact first

4. **Communicate early**:
   - Update status page
   - Notify stakeholders
   - Set expectations on resolution time

5. **Look for common causes**: Multiple P0s might stem from one root issue (e.g., database down affecting multiple services)

6. **Document as you go**: Quick notes help with handoff and post-mortem

**Example**: If a database failure is causing 3 of the 5 incidents, fix the database first - it resolves multiple issues at once.

### 6) Scenario based questions on AWS

**Common scenarios to prepare**:

- **High traffic scenario**: How to handle sudden traffic spike?
  - *Answer*: Use Auto Scaling Groups, CloudFront CDN, elastic load balancers

- **Data loss prevention**: How to ensure data isn't lost?
  - *Answer*: Regular snapshots, Multi-AZ deployment, S3 versioning

- **Cost optimization**: Service is too expensive, how to reduce costs?
  - *Answer*: Right-size instances, use Reserved Instances, implement auto-scaling, use S3 lifecycle policies

- **Security breach**: Unauthorized access detected, what to do?
  - *Answer*: Rotate credentials immediately, check CloudTrail logs, review security groups, enable MFA

### 7) How would you pass on your knowledge or findings if some incidents were still open but your shift is over

**Effective handoff process**:

1. **Written documentation**:
   - Update incident ticket with current status
   - Document what you've tried and results
   - List next steps to investigate
   - Include relevant logs, screenshots, error messages

2. **Direct communication**:
   - Synchronous handoff call/meeting with next shift
   - Walk through each open incident
   - Answer questions and clarify context

3. **Use handoff templates**:
   ```
   - Incident ID and severity
   - Time detected
   - Current impact
   - Actions taken so far
   - Current hypothesis
   - Next steps
   - Key contacts involved
   ```

4. **Make yourself available**: Share contact info for critical issues even off-shift

5. **Update monitoring dashboards**: Ensure relevant metrics are visible

### 8) What are APM services

**APM (Application Performance Monitoring)** services help track and manage application performance and availability.

**What they do**:
- Monitor response times and transaction traces
- Detect errors and exceptions in real-time
- Track dependencies between services
- Provide distributed tracing across microservices
- Monitor database query performance
- Alert on performance degradation

**Popular APM tools**:
- New Relic
- Datadog APM
- AppDynamics
- Dynatrace
- AWS X-Ray
- Elastic APM

**Example use case**: You notice API response time increased from 100ms to 2 seconds. APM shows the delay is in a specific database query, helping you identify the exact bottleneck.

### 9) What all monitoring tools have you used

**Prepare to discuss**:

- **Infrastructure monitoring**: CloudWatch, Prometheus, Nagios
- **APM**: New Relic, Datadog
- **Log management**: ELK Stack (Elasticsearch, Logstash, Kibana), Splunk
- **Uptime monitoring**: Pingdom, UptimeRobot
- **Distributed tracing**: Jaeger, Zipkin

**For each tool, mention**:
- What you monitored
- How you set up alerts
- A specific incident you detected/resolved using it

### 10) Difference between Availability and Durability

**Availability**: Can you access your data **right now**?
- Measures uptime and accessibility
- *Example*: 99.99% availability = 52 minutes downtime per year
- About system being operational

**Durability**: Will your data still exist in the **future**?
- Measures data persistence and protection against loss
- *Example*: 99.999999999% (11 nines) durability = lose 1 object per 10 million objects every 10,000 years
- About data not being lost or corrupted

**Real-world example**:
- **High availability, low durability**: Data stored on a single hard drive that's always accessible but could crash and lose everything
- **High durability, low availability**: Data backed up to multiple locations but takes hours to retrieve
- **AWS S3**: Both high availability (99.99%) and high durability (11 nines)

### 11) How would you scale your Architecture (3 tier web Architecture)

**3-Tier Architecture**: Presentation Layer (Web) → Application Layer (App Server) → Data Layer (Database)

**Scaling strategies for each tier**:

**1. Web Tier (Presentation)**:
- Use **Load Balancer** (Application Load Balancer) to distribute traffic
- **Auto Scaling Groups** to add/remove web servers based on demand
- **CloudFront CDN** to cache static content closer to users
- Serve static assets from **S3**

**2. Application Tier**:
- **Horizontal scaling**: Add more application servers behind load balancer
- **Containerization**: Use ECS/EKS for easier scaling and deployment
- **Stateless design**: Store session data in Redis/ElastiCache, not on app servers
- **API Gateway** for rate limiting and caching

**3. Database Tier**:
- **Read replicas** for read-heavy workloads
- **Database sharding** to distribute data across multiple databases
- **Caching layer** (Redis, Memcached) to reduce database load
- **Multi-AZ deployment** for high availability
- Consider **NoSQL** (DynamoDB) for massive scale

**Additional considerations**:
- **Monitoring**: CloudWatch, custom metrics
- **Auto-scaling policies**: CPU, memory, custom metrics
- **Multi-region deployment** for global users

### 12) What motivates you to be an SRE?

**Key points to personalize and mention**:

1. **Bridge between development and operations**: Enjoy both coding and infrastructure

2. **Problem-solving at scale**: Interest in building systems that serve millions while maintaining reliability

3. **Continuous learning**: SRE involves diverse technologies - cloud, networking, databases, automation

4. **Impact**: Directly affects user experience and business continuity

5. **Automation mindset**: Passion for eliminating toil and manual work through code

6. **Culture of learning from failure**: Blameless post-mortems and continuous improvement

**Example answer**: "I'm motivated by the challenge of building reliable systems at scale. I love that SRE combines my coding skills with infrastructure knowledge. The idea of automating repetitive tasks and learning from incidents through blameless post-mortems really appeals to me. I'm excited about being in a role where my work directly impacts user experience and system reliability."

---

## Round 2 - Hiring Manager Round
**Duration:** 1 hr 10 mins

### 1) 20 mins explanation about the role

**What to listen for and ask about**:
- Daily responsibilities
- Team structure and who you'll work with
- Technologies and tools used
- On-call expectations
- Learning and mentorship opportunities
- Success metrics for the internship

### 2) What motivates you to be an SRE?

(Same as Round 1, Question 12 - but potentially dive deeper based on the role explanation)

### 3) What all do you think an SRE does?

**Core responsibilities**:

1. **Ensure reliability**: Maintain uptime and performance SLOs

2. **Incident management**: Respond to outages, conduct post-mortems

3. **Automation**: Write code to eliminate manual toil (Infrastructure as Code)

4. **Capacity planning**: Forecast growth and scale infrastructure proactively

5. **Monitoring and observability**: Build dashboards, alerts, and logging infrastructure

6. **Performance optimization**: Identify and resolve bottlenecks

7. **Disaster recovery**: Plan and test backup/recovery procedures

8. **Collaboration**: Work with developers on reliability best practices

9. **Error budget management**: Balance feature velocity with stability

**SRE philosophy**: Treat operations as a software problem - automate everything possible.

### 4) Some networking troubleshooting related AWS Architecture questions

**Common scenarios to prepare**:

**Scenario 1: EC2 instance not accessible via internet**
- Check Security Group inbound rules (port 22/80/443 open?)
- Verify Network ACL rules
- Ensure instance has public IP or Elastic IP
- Check route table has route to Internet Gateway
- Verify instance is in public subnet

**Scenario 2: Services in different VPCs can't communicate**
- Set up VPC Peering or Transit Gateway
- Update route tables in both VPCs
- Check security groups allow traffic from peer VPC CIDR

**Scenario 3: High latency for database connections**
- Check if DB and app are in same AZ (cross-AZ has latency)
- Verify security group rules aren't causing delays
- Check for network bandwidth limits
- Review VPC flow logs for packet loss

**Scenario 4: Users in specific region experiencing slow load times**
- Implement CloudFront with edge locations
- Consider multi-region deployment
- Check Route53 routing policy

### 5) What all monitoring tools have you used?

(Same as Round 1, Question 9 - be prepared to go deeper into specific tools)

### 6) What all can you monitor in an EC2 instance using CloudWatch (AWS)

**CloudWatch metrics for EC2**:

**Default metrics (no agent needed)**:
- CPU Utilization
- Disk Read/Write operations
- Network In/Out
- Status Check (instance and system)

**Custom metrics (with CloudWatch agent)**:
- Memory utilization
- Disk space utilization
- Swap usage
- Process-specific metrics
- Custom application metrics

**Logs monitoring**:
- Application logs
- System logs
- Access logs

**Alarms you can set**:
- Alert when CPU > 80% for 5 minutes
- Alert when disk space > 90%
- Alert when status check fails

### 7) What all metrics are available on CloudWatch to monitor EC2 instances other than CPU utilization and RAM

**Network metrics**:
- NetworkIn/NetworkOut (bytes)
- NetworkPacketsIn/NetworkPacketsOut

**Disk metrics**:
- DiskReadOps/DiskWriteOps (operations)
- DiskReadBytes/DiskWriteBytes (throughput)

**Status metrics**:
- StatusCheckFailed (system + instance)
- StatusCheckFailed_Instance (issues requiring your intervention)
- StatusCheckFailed_System (AWS infrastructure issues)

**EBS-specific metrics**:
- EBS ReadOps/WriteOps
- EBS ReadBytes/WriteBytes
- EBS VolumeIdleTime

**Instance metadata**:
- CPUCreditBalance (for T-series burst instances)
- CPUCreditUsage

**Custom metrics** (with agent):
- Memory usage patterns
- Swap usage
- Disk space per partition
- Number of running processes
- TCP connection count

### 8) What are different types of load balancers available on AWS and which OSI layer do they operate on

**Three types of AWS load balancers**:

**1. Application Load Balancer (ALB)**
- **OSI Layer**: Layer 7 (Application layer)
- **Routes based on**: HTTP/HTTPS content (URLs, headers, host)
- **Use cases**: Web applications, microservices, containers
- **Features**: 
  - Path-based routing (/api → backend, /images → static)
  - Host-based routing (api.example.com vs web.example.com)
  - WebSocket support
  - HTTP/2 support
- **Example**: Route /users to user service, /orders to order service

**2. Network Load Balancer (NLB)**
- **OSI Layer**: Layer 4 (Transport layer)
- **Routes based on**: IP protocol data (TCP/UDP/TLS)
- **Use cases**: High performance, low latency, TCP/UDP traffic
- **Features**:
  - Handles millions of requests per second
  - Static IP support
  - Preserves source IP
  - Ultra-low latency
- **Example**: Load balancing for gaming servers, IoT devices

**3. Classic Load Balancer (CLB)** - Legacy
- **OSI Layer**: Layer 4 and Layer 7 (both)
- **Routes based on**: Basic load balancing
- **Status**: Not recommended for new applications
- **Features**: Simple load balancing across EC2 instances

**Quick decision guide**:
- Need content-based routing? → ALB
- Need extreme performance/low latency? → NLB
- Simple TCP/HTTP load balancing? → ALB (still better than CLB)

### 9) 1 coding question (can do in any language)

**Common SRE coding questions**:

**Example 1: Log file parsing**
```python
# Parse log file and count error types
def parse_logs(log_file):
    error_counts = {}
    with open(log_file, 'r') as f:
        for line in f:
            if 'ERROR' in line:
                # Extract error type
                error_type = line.split('ERROR:')[1].split()[0]
                error_counts[error_type] = error_counts.get(error_type, 0) + 1
    return error_counts
```

**Example 2: Check if service is healthy**
```python
import requests

def health_check(url, timeout=5):
    try:
        response = requests.get(url, timeout=timeout)
        if response.status_code == 200:
            return True
        else:
            return False
    except requests.exceptions.RequestException:
        return False
```

**Example 3: Implement rate limiter**
```python
from collections import deque
import time

class RateLimiter:
    def __init__(self, max_requests, time_window):
        self.max_requests = max_requests
        self.time_window = time_window
        self.requests = deque()

    def allow_request(self):
        now = time.time()
        # Remove old requests outside time window
        while self.requests and self.requests[0] < now - self.time_window:
            self.requests.popleft()

        if len(self.requests) < self.max_requests:
            self.requests.append(now)
            return True
        return False
```

### 10) 2 SQL questions

**Common SQL questions for SRE**:

**Example 1: Find slow queries**
```sql
-- Find top 5 slowest queries from logs table
SELECT query_text, AVG(execution_time) as avg_time, COUNT(*) as count
FROM query_logs
WHERE execution_time > 1000  -- queries taking more than 1 second
GROUP BY query_text
ORDER BY avg_time DESC
LIMIT 5;
```

**Example 2: Monitor error rates**
```sql
-- Get hourly error rate from application logs
SELECT 
    DATE_TRUNC('hour', timestamp) as hour,
    COUNT(*) as total_requests,
    SUM(CASE WHEN status_code >= 500 THEN 1 ELSE 0 END) as errors,
    (SUM(CASE WHEN status_code >= 500 THEN 1 ELSE 0 END) * 100.0 / COUNT(*)) as error_rate
FROM request_logs
WHERE timestamp > NOW() - INTERVAL '24 hours'
GROUP BY hour
ORDER BY hour DESC;
```

**Example 3: Find duplicate records**
```sql
-- Find duplicate user entries
SELECT email, COUNT(*) as count
FROM users
GROUP BY email
HAVING COUNT(*) > 1;
```

**Example 4: Join tables for incident analysis**
```sql
-- Join incidents with services to find most problematic services
SELECT 
    s.service_name,
    COUNT(i.incident_id) as incident_count,
    AVG(i.resolution_time) as avg_resolution_minutes
FROM incidents i
JOIN services s ON i.service_id = s.service_id
WHERE i.created_at > NOW() - INTERVAL '30 days'
GROUP BY s.service_name
ORDER BY incident_count DESC;
```

### 11) What all subjects are you studying currently in your semester

**Be prepared to discuss**: How your current coursework relates to SRE role
- Data structures → efficient system design
- Databases → understanding data persistence and queries
- Networks → troubleshooting connectivity issues
- Operating Systems → understanding resource management
- Data Science → analyzing metrics and patterns

### 12) What are some ML Algorithms you have studied

**Common algorithms to mention**:

**Supervised Learning**:
- Linear Regression (predicting continuous values)
- Logistic Regression (binary classification)
- Decision Trees (classification and regression)
- Random Forest (ensemble method)
- Neural Networks (deep learning)

**Unsupervised Learning**:
- K-Means Clustering (grouping similar data)
- PCA (dimensionality reduction)

**How ML relates to SRE**:
- **Anomaly detection**: Identify unusual patterns in metrics
- **Predictive scaling**: Forecast traffic and scale proactively
- **Root cause analysis**: Correlate metrics to find incident causes
- **Alert optimization**: Reduce false positives using ML

### 13) One probability type of question (to test analytical ability)

**Example questions**:

**Question 1**: If a service has 99.9% uptime SLA, what's the probability it stays up for an entire year?
```
Answer: 0.999^365 ≈ 69.4%
Insight: High daily uptime doesn't guarantee yearly reliability
```

**Question 2**: System has 3 independent components, each 99% reliable. What's system reliability if all must work?
```
Answer: 0.99 × 0.99 × 0.99 = 0.970299 = 97.03%
Insight: Adding dependencies reduces overall reliability
```

**Question 3**: If 2% of requests fail, and you make 100 requests, what's the probability all succeed?
```
Answer: 0.98^100 ≈ 13.3%
Insight: Even low error rates compound over many requests
```

**Question 4**: Load balancer distributes to 5 servers. One fails. What fraction of new requests will fail if using round-robin?
```
Answer: 1/5 = 20%
Insight: Need health checks to remove failed servers from rotation
```

---

## Round 3 - CTO Round
**Duration:** 25 mins (10 mins actual interview)

### 1) Introduce yourself

**Structure** (Keep it to 2 minutes):

"Hi, I'm [Name]. I'm currently a [year] student studying [major] at [university]. 

I'm passionate about site reliability engineering because [brief motivation]. 

I have experience with [2-3 key technical skills relevant to SRE like AWS, monitoring tools, scripting].

I've completed internships at [companies] where I [1-2 major achievements].

In my current semester, I'm studying [relevant subjects] which have strengthened my understanding of [how it relates to SRE].

I'm excited about this opportunity because [specific reason related to the company]."

### 2) What all internships have you done

**Format to discuss each internship**:
- Company name and duration
- Your role/title
- Team you worked with
- 2-3 key projects or responsibilities
- Technologies used

**Keep it concise** - CTO wants the high-level view, not deep technical details (yet).

### 3) What all contributions have you done in the internships

**Use STAR method** (Situation, Task, Action, Result):

**Example 1**:
- **Situation**: Manual deployment process taking 2 hours
- **Task**: Automate deployment pipeline
- **Action**: Built CI/CD pipeline using Jenkins and Docker
- **Result**: Reduced deployment time to 15 minutes, deployments increased from weekly to daily

**Example 2**:
- **Situation**: Frequent production incidents with no monitoring
- **Task**: Implement comprehensive monitoring
- **Action**: Set up Prometheus and Grafana with custom dashboards and alerts
- **Result**: MTTD reduced from 30 minutes to 2 minutes

**Example 3**:
- **Situation**: Database queries causing application slowdowns
- **Task**: Optimize database performance
- **Action**: Analyzed slow query logs, added indexes, implemented caching
- **Result**: API response time improved from 2s to 200ms

**Focus on**:
- Problems you solved
- Initiative you took
- Measurable improvements

### 4) What is the Tech Stack you used

**Organize by category**:

**Cloud & Infrastructure**:
- AWS (EC2, S3, RDS, CloudWatch)
- Docker, Kubernetes

**Monitoring & Logging**:
- Prometheus, Grafana
- ELK Stack
- CloudWatch

**Automation & IaC**:
- Terraform
- Ansible
- Jenkins

**Programming**:
- Python (automation scripts)
- Bash scripting
- SQL

**Networking**:
- Load balancers
- VPC, Security Groups

**Be ready to explain**: Why you chose each tool and how you used it

### 5) What is the business impact of all those contributions

**Translate technical work to business value**:

**Technical → Business translations**:

| Technical Achievement | Business Impact |
|----------------------|-----------------|
| Reduced deployment time from 2hr to 15min | Faster time-to-market for features, reduced developer waiting time = cost savings |
| Improved API response time from 2s to 200ms | Better user experience → higher retention and conversion rates |
| Reduced MTTD from 30min to 2min | Less downtime = less revenue loss, better customer satisfaction |
| Automated 10 hours/week of manual tasks | Engineering team can focus on product features instead of toil |
| Increased system uptime from 99% to 99.9% | 10x less downtime = better SLA compliance, fewer support tickets |
| Implemented caching reducing database load by 70% | Lower infrastructure costs, can handle more users without scaling database |

**Framework to use**:
1. **Cost savings**: Reduced infrastructure costs or manual labor
2. **Revenue protection**: Prevented or reduced downtime
3. **Efficiency**: Faster deployments, reduced time-to-market
4. **Scalability**: Enabled business to handle growth
5. **User experience**: Improved performance = better customer satisfaction

**Example answer**:
"By implementing the CI/CD pipeline, I reduced deployment time by 90%. This had three business impacts: First, we could push bug fixes and features to customers daily instead of weekly, improving competitiveness. Second, it freed up 10 developer hours per week previously spent on manual deployments, allowing them to build
 new features. Third, automated testing caught issues before production, reducing customer-reported bugs by 40%."

---

## General Tips for All Rounds

### Before the Interview:
- Review your resume thoroughly
- Prepare specific examples from past experiences
- Practice explaining technical concepts simply
- Research the company and its tech stack
- Prepare questions to ask interviewers

### During the Interview:
- Think out loud - show your problem-solving process
- Ask clarifying questions before answering
- Be honest about what you don't know
- Use real examples from your experience
- Show enthusiasm and curiosity

### Key SRE Principles to Demonstrate:
1. **Automation mindset**: Always look to eliminate toil
2. **Data-driven decisions**: Use metrics and monitoring
3. **Blameless culture**: Focus on systems, not individuals
4. **Gradual rollouts**: Minimize blast radius of changes
5. **Error budgets**: Balance reliability with innovation

### Questions to Ask Interviewers:
- What does a typical day look like for an SRE here?
- What's the on-call rotation and escalation process?
- How does the team handle post-mortems?
- What monitoring and observability tools does the team use?
- What's the biggest reliability challenge the team is facing?
- How does SRE collaborate with development teams?
- What opportunities are there for learning and growth?

**Remember**: SRE is about balance - reliability AND velocity, automation AND human judgment, proactive AND reactive work. Show that you understand these trade-offs!
