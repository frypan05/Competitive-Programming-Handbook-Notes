#!/usr/bin/env bash
set -euo pipefail

# Run the static blog container locally.
# Defaults align with build.sh/Jenkinsfile.
# Env overrides:
#   IMAGE_NAME (default: ops-blog)
#   IMAGE_TAG  (default: local)
#   PORT       (default: 8080)

IMAGE_NAME="${IMAGE_NAME:-ops-blog}"
IMAGE_TAG="${IMAGE_TAG:-local}"
PORT="${PORT:-8080}"

echo "Starting container ${IMAGE_NAME}:${IMAGE_TAG} on http://localhost:${PORT}"

CONTAINER_ID="$(
  docker run -d \
    -p "${PORT}:80" \
    --name "ops-blog-dev-${PORT}" \
    "${IMAGE_NAME}:${IMAGE_TAG}"
)"

echo "Container ID: ${CONTAINER_ID}"
echo "View the site at: http://localhost:${PORT}"
echo "To stop: docker rm -f ${CONTAINER_ID}"
