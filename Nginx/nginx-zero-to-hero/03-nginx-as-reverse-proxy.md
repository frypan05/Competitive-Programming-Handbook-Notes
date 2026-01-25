# 🔁 Section 3: NGINX as a Reverse Proxy (Ubuntu/Linux)

## 🧠 What is a Reverse Proxy?

A **reverse proxy** is a server that receives client requests and forwards them to backend servers, then sends the response back to the client.

NGINX is one of the most popular tools used as a reverse proxy in production.

---

## 🔄 Reverse Proxy vs Forward Proxy

| Feature         | Forward Proxy                       | Reverse Proxy                           |
|-----------------|--------------------------------------|------------------------------------------|
| Who configures it | Client                              | Server-side                              |
| Forwards requests to | External servers (internet)         | Internal backend servers (apps/services) |
| Use case         | Browsing anonymously, caching       | Load balancing, SSL termination, API gateway |
| Example          | Proxy server for office users       | NGINX between frontend and backend apps  |

---

## 📌 Why Use NGINX as a Reverse Proxy?

- Protect backend services from direct access
- Centralized SSL termination
- Load balancing backend apps
- Path-based routing (`/api` → backend1, `/app` → backend2)
- Easy caching and compression

---

## 📝 Reverse Proxy Configuration (Ubuntu/Linux)

### 🔧 File: `/etc/nginx/sites-available/default`

Update the existing `server` block or create a new one:

```nginx
server {
    listen 80;
    server_name localhost;

    location / {
        proxy_pass http://localhost:3000;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

### Breakdown:
- `proxy_pass` → forwards requests to your backend app
- `proxy_set_header` → preserves original request metadata (like IP and host)

---

## 🧪 Demo: Reverse Proxy to a Node.js App

### Step 1: Install Node.js (optional if using your own backend)
```bash
sudo apt update
sudo apt install nodejs npm -y
```

### Step 2: Create a simple backend app
```bash
mkdir ~/node-backend && cd ~/node-backend
nano server.js
```

**Paste this:**
```js
const http = require('http');
http.createServer((req, res) => {
  res.end('Hello from Node.js backend!');
}).listen(3000);
```

Run it:
```bash
node server.js
```

> Your app is now running at `http://localhost:3000`

---

### Step 3: Configure NGINX as reverse proxy

Edit the NGINX default site:
```bash
sudo nano /etc/nginx/sites-available/default
```

Replace the `location / {}` block with:
```nginx
location / {
    proxy_pass http://localhost:3000;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
}
```

### Step 4: Test and reload NGINX

Check config for syntax errors:
```bash
sudo nginx -t
```

Reload NGINX:
```bash
sudo systemctl reload nginx
```

### Step 5: Test in browser

Visit:
```
http://localhost
```

✅ You should see: `Hello from Node.js backend!`

---

## 📁 File Structure Recap (Ubuntu)

| Path                              | Purpose                                |
|-----------------------------------|----------------------------------------|
| `/etc/nginx/nginx.conf`           | Global NGINX settings                   |
| `/etc/nginx/sites-available/default` | Active site config for reverse proxy   |
| `/var/www/html`                   | Not used in reverse proxy               |
| `/var/log/nginx/access.log`       | Logs all requests                       |

---

## ✅ Summary

- NGINX can proxy traffic to backend apps using `proxy_pass`.
- Config changes go in `/etc/nginx/sites-available/default` (on Ubuntu).
- Always test config and reload NGINX after changes.
- Ideal for API gateways, internal routing, and SSL termination.

