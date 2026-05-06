# 04 - Security Best Practices

ngrok makes local services public. Treat it carefully.

---

## Core rule

**Expose only what is necessary, only for as long as necessary.**

---

## What not to expose

- local databases with no auth
- internal admin panels
- debug endpoints (`/debug`, `/actuator`, `/metrics`) unless protected
- file systems/dev tools bound insecurely

---

## Minimum safety checklist

1. Require authentication on sensitive routes.
2. Validate and sanitize all external input.
3. Verify webhook signatures.
4. Use HTTPS URLs only.
5. Shut down tunnel when done.

---

## Scope down exposure

- Tunnel only the required app port.
- Prefer exposing one service rather than many.
- Keep endpoint paths narrow for temporary demos.

---

## Handle secrets correctly

- Never put API keys/tokens in client-side responses.
- Use environment variables.
- Rotate compromised secrets immediately.
- Do not share screenshots containing sensitive headers/tokens.

---

## Logging and monitoring

During tunnel sessions:

- monitor request logs for suspicious calls
- watch for brute-force patterns
- quickly stop tunnel if unexpected traffic appears

---

## Team usage guidance

When sharing tunnel URLs in team chats:

- mention expected lifetime
- mention intended audience
- revoke/stop after review session

Treat tunnel URLs as temporary privileged access links.

