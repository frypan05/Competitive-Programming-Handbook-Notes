# DevOps Knowledge Base Site

A modern, static site that automatically indexes and displays your DevOps learning materials from the repository. Built with vanilla HTML/CSS/JavaScript and automated with GitHub Actions.

## 🎯 Overview

This project transforms your markdown notes from `DevOps/`, `Networking/`, `Linux/`, and `Nginx/` directories into a searchable, browsable knowledge base website.

## ✨ Features

- **Automatic Content Indexing**: CI/CD pipeline scans directories and generates a searchable JSON index
- **Dynamic Content Loading**: No manual JSON file creation needed
- **Search & Filter**: Real-time search and category filtering
- **Modal Reading**: Click any item to read the full content in a toast modal
- **Zero Build Step**: Pure HTML/CSS/JS - deploy anywhere
- **GitHub Actions Integration**: Automatically updates content index on every push

## 📁 Project Structure

```
OpsProject/
├── site/
│   ├── index.html           # Main website
│   ├── styles.css           # Styling
│   ├── posts.json           # Hand-curated featured posts
│   └── content-index.json   # Auto-generated content index (gitignored)
├── scripts/
│   └── generate-index.py    # Content indexing script
└── README.md
```

## 🚀 Quick Start

### 1. Generate Content Index Locally

```bash
# From repository root
python OpsProject/scripts/generate-index.py
```

This will:
- Scan `DevOps/`, `Networking/`, `Linux/`, and `Nginx/` directories
- Extract metadata from markdown files
- Generate `OpsProject/site/content-index.json`

### 2. View the Site

Open `OpsProject/site/index.html` in your browser or serve it locally:

```bash
cd OpsProject/site
python -m http.server 8000
# Visit http://localhost:8000
```

## 🔄 CI/CD Automation

The GitHub Actions workflow (`.github/workflows/generate-content-index.yml`) automatically:

1. **Triggers on**:
   - Push to `main`/`master` branch
   - Changes in `DevOps/`, `Networking/`, `Linux/`, or `Nginx/` directories
   - Manual workflow dispatch
   - Daily at midnight UTC (scheduled)

2. **Process**:
   - Checks out the repository
   - Runs the Python indexing script
   - Commits updated `content-index.json` if changed
   - Uploads artifact for review

3. **Setup**:
   - No additional setup required
   - Uses Python 3.11
   - Zero dependencies (pure stdlib)

## 📝 Content Structure

### What Gets Indexed

The script scans for markdown files (`.md`) and extracts:

- **Title**: From frontmatter or first H1 header
- **Summary**: First ~200 characters of content
- **Category**: `DevOps`, `Networking`, or `Linux`
- **Subcategory**: Parent directory name
- **Body**: Full markdown content
- **Path**: Relative path in repository
- **Tags**: From frontmatter (if present)
- **Updated Date**: File modification timestamp

### Example Markdown Structure

```markdown
---
title: My Custom Title
tags: docker, kubernetes, ci-cd
---

# Main Title

Your content here...
```

## 🎨 Adding Custom Content

### Featured Posts

Edit `site/posts.json` to add hand-curated featured posts:

```json
[
  {
    "slug": "my-post",
    "title": "My Featured Post",
    "year": "2025",
    "body": "Post content goes here..."
  }
]
```

These appear in the "Selected Writings" section.

### Styling

Modify `site/styles.css` to customize:
- Color scheme (CSS variables in `:root`)
- Layout and spacing
- Toast modal appearance
- Filter buttons

## 🔧 Configuration

### Customize Indexed Directories

Edit `scripts/generate-index.py`:

```python
CONTENT_DIRS = {
    "DevOps": BASE_DIR / "DevOps",
    "Networking": BASE_DIR / "Networking",
    "Linux": BASE_DIR / "Linux",
    # Add more directories here
}
```

### Change Output Location

```python
OUTPUT_FILE = BASE_DIR / "OpsProject" / "site" / "content-index.json"
```

## 🌐 Deployment

### GitHub Pages

1. Enable GitHub Pages in repository settings
2. Set source to `main` branch, `/OpsProject/site` folder
3. Content auto-updates via GitHub Actions

### Netlify

```toml
# netlify.toml
[build]
  publish = "OpsProject/site"
  command = "python OpsProject/scripts/generate-index.py"
```

### Vercel

```json
{
  "buildCommand": "python OpsProject/scripts/generate-index.py",
  "outputDirectory": "OpsProject/site"
}
```

### Static Hosting (Nginx, Apache, etc.)

Simply copy the `OpsProject/site/` directory to your web server and run the script before deploying:

```bash
python OpsProject/scripts/generate-index.py
rsync -avz OpsProject/site/ user@server:/var/www/html/
```

## 📊 Content Index Format

The generated `content-index.json` has this structure:

```json
{
  "generated_at": "2025-01-28T12:00:00",
  "total_items": 42,
  "categories": {
    "DevOps": {
      "name": "DevOps",
      "count": 15,
      "subcategories": {
        "Docker": 5,
        "Kubernetes": 10
      }
    }
  },
  "items": [
    {
      "slug": "unique-slug",
      "title": "Content Title",
      "category": "DevOps",
      "subcategory": "Docker",
      "summary": "Brief summary...",
      "body": "Full markdown content...",
      "path": "DevOps/Docker/README.md",
      "tags": ["docker", "containers"],
      "updated_at": "2025-01-28T10:30:00"
    }
  ]
}
```

## 🛠️ Troubleshooting

### Content not showing up?

1. Check if `content-index.json` exists in `site/` folder
2. Run the script manually: `python OpsProject/scripts/generate-index.py`
3. Check browser console for errors (F12)
4. Verify markdown files have proper titles (H1 or frontmatter)

### GitHub Actions not running?

1. Check workflow permissions in repository settings
2. Verify workflow file is in `.github/workflows/`
3. Check Actions tab for errors
4. Ensure branch name matches (main/master)

### Search not working?

1. Content must be loaded from `content-index.json`
2. Check browser console for fetch errors
3. If running locally, use `python -m http.server` (not `file://`)

## 🎯 Future Enhancements

- [ ] Add markdown rendering library (marked.js) for better formatting
- [ ] Implement pagination for large content sets
- [ ] Add tags/topics filtering
- [ ] Dark/light theme toggle
- [ ] Export content as PDF
- [ ] Full-text search with fuzzy matching
- [ ] Reading time estimates
- [ ] Recently updated section

## 📄 License

MIT License - Use this however you like!

## 🤝 Contributing

This is a personal learning resource, but feel free to:
- Fork and adapt for your own notes
- Submit PRs for bug fixes
- Share improvements


**Built with ❤️ for DevOps learners**
