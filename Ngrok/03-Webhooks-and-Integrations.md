# 03 - Webhooks and Integrations

Webhook testing is one of ngrok's most valuable use cases.

---

## Why webhooks need ngrok

Providers (Stripe, GitHub, Razorpay, Twilio, etc.) send events to a **public** URL.  
`localhost` is not public, so they cannot call your machine directly.

ngrok gives you a public callback URL that forwards to your local webhook endpoint.

---

## Generic webhook testing flow

1. Run your app locally (example: `localhost:3000`)
2. Start tunnel:

```bash
ngrok http 3000
```

3. Copy forwarding HTTPS URL
4. Set webhook endpoint in provider dashboard:

`https://<ngrok-id>.ngrok-free.app/webhooks/provider`

5. Trigger test event from provider
6. Inspect request in ngrok inspector and app logs

---

## Example endpoint mappings

| Local route | Public route via ngrok |
|---|---|
| `http://localhost:3000/webhooks/stripe` | `https://<id>.ngrok-free.app/webhooks/stripe` |
| `http://localhost:3000/api/github/webhook` | `https://<id>.ngrok-free.app/api/github/webhook` |
| `http://localhost:8080/callback` | `https://<id>.ngrok-free.app/callback` |

---

## Signature verification still applies

Do **not** skip verification just because traffic comes through ngrok.

For webhook security:

- verify provider signature/header
- reject invalid timestamps/signatures
- return proper status codes
- log event IDs to avoid duplicate processing

ngrok is a transport relay, not a trust guarantee.

---

## OAuth redirect testing locally

Many OAuth providers require a public redirect URI.

With ngrok:

1. Expose local auth server
2. Set redirect URI in provider dashboard to ngrok URL
3. Complete login flow end-to-end locally

Example redirect:

`https://<id>.ngrok-free.app/auth/callback`

---

## Debugging webhook issues fast

When callbacks fail:

1. Confirm provider is hitting the correct ngrok URL
2. Ensure local route path matches exactly
3. Check app returns `2xx` for successful processing
4. Review request body/headers in inspector
5. Confirm secret/signature logic is correct

---

## Practical caution

If your free ngrok URL changes, provider webhooks still point to old URL and fail.  
Update webhook endpoint each time unless you use a stable domain feature/plan.

