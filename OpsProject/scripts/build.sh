#!/usr/bin/env bash
set -euo pipefail

# Build the Docker image for the static blog.
# Defaults align with Jenkinsfile: IMAGE_NAME=ops-blog, IMAGE_TAG=local
# Override via env: IMAGE_NAME, IMAGE_TAG, BUILD_CONTEXT, DOCKERFILE

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE_NAME="${IMAGE_NAME:-ops-blog}"
IMAGE_TAG="${IMAGE_TAG:-local}"
BUILD_CONTEXT="${BUILD_CONTEXT:-${ROOT_DIR}}"
DOCKERFILE="${DOCKERFILE:-${ROOT_DIR}/Dockerfile}"

echo "Building image: ${IMAGE_NAME}:${IMAGE_TAG}"
echo "Context: ${BUILD_CONTEXT}"
echo "Dockerfile: ${DOCKERFILE}"

docker build \
  -t "${IMAGE_NAME}:${IMAGE_TAG}" \
  -f "${DOCKERFILE}" \
  "${BUILD_CONTEXT}"

echo "Built ${IMAGE_NAME}:${IMAGE_TAG}"
