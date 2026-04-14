#!/usr/bin/env python3
"""
Generate content index from DevOps subdirectories, Networking, and Linux directories.
This script scans markdown files and creates a structured JSON index.
DevOps subdirectories (Ansible, Docker, Kubernetes, etc.) are treated as separate categories.
"""

import json
import os
import re
from datetime import datetime
from pathlib import Path
from typing import Any, Dict, List

# Base directory - adjust as needed
BASE_DIR = Path(__file__).resolve().parent.parent.parent

# Directories to scan - DevOps subdirectories are treated as separate categories
CONTENT_DIRS = {
    "Ansible": BASE_DIR / "DevOps" / "Ansible",
    "Docker": BASE_DIR / "DevOps" / "Docker",
    "Kubernetes": BASE_DIR / "DevOps" / "Kubernetes",
    "Terraform": BASE_DIR / "DevOps" / "Terraform",
    "AWS": BASE_DIR / "DevOps" / "Cloud" / "AWS",
    "Jenkins": BASE_DIR / "DevOps" / "jenkins-df-.sh",
    "Github Actions": BASE_DIR / "DevOps" / "Github-Actions",
    "Networking": BASE_DIR / "Networking",
    "Linux": BASE_DIR / "Linux",
    "Nginx": BASE_DIR / "Nginx",
    "Origin": BASE_DIR / "Origin",
    "Scripting Questionaire": BASE_DIR / "Scripting-Questionaire",
    "APIs": BASE_DIR / "APIs",
}

# Also scan root DevOps directory for general content
DEVOPS_ROOT = BASE_DIR / "DevOps"

# Output file
OUTPUT_FILE = BASE_DIR / "OpsProject" / "site" / "content-index.json"


def extract_frontmatter(content: str) -> Dict[str, Any]:
    """Extract frontmatter from markdown content if present."""
    frontmatter = {}
    if content.startswith("---"):
        parts = content.split("---", 2)
        if len(parts) >= 3:
            fm_content = parts[1].strip()
            for line in fm_content.split("\n"):
                if ":" in line:
                    key, value = line.split(":", 1)
                    frontmatter[key.strip()] = value.strip()
    return frontmatter


def extract_title_from_content(content: str) -> str:
    """Extract title from markdown content (first H1 header)."""
    lines = content.split("\n")
    for line in lines:
        if line.startswith("# "):
            return line[2:].strip()
    return ""


def extract_summary(content: str, max_length: int = 200) -> str:
    """Extract a summary from the content."""
    # Remove frontmatter
    if content.startswith("---"):
        parts = content.split("---", 2)
        if len(parts) >= 3:
            content = parts[2]

    # Remove headers
    lines = content.split("\n")
    text_lines = []
    for line in lines:
        line = line.strip()
        if line and not line.startswith("#") and not line.startswith("```"):
            text_lines.append(line)
        if len(" ".join(text_lines)) > max_length:
            break

    summary = " ".join(text_lines)
    if len(summary) > max_length:
        summary = summary[:max_length].rsplit(" ", 1)[0] + "..."
    return summary


def create_slug(text: str) -> str:
    """Create a URL-friendly slug from text."""
    slug = text.lower()
    slug = re.sub(r"[^\w\s-]", "", slug)
    slug = re.sub(r"[-\s]+", "-", slug)
    return slug.strip("-")


def get_relative_path(full_path: Path, base_path: Path) -> str:
    """Get relative path from base directory."""
    try:
        return str(full_path.relative_to(base_path))
    except ValueError:
        return str(full_path)


def scan_directory(
    dir_path: Path, category: str, base_dir: Path | None = None
) -> List[Dict[str, Any]]:
    """Scan a directory for markdown files and extract metadata."""
    items = []

    if not dir_path.exists():
        print(f"Warning: Directory {dir_path} does not exist")
        return items

    if base_dir is None:
        base_dir = dir_path

    # Find all markdown files
    md_files = list(dir_path.rglob("*.md"))

    for md_file in md_files:
        try:
            with open(md_file, "r", encoding="utf-8") as f:
                content = f.read()

            # Extract metadata
            frontmatter = extract_frontmatter(content)
            title = frontmatter.get("title") or extract_title_from_content(content)

            # Skip if no title found
            if not title:
                # Use filename as fallback
                title = md_file.stem.replace("-", " ").replace("_", " ").title()
                if title.lower() == "readme":
                    # Use parent directory name for README files
                    title = f"{md_file.parent.name.replace('-', ' ').replace('_', ' ').title()}"

            # Create slug from path
            rel_path = get_relative_path(md_file, base_dir)
            slug = create_slug(f"{category}-{rel_path}")

            # Extract summary
            summary = frontmatter.get("description") or extract_summary(content)

            # Get subcategory from directory structure
            subcategory = ""
            try:
                # Get the immediate parent directory relative to the category base
                relative_parent = md_file.parent.relative_to(base_dir)
                if relative_parent != Path("."):
                    # Use the first subdirectory as subcategory
                    subcategory = str(relative_parent).split(os.sep)[0]
            except ValueError:
                parent_dir = md_file.parent.name
                if parent_dir != dir_path.name:
                    subcategory = parent_dir

            item = {
                "slug": slug,
                "title": title,
                "category": category,
                "subcategory": subcategory,
                "summary": summary,
                "body": content,
                "path": get_relative_path(md_file, BASE_DIR),
                "tags": frontmatter.get("tags", "").split(",")
                if frontmatter.get("tags")
                else [],
                "updated_at": datetime.fromtimestamp(
                    md_file.stat().st_mtime
                ).isoformat(),
            }

            items.append(item)
            print(
                f"Processed: {title} ({category}/{subcategory if subcategory else 'root'})"
            )

        except Exception as e:
            print(f"Error processing {md_file}: {e}")

    return items


def scan_devops_root() -> List[Dict[str, Any]]:
    """Scan the root DevOps directory for general markdown files (non-recursive into subdirs)."""
    items = []

    if not DEVOPS_ROOT.exists():
        print(f"Warning: DevOps root directory {DEVOPS_ROOT} does not exist")
        return items

    # Only get markdown files directly in the DevOps root (not recursive)
    md_files = [f for f in DEVOPS_ROOT.glob("*.md")]

    for md_file in md_files:
        try:
            with open(md_file, "r", encoding="utf-8") as f:
                content = f.read()

            frontmatter = extract_frontmatter(content)
            title = frontmatter.get("title") or extract_title_from_content(content)

            if not title:
                title = md_file.stem.replace("-", " ").replace("_", " ").title()

            slug = create_slug(f"devops-{md_file.stem}")
            summary = frontmatter.get("description") or extract_summary(content)

            item = {
                "slug": slug,
                "title": title,
                "category": "DevOps",
                "subcategory": "",
                "summary": summary,
                "body": content,
                "path": get_relative_path(md_file, BASE_DIR),
                "tags": frontmatter.get("tags", "").split(",")
                if frontmatter.get("tags")
                else [],
                "updated_at": datetime.fromtimestamp(
                    md_file.stat().st_mtime
                ).isoformat(),
            }

            items.append(item)
            print(f"Processed: {title} (DevOps/root)")

        except Exception as e:
            print(f"Error processing {md_file}: {e}")

    return items


def generate_categories_summary(items: List[Dict[str, Any]]) -> Dict[str, Any]:
    """Generate category summaries."""
    categories = {}

    for item in items:
        cat = item["category"]
        if cat not in categories:
            categories[cat] = {"name": cat, "count": 0, "subcategories": {}}

        categories[cat]["count"] += 1

        subcat = item.get("subcategory", "")
        if subcat:
            if subcat not in categories[cat]["subcategories"]:
                categories[cat]["subcategories"][subcat] = 0
            categories[cat]["subcategories"][subcat] += 1

    return categories


def main():
    """Main function to generate content index."""
    print("Starting content index generation...")
    print(f"Base directory: {BASE_DIR}")

    all_items = []

    # Scan DevOps root directory for general files
    print(f"\nScanning DevOps root directory: {DEVOPS_ROOT}")
    devops_root_items = scan_devops_root()
    all_items.extend(devops_root_items)
    print(f"Found {len(devops_root_items)} items in DevOps root")

    # Scan each content directory (DevOps subdirectories and other top-level dirs)
    for category, dir_path in CONTENT_DIRS.items():
        print(f"\nScanning {category} directory: {dir_path}")
        items = scan_directory(dir_path, category, dir_path)
        all_items.extend(items)
        print(f"Found {len(items)} items in {category}")

    # Generate categories summary
    categories = generate_categories_summary(all_items)

    # Create output structure
    output = {
        "generated_at": datetime.now().isoformat(),
        "total_items": len(all_items),
        "categories": categories,
        "items": all_items,
    }

    # Ensure output directory exists
    OUTPUT_FILE.parent.mkdir(parents=True, exist_ok=True)

    # Write to file
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        json.dump(output, f, indent=2, ensure_ascii=False)

    print(f"\n Content index generated successfully!")
    print(f" Output: {OUTPUT_FILE}")
    print(f" Total items: {len(all_items)}")
    print(f" Categories: {', '.join(sorted(categories.keys()))}")
    print(f"\n Category breakdown:")
    for cat_name in sorted(categories.keys()):
        cat_data = categories[cat_name]
        print(f"   • {cat_name}: {cat_data['count']} items")
        if cat_data["subcategories"]:
            for subcat, count in sorted(cat_data["subcategories"].items()):
                print(f"      - {subcat}: {count} items")


if __name__ == "__main__":
    main()
