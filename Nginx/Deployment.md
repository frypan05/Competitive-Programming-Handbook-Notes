Node.js Deployment - NGINX, SSL With Lets Encrypt
<https://gist.github.com/piyushgarg-dev/8b14c87c8ff4d626ecbc747b6b9fc57f#file-nginx_aws-md>

# Node.js Deployment

> Steps to deploy a Node.js app to DigitalOcean using PM2, NGINX as a reverse proxy and an SSL from LetsEncrypt

## 1. Create Free AWS Account

Create free AWS Account at [https://aws.amazon.com/](https://aws.amazon.com/)

## 2. Create and Lauch an EC2 instance and SSH into machine

I would be creating a t2.medium ubuntu machine for this demo.

## 3. Install Node and NPM

```shell
curl -sL https://deb.nodesource.com/setup_18.x | sudo -E bash -
sudo apt install nodejs

node --version
```

## 4. Clone your project from Github

```shell
git clone https://github.com/piyushgargdev-01/short-url-nodejs
```

## 5. Install dependencies and test app

```shell
sudo npm i pm2 -g
pm2 start index

# Other pm2 commands
pm2 show app
pm2 status
pm2 restart app
pm2 stop app
pm2 logs (Show log stream)
pm2 flush (Clear logs)

# To make sure app starts when reboot
pm2 startup ubuntu
```

## 6. Setup Firewall

```shell
sudo ufw enable
sudo ufw status
sudo ufw allow ssh (Port 22)
sudo ufw allow http (Port 80)
sudo ufw allow https (Port 443)
```

## 7. Install NGINX and configure

```shell
sudo apt install nginx

sudo nano /etc/nginx/sites-available/default
```

Add the following to the location part of the server block

```shell
    server_name yourdomain.com www.yourdomain.com;
    
    location / {
        proxy_pass http://localhost:8001; #whatever port your app runs on
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection 'upgrade';
        proxy_set_header Host $host;
        proxy_cache_bypass $http_upgrade;
    }
```

```shell
# Check NGINX config
sudo nginx -t

# Restart NGINX
sudo nginx -s reload
```

## 8. Add SSL with LetsEncrypt

We need to enable the HTTPS port 443 in the AWS security group so that it is accessible to the outside world.
port 443 which is default for https and port 80(Nginx) is http only

```shell
sudo add-apt-repository ppa:certbot/certbot
sudo apt-get update
sudo apt-get install python3-certbot-nginx
sudo certbot --nginx -d yourdomain.com -d www.yourdomain.com

# Only valid for 90 days, test the renewal process with
certbot renew --dry-run
```

---
if working in other browser but not in chrome,
your port is blocked on firewall allow port 80 on firewall "sudo ufw allow 80/tcp", "sudo ufw allow 443/tcp" it will work

TBD - EOD
