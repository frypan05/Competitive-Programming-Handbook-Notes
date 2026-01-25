# 🔒 Section 5: SSL/TLS Setup in NGINX Using a Self-Signed Certificate (Ubuntu/Linux)

## 🎯 Goal

Secure your application with HTTPS using a **self-signed SSL certificate**.  
This is ideal for **local development**, **internal tools**, and **non-public test environments**.

---

## 🧠 Why Use HTTPS (Even in Dev)?

- Encrypts traffic between client and server
- Simulates production-like environment for testing
- Helps catch mixed-content issues early
- Required by modern frontend frameworks and APIs

---

## 🛠️ Step-by-Step: Create a Self-Signed Certificate

### Step 1: Generate SSL Certificate and Key

```bash
sudo openssl req -x509 -nodes -days 365 \
 -newkey rsa:2048 \
 -keyout /etc/ssl/private/nginx-selfsigned.key \
 -out /etc/ssl/certs/nginx-selfsigned.crt
```

When prompted:
- Common Name (CN): use `localhost` or your server’s IP

---

### Step 2: Update NGINX Configuration

Edit the default site config:
```bash
sudo nano /etc/nginx/sites-available/default
```

Replace with the following:

```nginx
server {
    listen 443 ssl;
    server_name localhost;

    ssl_certificate /etc/ssl/certs/nginx-selfsigned.crt;
    ssl_certificate_key /etc/ssl/private/nginx-selfsigned.key;

    location / {
        proxy_pass http://localhost:3000;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}

# Optional: Redirect HTTP to HTTPS
server {
    listen 80;
    server_name localhost;
    return 301 https://$host$request_uri;
}
```

---

### Step 3: Reload NGINX

Check and reload configuration:

```bash
sudo nginx -t
sudo systemctl reload nginx
```

---

### Step 4: Test HTTPS Locally

Open your browser and visit:

```
https://localhost
```

⚠️ You will see a warning:  
> "Your connection is not private"

✅ That’s expected with self-signed certs. Proceed anyway to view your site securely.

---

## 📁 SSL File Paths Recap

| Path                                         | Purpose               |
|----------------------------------------------|------------------------|
| `/etc/ssl/certs/nginx-selfsigned.crt`        | SSL certificate        |
| `/etc/ssl/private/nginx-selfsigned.key`      | Private key            |
| `/etc/nginx/sites-available/default`         | HTTPS proxy config     |

---

## 🧪 Bonus: Test Without Browser (curl)

```bash
curl -k https://localhost
```

> `-k` allows insecure (self-signed) HTTPS connections.

---

## ✅ Summary

- Self-signed SSL is perfect for secure local development.
- Requires only OpenSSL and a few lines in NGINX.
- Always test your HTTPS setup with curl and browser.
- In production, switch to Let’s Encrypt or trusted CAs.

---
