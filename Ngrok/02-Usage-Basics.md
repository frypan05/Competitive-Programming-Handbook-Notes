# 02 - Usage Basics

This file explains day-to-day commands and workflows.

---

## Basic HTTP tunnel

Expose local port 3000:

```bash
ngrok http 3000
```

Equivalent:

```bash
ngrok http http://localhost:3000
```

---

## Share local frontend quickly

1. Start frontend (`localhost:5173`, `3000`, etc.)
2. Run:

```bash
ngrok http 5173
```

3. Share HTTPS URL with teammates/client

---

## Expose local API

If backend runs on 8080:

```bash
ngrok http 8080
```

Use public URL + your route paths (example: `/api/users`).

---

## Inspect traffic

ngrok provides request inspection UI/API depending on client setup.

This helps you:

- debug headers/body quickly
- replay webhook requests
- see response status and latency

Use this for debugging callback failures instead of guessing.

---

## Use custom host header (when needed)

Some apps rely on host routing. Example:

```bash
ngrok http --host-header=rewrite 3000
```

or specific host:

```bash
ngrok http --host-header=localhost:3000 3000
```

---

## TCP tunneling (non-HTTP services)

Expose SSH-like TCP service:

```bash
ngrok tcp 22
```

You get host:port endpoint from ngrok to reach local TCP service.

---

## Running named tunnels from config

```bash
ngrok start app
```

Run all configured tunnels:

```bash
ngrok start --all
```

---

## Common developer patterns

1. **Backend local + frontend local**: expose backend only; frontend uses local API URL.
2. **Webhook testing**: expose callback port and paste ngrok URL in provider dashboard.
3. **Quick demo**: share URL for instant review without deployment pipeline.

---

## Good operating habits

- Start app first, then ngrok
- Share only HTTPS URL
- Stop tunnels when not needed
- Rotate or regenerate endpoints for sensitive demos

