This transcript provides a complete guide for deploying a Node.js application on **AWS EC2** using **Nginx** as a reverse proxy with an **SSL certificate**.

---

## **1. Environment Setup (AWS EC2)**

To host the application, you need a virtual machine in the cloud.

* **Service Used:** AWS EC2 (Elastic Compute Cloud).
* **Region Selection:** Choose the region nearest to you (e.g., **Mumbai** for users in India) to reduce **latency**.
* **Instance Configuration:**
* **OS:** Ubuntu 22.04 LTS.
* **Instance Type:** `t2.medium` (2 vCPUs, 4GB RAM) was used for performance, though `t2.micro` is available for free tier.


* **Security:** Create a **Key Pair** (.pem file) to securely access the server via SSH. Store this file safely; if lost, you cannot access your machine.

---

## **2. Networking and Static IP**

By default, EC2 public IPs are temporary and change on restart.

* **Elastic IP:** Allocate an Elastic IP and associate it with your EC2 instance to ensure your server has a **Permanent/Static IP address**.
* **Security Groups:** Open specific ports to allow traffic:
* **SSH (22):** For remote access.
* **HTTP (80):** For standard web traffic.
* **HTTPS (443):** For secure web traffic.
* **Custom Port (e.g., 8001):** If you want to test the application directly before setting up the proxy.



---

## **3. Application Deployment**

Connect to the server via terminal using SSH:
`ssh -i "your-key.pem" ubuntu@your-public-ip`

### **Installation Steps:**

1. **Update Packages:** `sudo apt-get update`.
2. **Install Node.js:** Use a version manager or setup script for Node.js (Version 18 used in the video).
3. **Clone Project:** Use `git clone` to pull your source code from GitHub.
4. **Install Dependencies:** Run `npm install` inside the project folder.

### **Process Management with PM2:**

To keep the application running in the background even after closing the terminal:

* **Install PM2:** `sudo npm install -g pm2`.
* **Start App:** `pm2 start index.js`.
* **Logs/Management:** Use `pm2 logs` and `pm2 restart all`.

---

## **4. Configuring Nginx as a Reverse Proxy**

Instead of users accessing the app via `IP:8001`, Nginx acts as a middleman.

### **Configuration Steps:**

* **Edit Config:** `sudo vim /etc/nginx/sites-available/default`.
* **Server Name:** Set your domain (e.g., `piyushgarg.dev`).
* **Proxy Pass:** Direct traffic to the local port where Node.js is running.

**Example Config Block:**

```nginx
location / {
    proxy_pass http://localhost:8001; # Redirects port 80 to 8001
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection 'upgrade';
    proxy_set_header Host $host;
    proxy_cache_bypass $http_upgrade;
}

```

* **Apply Changes:** Test with `sudo nginx -t` and reload with `sudo nginx -s reload`.

---

## **5. Domain and SSL Setup**

* **DNS Mapping:** Go to your domain provider (e.g., Freenom, GoDaddy) and point the **A Record** to your AWS Elastic IP.
* **SSL Certificate:** This adds "HTTPS" and the padlock icon to the site. The video concludes by preparing for SSL installation (typically using **Certbot** by Let's Encrypt).

---
Here is the updated final section of the notes, covering **SSL Installation with Certbot**, **Auto-renewal**, and **Resource Cleanup** to avoid AWS charges.

---

## **6. Securing the Application with SSL (HTTPS)**

To add the padlock icon and encrypt traffic, we use **Certbot** by Let’s Encrypt. It automates the process of obtaining and renewing SSL certificates for Nginx.

### **Installation & Configuration:**

1. **Install Certbot:**
```bash
sudo apt-get install python3-certbot-nginx

```


2. **Generate Certificate:**
Run the following command to automatically update your Nginx configuration with the SSL certificate:
```bash
sudo certbot --nginx -d yourdomain.com -d www.yourdomain.com

```


* **`-d`**: Stands for "Domain."
* **Challenge Failures:** If the `www` version fails, ensure you have added a corresponding **CNAME** or **A record** in your DNS settings and waited for it to propagate.


3. **Process:** Certbot will ask for your email, agreement to terms, and whether to redirect HTTP traffic to HTTPS (recommended).

---

## **7. Managing SSL Certificates**

* **Validity:** Let's Encrypt certificates are valid for **90 days**.
* **Auto-Renewal:** To prevent the certificate from expiring, you can set up a renewal command:
```bash
sudo certbot renew --dry-run

```


*This ensures that Nginx will automatically refresh the certificate before it expires.*

---

## **8. Testing the Full-Stack Deployment**

Once Nginx is proxied and SSL is active, the application is fully functional:

* **Domain Access:** The app is accessible via `https://yourdomain.com` without needing a port number.
* **Database Connectivity:** Features like **Sign-up**, **Login**, and **URL Generation** (Shortening) will work as they communicate through the proxy to the Node.js backend and MongoDB.

---

## **9. Important: Resource Cleanup (Avoiding Charges)**

AWS charges for running instances and idle Elastic IPs. Once you are done with your demo/testing, follow these steps to avoid a high bill:

| Step | Action | Why? |
| --- | --- | --- |
| **1. Terminate Instance** | Select Instance → Instance State → **Terminate** | Stops billing for the virtual machine immediately. |
| **2. Release Elastic IP** | Network & Security → Elastic IPs → **Release** | **Crucial:** AWS charges for Elastic IPs that are *not* attached to a running instance. |
| **3. Delete Volumes** | Storage → Volumes → **Delete** | Terminates any EBS storage associated with the machine. |

---

### **Summary of Final Workflow**

1. **Deploy** Node.js code and manage it with **PM2**.
2. **Route** traffic through **Nginx** on Port 80.
3. **Map** your domain using **DNS A Records**.
4. **Secure** the site with **Certbot** for HTTPS.
5. **Clean up** resources on AWS Dashboard to stay within the free tier.

---

Here is your **Ultimate Nginx & Node.js Deployment Cheat Sheet**. This single-page guide covers everything from server setup to SSL security.

---

# 🚀 Nginx + Node.js Deployment Cheat Sheet

## **Phase 1: AWS Server Setup**

* **Launch EC2:** Ubuntu 22.04 LTS, Type `t2.micro` or `t2.medium`.
* **Static IP:** Allocate an **Elastic IP** and associate it with your instance.
* **Security Group (Firewall):** Open these Inbound ports:
* `22` (SSH)
* `80` (HTTP)
* `443` (HTTPS)


* **DNS:** Point your Domain's **A Record** to your Elastic IP.

---

## **Phase 2: Server Environment**

```bash
# 1. Update system
sudo apt-get update && sudo apt-get upgrade -y

# 2. Install Node.js (v18 example)
curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
sudo apt-get install -y nodejs

# 3. Install Nginx & PM2
sudo apt-get install nginx -y
sudo npm install -g pm2

```

---

## **Phase 3: App Deployment**

```bash
# 1. Clone & Install
git clone <your-repo-link>
cd <folder-name>
npm install

# 2. Setup Environment Variables (Optional)
export MONGODB_URI='your-connection-string'

# 3. Start App with PM2
pm2 start index.js --name "my-app"
pm2 save
pm2 startup

```

---

## **Phase 4: Nginx Reverse Proxy**

**File Path:** `/etc/nginx/sites-available/default`

```nginx
server {
    listen 80;
    server_name yourdomain.com www.yourdomain.com;

    location / {
        proxy_pass http://localhost:8001; # Your Node.js Port
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection 'upgrade';
        proxy_set_header Host $host;
        proxy_cache_bypass $http_upgrade;
    }
}

```

**Apply Changes:**

```bash
sudo nginx -t            # Test syntax
sudo nginx -s reload     # Reload config

```

---

## **Phase 5: SSL (HTTPS)**

```bash
# 1. Install Certbot
sudo apt-get install python3-certbot-nginx -y

# 2. Get Certificate (Automated Nginx Setup)
sudo certbot --nginx -d yourdomain.com -d www.yourdomain.com

# 3. Verify Auto-renewal
sudo certbot renew --dry-run

```

---

## **Phase 6: Maintenance & Cleanup**

* **Logs:** `pm2 logs` or `sudo tail -f /var/log/nginx/error.log`
* **Status:** `pm2 status`
* **DELETION (To stop billing):**
1. **Terminate** EC2 Instance.
2. **Release** Elastic IP (Crucial!).
3. **Delete** EBS Volumes.



---

**Next Step:** This marks the end of our deployment project! Do you have a specific part of the Nginx configuration or a different tech stack you'd like to explore next?