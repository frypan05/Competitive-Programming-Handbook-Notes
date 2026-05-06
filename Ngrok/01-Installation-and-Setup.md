# 01 - Installation and Setup

This guide covers first-time setup from zero.

---

## Prerequisites

- A local app running on a known port (example: `3000`)
- Internet access
- ngrok account (free is fine)

---

## Step 1: Create ngrok account

1. Sign up at [https://ngrok.com](https://ngrok.com)
2. Verify email/login
3. Open dashboard and copy your **authtoken**

The authtoken links your local ngrok client to your account.

---

## Step 2: Install ngrok

Pick one method for your OS.

### Windows

- Download zip from ngrok website, extract binary
- Or use package manager:

```powershell
winget install Ngrok.Ngrok
```

### macOS

```bash
brew install ngrok/ngrok/ngrok
```

### Linux

Use official install docs/package for your distro, or download binary:

```bash
unzip ngrok-v3-stable-linux-amd64.zip
sudo mv ngrok /usr/local/bin
```

---

## Step 3: Authenticate client

Run once after install:

```bash
ngrok config add-authtoken <YOUR_AUTHTOKEN>
```

This stores credentials in ngrok config.

---

## Step 4: Verify installation

```bash
ngrok version
```

If this prints a version, install is successful.

---

## Step 5: Start your local app

Example:

```bash
npm run dev
```

Assume app is now on `http://localhost:3000`.

---

## Step 6: Create first tunnel

```bash
ngrok http 3000
```

You will see:

- Forwarding URL (HTTPS + maybe HTTP)
- Local service target (`http://localhost:3000`)
- Live request logs/statistics

Use the HTTPS forwarding URL to access your local app publicly.

---

## Optional: Stable setup with config file

Create reusable named tunnels in config file.

Example config (`~/.config/ngrok/ngrok.yml` on Linux/macOS):

```yaml
version: 2
tunnels:
  app:
    proto: http
    addr: 3000
  api:
    proto: http
    addr: 8080
```

Run named tunnel:

```bash
ngrok start app
```

Run multiple:

```bash
ngrok start app api
```

---

## First-time troubleshooting

| Symptom | Likely cause | Fix |
|---|---|---|
| `command not found: ngrok` | Binary not installed/in PATH | Reinstall or add to PATH |
| URL opens but app fails | Wrong local port | Check your app port and rerun tunnel |
| Auth error | Missing/invalid authtoken | Re-add token from dashboard |
| Tunnel disconnects | Internet/app process stopped | Restart app and tunnel |

