Node.js Deployment - NGINX, SSL With Lets Encrypt
<https://gist.github.com/piyushgarg-dev/8b14c87c8ff4d626ecbc747b6b9fc57f#file-nginx_aws-md>

# Node.js Deployment

> Steps to deploy a Node.js app to DigitalOcean using PM2, NGINX as a reverse proxy and an SSL from LetsEncrypt

## 1. Create Free AWS Account

Create free AWS Account at [https://aws.amazon.com/](https://aws.amazon.com/)

## 2. Create and Lauch an EC2 instance and SSH into machine

I would be creating a t2.medium ubuntu machine for this demo.

![](img/Pasted%20image%2020260118205822.png)
![](img/Pasted%20image%2020260118205625.png)
![](img/Pasted%20image%2020260118210003.png)
![](img/Pasted%20image%2020260118210113.png)

next up get a free domain from <https://www.freenom.com/en/index.html>
OR <https://freedomain.one>
whichever works for you
![](img/Pasted%20image%2020260118231034.png)

normal ec2 ip changes when we restart it, create an elastic IP at default config
![](img/Pasted%20image%2020260118212920.png)

then associate elastic ip
![](img/Pasted%20image%2020260118212950.png)
![](img/Pasted%20image%2020260118212815.png)

the public ipv4 of server wont change now ![](img/Pasted%20image%2020260118213107.png)

---
if you are on windows and dont want to ssh into the server you can just connect as shown w/o changing the config
![](img/Pasted%20image%2020260118213329.png)

and you will be redirected to ubuntu shell
![](img/Pasted%20image%2020260118213510.png)

OR (for linux/mac)

SSH using your .pem key
![](img/Pasted%20image%2020260118213656.png)

![](img/Pasted%20image%2020260118214026.png)

then do **sudo apt-get update** to update the instance

## 3. Install Node and NPM

```shell
curl -sL https://deb.nodesource.com/setup_18.x | sudo -E bash -
sudo apt install nodejs

node --version
```

paste the above comm as it is it , you will get deprecation warning but thats fine

## 4. Clone your project from Github

```shell
git clone https://github.com/piyushgargdev-01/short-url-nodejs
```

![](img/Pasted%20image%2020260118214952.png)
then **npm install** within the folder

## 5. Install dependencies and test app

below comm will run outside the project dir , next we setup a **process manager** so that we can run the app in background

```shell
sudo npm i pm2 -g
```

rest of these comm will run in /short-url-nodejs dir

```shell
pm2 start index.js
```

![](img/Pasted%20image%2020260118221849.png)

you would also need to setup a cluster in mongodb atlas
![](img/Pasted%20image%2020260118224545.png)

then in terminal

```shell
mongodb+srv://<db_username>:<db_password>@short-url-nginx.olkueuk.mongodb.net/?appName=short-url-nginx
```

then run

```shell
pm2 restart all
```

![](img/Pasted%20image%2020260118224735.png)

---
there seems to be an issue with the index.js not being able to pickup the env var of MONGODB -->do **vim index.js**

**Not a good Practice** but put the env var in the the func **connectToMongoDB**

![](img/Pasted%20image%2020260118225344.png)

do **pm2 restart all**
![](img/Pasted%20image%2020260118225559.png)

then **pm2 logs**
![](img/Pasted%20image%2020260118225627.png)

**MongoDB is connected now :D**

```shell

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

next up we need to edit inbound rules at aws since port access is disabled rn
![](img/Pasted%20image%2020260118230015.png)
![](img/Pasted%20image%2020260118230130.png)

after selecting the right security group id we will edit inbound rules
![](img/Pasted%20image%2020260118230503.png)
we added two rules for 8001 , one for anywhere IPV4 , another for anywhere IPV6
then save rules and go the <public_ip_of_instance>:8001
![](img/Pasted%20image%2020260118230726.png)

works!!
currently we access it through ip + port and it doesnt have ssl certification

![](img/Pasted%20image%2020260118231318.png)

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
