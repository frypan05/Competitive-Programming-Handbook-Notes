#!/usr/bin/env python3
"""
Seed script to generate sample blog posts for the OpsProject static site.

Usage:
  python3 scripts/seed.py
  python3 scripts/seed.py --count 5 --out site/posts.json

This writes a small JSON file consumed by front-end JS (if you add it), or
you can use the generated data to manually embed posts into HTML.
"""

import argparse
import json
import random
import textwrap
from datetime import datetime, timedelta
from pathlib import Path

DEFAULT_POSTS = [
    {
        "title": "Intro to Bash for DevOps",
        "slug": "intro-bash-devops",
        "summary": "Learn shell basics, variables, conditionals, and loops.",
        "body": "Bash is the glue for automation. Start with shebang, set -euo pipefail, and iterate safely.",
        "tags": ["bash", "shell", "devops"],
    },
    {
        "title": "Text Processing with grep, awk, and sed",
        "slug": "text-processing-cli",
        "summary": "Find, filter, and transform logs efficiently.",
        "body": "grep narrows, sed edits, awk aggregates. Combine them to slice logs quickly.",
        "tags": ["linux", "logs", "scripting"],
    },
    {
        "title": "Writing Idempotent Shell Scripts",
        "slug": "idempotent-shell",
        "summary": "Avoid double-applying changes in provisioning scripts.",
        "body": "Check before create, use traps for cleanup, and design scripts to handle reruns without side effects.",
        "tags": ["bash", "infra", "patterns"],
    },
    {
        "title": "Automating Health Checks with curl and jq",
        "slug": "health-checks-curl-jq",
        "summary": "Smoke test services with simple CLI tools.",
        "body": "Use curl -f to fail on bad HTTP and jq to validate JSON keys for quick readiness checks.",
        "tags": ["cli", "monitoring", "bash"],
    },
    {
        "title": "Small Python Utilities for Ops",
        "slug": "python-ops-utils",
        "summary": "When Bash gets messy, reach for Python scripts.",
        "body": "Argparse + requests + pathlib cover many automation cases. Keep dependencies minimal for portability.",
        "tags": ["python", "automation", "devops"],
    },
]


def synthesize_posts(count: int) -> list[dict]:
    posts = []
    now = datetime.utcnow()
    base = DEFAULT_POSTS.copy()
    random.shuffle(base)
    # Repeat and slightly vary summaries if more than defaults requested
    while len(base) < count:
        base.append(
            {
                "title": f"Linux Tip #{len(base) + 1}",
                "slug": f"linux-tip-{len(base) + 1}",
                "summary": "Quick shell one-liner for daily ops.",
                "body": "Use `set -euo pipefail` in scripts and quote your variables.",
                "tags": ["linux", "tips"],
            }
        )

    for i in range(count):
        p = base[i]
        posts.append(
            {
                "title": p["title"],
                "slug": p["slug"],
                "summary": p["summary"],
                "body": textwrap.shorten(p["body"], width=240, placeholder="..."),
                "tags": p["tags"],
                "published_at": (now - timedelta(days=i)).isoformat() + "Z",
            }
        )
    return posts


def main():
    parser = argparse.ArgumentParser(description="Seed sample blog posts JSON.")
    parser.add_argument(
        "--count", type=int, default=5, help="Number of posts to generate."
    )
    parser.add_argument(
        "--out",
        type=Path,
        default=Path("site/posts.json"),
        help="Output file path.",
    )
    args = parser.parse_args()

    posts = synthesize_posts(max(1, args.count))
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps({"posts": posts}, indent=2))
    print(f"Wrote {len(posts)} posts to {args.out}")


if __name__ == "__main__":
    main()
