# 05 - Free vs Paid and When to Use ngrok

This guide helps you decide where ngrok fits in your stack.

---

## Free ngrok is great for

- learning and prototyping
- local demos
- webhook testing
- OAuth callback testing
- short-term collaboration

---

## Paid features are usually for

- stable/predictable domains
- team governance and access controls
- stronger auth/policy/compliance needs
- higher limits/reliability expectations
- enterprise workflows

---

## ngrok vs traditional hosting

| Requirement | ngrok | Domain + Hosting |
|---|---|---|
| Quick local sharing | Excellent | Overkill |
| 24/7 production uptime | Not ideal | Required |
| Custom brand URL | Limited/plan-dependent | Standard |
| Scalability for many users | Limited by your local setup | Designed for scale |
| Deployment permanence | Temporary | Permanent |

---

## Decision framework

Use ngrok when:

- you are still developing
- you need external callbacks to local machine
- speed matters more than permanence

Use hosting + custom domain when:

- users depend on uptime
- you need brand trust/professional identity
- traffic volume and reliability matter
- compliance/security controls must be formalized

---

## Final takeaway

ngrok is a **developer acceleration tool**, not a full replacement for production infrastructure.

Best practice:

- develop and iterate with ngrok
- deploy production builds to proper hosting
- map your custom domain to production environment

