# 00 - Introduction to ngrok

## What is ngrok?

ngrok is a tunneling tool that gives your local app a temporary public URL.

If your app runs on `localhost:3000`, ngrok can expose it as:

`https://random-id.ngrok-free.app`

Anyone with that URL can access your local app through ngrok's relay.

---

## Why localhost is private by default

Your machine usually sits behind:

- a router (NAT)
- firewall rules
- private IP addressing (`192.168.x.x`, `10.x.x.x`)

This means inbound internet traffic cannot directly reach your laptop/server unless you configure port forwarding or reverse proxy infrastructure.

---

## How ngrok works (reverse tunnel model)

1. You start your local app (example: `localhost:3000`).
2. You run ngrok on your machine.
3. ngrok client opens an **outbound** connection to ngrok cloud.
4. ngrok cloud creates a public URL.
5. External user requests hit ngrok cloud first.
6. ngrok cloud forwards that traffic through the existing tunnel to your local app.
7. Response returns via the same path back to the user.

Because the connection is initiated outbound from your machine, you typically do not need router/firewall inbound changes.

---

## Ngrok vs domain + hosting (core confusion cleared)

These are different products:

| Topic | ngrok | Domain + Hosting |
|---|---|---|
| Main purpose | Temporary exposure of local/private service | Permanent internet presence |
| Where app runs | Your machine or private network | Cloud/server infrastructure |
| URL type | Usually random subdomain under ngrok domain | Your custom domain (e.g., `brand.com`) |
| Uptime | Depends on your machine and tunnel session | Designed for 24/7 availability |
| Production suitability | Mostly dev/test/internal workflows | Yes, for public production apps |

---

## Why ngrok can offer free public URLs

1. ngrok owns parent domains like `ngrok-free.app`.
2. It can issue random subdomains under its own domain at low marginal cost.
3. It does **not** host your app code in free tunnel usage; it only relays traffic.
4. Free tier is a freemium funnel; advanced features are paid.

---

## Typical real-world use cases

- Webhook testing (Stripe, Razorpay, GitHub, Slack, Twilio, etc.)
- Demoing local builds to teammates/clients
- Testing OAuth redirect flows locally
- Mobile device testing against local backend
- Temporary secure access to internal/dev service

---

## Important limitations

- If your laptop sleeps, tunnel breaks.
- If internet disconnects, URL becomes unusable.
- Free URLs may change between sessions.
- Bandwidth/performance limits depend on plan.
- Exposing sensitive local services can be risky.

---

## Key takeaway

Use ngrok as a **development bridge**, not as your final production hosting strategy.

