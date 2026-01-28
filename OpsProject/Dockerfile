# syntax=docker/dockerfile:1

FROM nginx:stable-alpine

# Copy static site into nginx html directory
COPY site/ /usr/share/nginx/html/

# Optional: basic security hardening of default config
# Uncomment if you add a custom nginx.conf
# COPY nginx.conf /etc/nginx/nginx.conf

EXPOSE 80

# Simple healthcheck to ensure nginx responds
HEALTHCHECK --interval=30s --timeout=3s --retries=3 CMD wget -qO- http://localhost/ || exit 1

# Use the default nginx entrypoint/cmd
