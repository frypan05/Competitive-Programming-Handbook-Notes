#!/usr/bin/env bash
set -euo pipefail

# Basic placeholder checks for the OpsProject static blog.
# Expand this script as you add more validations (HTML lint, link check, etc).

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SITE_DIR="${ROOT_DIR}/site"

pass()  { echo "[OK] $*"; }
fail()  { echo "[FAIL] $*" >&2; exit 1; }

# Check required files exist
[ -f "${SITE_DIR}/index.html" ] && pass "Found site/index.html" || fail "Missing site/index.html"
[ -f "${SITE_DIR}/styles.css" ] && pass "Found site/styles.css"   || fail "Missing site/styles.css"

# Simple HTML sanity checks (presence of a <title>)
if grep -qi "<title>" "${SITE_DIR}/index.html"; then
  pass "index.html has a <title>"
else
  fail "index.html is missing a <title> tag"
fi

# Check Dockerfile exists
[ -f "${ROOT_DIR}/Dockerfile" ] && pass "Found Dockerfile" || fail "Missing Dockerfile"

# Optional: check shell scripts are executable
for script in "${ROOT_DIR}"/scripts/*.sh; do
  if [ ! -x "$script" ]; then
    chmod +x "$script"
    pass "Fixed exec bit: $(basename "$script")"
  else
    pass "Executable: $(basename "$script")"
  fi
done

echo "All basic checks passed."
