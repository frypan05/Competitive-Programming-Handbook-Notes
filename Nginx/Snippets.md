## Reverse Proxy with Nginx

```nginx
server {
    listen 80;
    server_name example.com;
    
    location / {
        proxy_pass http://localhost:5000;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

## Load Balancing with Nginx

```nginx
upstream backend {
    server backend1.example.com;
    server backend2.example.com;
}

server {
    listen 80;
    server_name example.com;
    
    location / {
        proxy_pass http://backend;
    }
}
```

## Enabling SSL with Let's Encrypt

```sh
sudo apt install certbot python3-certbot-nginx
sudo certbot --nginx -d example.com -d www.example.com
```

## Managing Nginx Service

```sh
sudo systemctl start nginx   # Start Nginx
sudo systemctl stop nginx    # Stop Nginx
sudo systemctl restart nginx # Restart Nginx
sudo systemctl reload nginx  # Reload configuration
```

## Checking Nginx Status

```sh
sudo systemctl status nginx
```

