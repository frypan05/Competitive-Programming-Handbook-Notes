# ⚖️ Section 4: Load Balancing with NGINX (Ubuntu/Linux)

## 🎯 Goal

Use NGINX to distribute traffic across multiple backend servers — improving availability, reliability, and scalability of your applications.

---

## 🧠 What is Load Balancing?

**Load balancing** is the process of distributing incoming network traffic across multiple backend servers.

Benefits:
- Prevents server overload
- Increases availability and fault tolerance
- Enables horizontal scaling

NGINX supports multiple load balancing algorithms out of the box.

---

## 🧮 Load Balancing Algorithms in NGINX

| Algorithm         | Behavior                                                               |
|-------------------|-------------------------------------------------------------------------|
| `round-robin`     | Default — rotates through all backends equally                         |
| `least_conn`      | Sends traffic to the backend with the fewest active connections         |
| `ip_hash`         | Uses client IP to consistently route requests to the same backend       |

---

## 📝 Basic Load Balancer Configuration

Edit:
```bash
sudo nano /etc/nginx/sites-available/default
```

Replace contents with:

```nginx
upstream backend_app {
    server 127.0.0.1:3001;
    server 127.0.0.1:3002;
}

server {
    listen 80;
    server_name localhost;

    location / {
        proxy_pass http://backend_app;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

---

## 🧪 Demo: Load Balance Two Local Backend Servers

### Step 1: Create Backend Servers

We'll run two simple HTTP servers using Node.js.

#### Create script:
```bash
mkdir ~/load-test && cd ~/load-test
```

**server1.js**
```js
require('http').createServer((req, res) => {
  res.end('Response from Server 1');
}).listen(3001);
```

**server2.js**
```js
require('http').createServer((req, res) => {
  res.end('Response from Server 2');
}).listen(3002);
```

### Step 2: Run both servers
```bash
node server1.js &
node server2.js &
```

---

### Step 3: Reload NGINX

```bash
sudo nginx -t
sudo systemctl reload nginx
```

### Step 4: Test the Load Balancer

Open a browser or use curl:

```bash
curl http://localhost
```

Run it multiple times — you should see the response alternate between:

```
Response from Server 1
Response from Server 2
```

✅ You’ve just created a working load balancer using NGINX!

---

## 🔄 Switching Load Balancing Methods

### Use Least Connections
```nginx
upstream backend_app {
    least_conn;
    server 127.0.0.1:3001;
    server 127.0.0.1:3002;
}
```

### Use IP Hash
```nginx
upstream backend_app {
    ip_hash;
    server 127.0.0.1:3001;
    server 127.0.0.1:3002;
}
```

---


