# CI/CD Pipeline Setup Documentation

## Overview

This repository uses a sequential CI/CD pipeline where:
1. **Validation checks run first** (GitHub Actions)
2. **Only if validation passes**, deployment to Vercel is triggered
3. **Content index generation** runs independently without triggering deployments

## Pipeline Architecture

```
Push to main/master
    ↓
┌─────────────────────────────────────┐
│   Validation Job (validate)         │
│   - Check for files with spaces     │
│   - Validate file encodings         │
│   - Other quality checks            │
└─────────────────────────────────────┘
    ↓
    ✅ All checks pass
    ↓
┌─────────────────────────────────────┐
│   Deploy Job (deploy)               │
│   - Depends on: validate            │
│   - Only runs on push to main       │
│   - Triggers Vercel deployment      │
└─────────────────────────────────────┘
```

## Workflows

### 1. Validate and Deploy (`validate-and-deploy.yml`)

**Triggers:**
- Push to `main` or `master` branch
- Pull requests to `main` or `master` branch

**Jobs:**

#### `validate` Job
- Runs on every push and PR
- Checks for files with spaces in their names
- Fails the entire pipeline if validation fails
- Provides detailed error messages in GitHub Actions summary

**Example output when files with spaces are found:**
```
❌ ERROR: Found files with spaces in their names:
./path/to/file with spaces.txt
./another/file name.md
```

#### `deploy` Job
- Only runs if `validate` job succeeds
- Only triggers on push to `main`/`master` (not on PRs)
- Integrates with Vercel for deployment

### 2. Generate Content Index (`generate-content-index.yml`)

**Triggers:**
- Push to specific paths: `DevOps/**`, `Networking/**`, `Linux/**`, `Nginx/**`
- Pull requests to the same paths
- Manual trigger via `workflow_dispatch`
- Daily schedule at 00:00 UTC

**Features:**
- Commits use `[skip ci]` to prevent triggering other workflows
- Implements retry logic for push conflicts
- Uses concurrency control to prevent parallel runs
- Generates and commits `content-index.json`

## Configuration Files

### `vercel.json`

Disables automatic Vercel deployments from GitHub:

```json
{
  "git": {
    "deploymentEnabled": false
  },
  "github": {
    "enabled": false,
    "autoAlias": false,
    "silent": true,
    "autoJobCancelation": true
  }
}
```

This ensures Vercel only deploys when explicitly triggered by the GitHub Actions workflow after validation passes.

## Validation Rules

### 1. No Spaces in Filenames ✅

**Why?** Spaces in filenames can cause:
- URL encoding issues
- Build system problems
- Deployment failures
- Cross-platform compatibility issues

**How to fix?**
```bash
# Rename files to use hyphens or underscores instead
mv "file with spaces.md" "file-with-spaces.md"
# or
mv "file with spaces.md" "file_with_spaces.md"
```

### 2. File Encoding Validation ✅

Ensures all files use valid character encodings.

## How to Use

### For Developers

1. **Before pushing code:**
   ```bash
   # Check for files with spaces locally
   find . -type f -name "* *" | grep -v "^\./.git/"
   ```

2. **If files with spaces exist:**
   ```bash
   # Rename them before committing
   mv "old name.txt" "old-name.txt"
   ```

3. **Push your changes:**
   ```bash
   git add .
   git commit -m "Your commit message"
   git push origin main
   ```

4. **Monitor the workflow:**
   - Go to the "Actions" tab in GitHub
   - Watch the validation and deployment jobs
   - If validation fails, fix issues and push again

### For Pull Requests

- Validation checks run automatically on all PRs
- Deployment does NOT run on PRs (only validation)
- All checks must pass before merging

## Troubleshooting

### ❌ Validation fails due to files with spaces

**Problem:** The workflow finds files with spaces in their names.

**Solution:**
1. Check the workflow summary for the list of problematic files
2. Rename each file to remove spaces
3. Commit and push the changes

```bash
# Example: Rename multiple files
find . -type f -name "* *" -not -path "./.git/*" | while read file; do
    newfile=$(echo "$file" | tr ' ' '-')
    git mv "$file" "$newfile"
done

git commit -m "fix: remove spaces from filenames"
git push
```

### ❌ Content index workflow conflicts with deploy workflow

**Solution:** Already handled!
- Content index commits use `[skip ci]` tag
- Concurrency controls prevent parallel runs
- Retry logic handles push conflicts

### ❌ Vercel deploys before validation completes

**Solution:** 
1. Ensure `vercel.json` exists with `deploymentEnabled: false`
2. Configure Vercel dashboard to disable auto-deployments:
   - Go to Project Settings → Git
   - Disable "Automatic Deployments from Git"

### 🔧 Manually trigger workflows

```bash
# From GitHub UI:
# 1. Go to Actions tab
# 2. Select workflow
# 3. Click "Run workflow" button

# Using GitHub CLI:
gh workflow run validate-and-deploy.yml
gh workflow run generate-content-index.yml
```

## Extending the Pipeline

### Add new validation checks

Edit `.github/workflows/validate-and-deploy.yml`:

```yaml
- name: Your new validation check
  run: |
      echo "Running custom validation..."
      # Add your validation logic here
      # Exit with code 1 to fail the check
```
