
# **Introduction to Nginx**

**Nginx** (pronounced as **"Engine-X"**) is a powerful web server that uses a non-threaded, **event-driven architecture**. It is primarily used to solve performance issues and manage how requests are handled before they reach the main application server (like Node.js or PHP). It is known for its efficiency, scalability, and ability to handle high concurrency with low resource consumption.

---

## **Forward Proxy (The VPN Model)**

![[img/Pasted image 20260117215102.png]]

In a forward proxy, multiple clients send requests to a single proxy server (like a VPN). The proxy then forwards these requests to the internet/server on behalf of the clients.

* **The Server** doesn't know which specific client is asking.

---
## **Key Advantages of Nginx**

### **1. Reverse Proxy (The Nginx Model)**

![[Pasted image 20260117215220.png]]

In a reverse proxy, the client sends a request to Nginx, and Nginx decides which backend server should handle it.

* **The Client** doesn't know which specific backend server resolved the request.
* Nginx acts as an intermediary, providing a layer of security and control.

### **2. Load Balancing**

If you have multiple servers running the same application, Nginx can distribute incoming traffic among them (e.g., using the **Round Robin** algorithm). This ensures no single server is overloaded.

### **3. HTTP Caching**

Nginx can store copies of frequently requested content (like images or videos). When a second user requests the same file, Nginx serves it from its own cache instead of asking the backend server, making the cycle much faster.

### **4. High Concurrency

Can handle **10,000+ concurrent requests** efficiently.

### **5. Static File Serving

It is excellent at serving static files (CSS, JS, Images, Videos) directly without involving Node.js.

### **6. API Gateway

![[Pasted image 20260117225434.png]]

It can route requests based on paths (e.g., `/admin` goes to Server A, while `/settings` goes to Server B).

### **7. SSL Termination

Nginx can handle SSL certificates (HTTPS) so your backend servers don't have to deal with the encryption overhead.

### **8.Rate Limiting**

It can limit the number of requests a user can make in a given period. This is essential for preventing DDoS attacks or protecting your API from being overwhelmed by a single client.

### **9.Compression (Gzip)**

It can compress responses (like HTML or JSON) before sending them to the client. This reduces the amount of data transferred over the network, leading to faster page loads for users.

## **Real-World Example: Google**

* When you visit `google.com/maps`, a reverse proxy (similar to Nginx) detects the `/maps` path and routes you to a specific server dedicated to the Maps application, while `google.com/search` might go elsewhere.

---

# **Nginx Installation and Configuration**

Install _Nginx_ using **apt-get nginx** or **sudo install nginx**
then nginx to run it in default config

## **1. Locating Nginx Files**

When you install Nginx on a Linux machine, the configuration files are stored in a specific system directory.

* **Directory Path:** `/etc/nginx`
* **Important Files:**
  * `nginx.conf`: The main configuration file that defines how the server behaves.
  * `sites-available` / `sites-enabled`: Folders typically used to manage different website configurations.

## **2. The `nginx.conf` File**

This is the heart of Nginx. It uses a simple, block-based syntax to define server behavior.

* **Comments:** Use the `#` symbol.
* **Directives:** Instructions that end with a semicolon `;`.
* **Blocks:** Grouped settings defined by curly braces `{ }`.

### **Configuration Structure**

* **Events Block:** Handles connection processing (e.g., `events { }`).
* **HTTP Block:** The main container for all web-related configurations.
* **Server Block:** Defines a specific virtual server (can have multiple server blocks for different ports or domains).
* **Location Block:** Routes requests based on the URL path (similar to routes in Express.js).

## **3. Practical Implementation (Step-by-Step)**

### **Step A: Creating a Backup**

Before editing, always create a backup of the original configuration:

```Bash
mv nginx.conf nginx.conf.backup
```

### **Step B: Writing a Custom Configuration**

You can use a terminal editor like **Vim** (`vim nginx.conf`) to write a basic server:

```Nginx
events {}

http {
    server {
        listen 80; # Tells Nginx to listen for traffic on port 80.
        server_name _; # A wildcard that catches all incoming requests.
        
        location / { # root location/route
            return 200 "Hello from Nginx Configuration File";
        }
    }
}
```

### **Step C: Reloading Nginx**

After every change to the configuration file, you must reload Nginx for the changes to take effect:

```Bash
nginx -s reload
```

## **4. Verifying the Result**

Once reloaded, visiting the server's IP or `localhost` will show the plain text response. You can verify this in the browser's **Network Tab**:

* **Status Code:** 200 OK
* **Content-Type:** text/plain
* **Response:** "Hello from Nginx Configuration File"

---

# **Serving Static Files with Nginx

<https://docs.nginx.com/nginx/admin-guide/web-server/serving-static-content/>

## **1. Introduction to Static Content**

Nginx is highly efficient at serving **static files** (content that doesn't change based on user input).

* **Examples of Static Files:** `index.html`, JavaScript files (`.js`), CSS files (`.css`), Images (`.jpg`, `.png`), and Videos (`.mp4`).

* **Why use Nginx?** Using a web server like Nginx to serve these files is faster and allows for better caching compared to using an application server like Node.js.

---

## **2. The `root` Directive**

The `root` directive tells Nginx where the website files are stored on the server's hard drive.

### **Implementation Steps:**

1. **Create a Folder:** Create a directory within your file system for your website (e.g., `/etc/nginx/website`).
![[Pasted image 20260118041207.png]]

2. **Add an Index File:** Place an `index.html` file inside that folder. Nginx automatically looks for `index.html` as the default file to serve.

 ```html
 <html>
    <head>
        <title> My Nginx website</title>
    </head>
    <body>
        <h1> Hello from Nginx </h1>
        <p> this is sample Nginx Webpage</p>
    </body>
</html>
 ```

1. **Configure Nginx:** Update the `nginx.conf` file to point to this path.

**Example Configuration:**

```Nginx
http {
    server {
        listen 8080;
        server_name _;
        
        location / {
            root /etc/nginx/website;
        }
    }
}
```

OR

```Nginx
http {
    server {
        listen 8080;
        server_name _;
        
        root /etc/nginx/website;
    }
}
```

> **Note:** Always use the **Absolute Path** (full path from the root directory) to avoid "404 Not Found" errors.

---

## **3. The Importance of MIME Types**

Nginx is smart, but by default, it might treat every file (except HTML) as **plain text** (`text/plain`).
![[Pasted image 20260118042329.png]]

### **The Problem:**

If you link a `style.css` file, Nginx might send it with the wrong `Content-Type`. If the browser receives CSS as `text/plain`, it might not render the styles correctly.

```css
body{
 background-color: #ff0000;
}
```

then add below snipped in the head tag

```html
<link rel="stylesheet" href="style.css"/>
```

then reload with nginx -s reload
![[Pasted image 20260118043120.png]]
![[Pasted image 20260118043238.png]]
![[Pasted image 20260118043346.png]]

![[Pasted image 20260118043557.png]]

then
![[Pasted image 20260118043728.png]]

but since add our own types{} in config file, it lost its default html detection
![[Pasted image 20260118044243.png]]
then we would have to edit the types to

```nginx
types{
 text/css css;
 text/html html;
}
```

you may want to use below comm to check for syntax and testing of config file

```bash
nignx -t
```

![[Pasted image 20260118044013.png]]

### **The Solution: `mime.types`**

Instead of manually defining types for every file extension, Nginx includes a file called `mime.types` that contains a pre-coded list of all popular file formats.

How to include it:

Use the include directive inside the http block. This is similar to an import statement in C++ or Java.

```Nginx
http {
    include /etc/nginx/mime.types; # Automatically maps .css, .js, .jpg, etc.
    
    server {
        listen 8080;
        location / {
            root /etc/nginx/website;
        }
    }
}
```

---

## **4. Advanced Routing for Static Content**

You can define different root folders for different types of files using multiple `location` blocks.

* **Path-based:** Route all requests starting with `/images/` to a specific media folder.

* **Extension-based:** Route all `.mp3` or `.mp4` requests to a high-bandwidth storage directory.

**Example:**

```Nginx
location /images/ {
    root /www/data; # Looks for files in /www/data/images/
}

location ~ \.(mp3|mp4)$ {
    root /www/media; # Specifically handles media files
}
```

---

## **5. Summary of Useful Commands**

|**Command**|**Purpose**|
|---|---|
|`nginx -t`|**Test** the configuration file for syntax errors.|
|`nginx -s reload`|**Reload** Nginx to apply changes without stopping the server.|
|`ls -l`|View file permissions and sizes in the directory.|

---

## Logs

* **Access Logs:** `/var/log/nginx/access.log`
* **Error Logs:** `/var/log/nginx/error.log`

**Official Documentation:** [Nginx Docs](https://nginx.org/en/docs/)
