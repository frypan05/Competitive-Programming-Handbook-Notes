cat > "Origin/11-Git-Deep-Dive.md" << 'GITEOF'
# 11 — Git Deep Dive

---

## SECTION 1 — How Git Works Internally

**Q: What is Git's data model?**

Git is a content-addressable filesystem. Everything is stored as objects identified by SHA-1 hashes.

```
blob   — raw file contents. A blob is just the content, no filename.
         Two files with identical content share the SAME blob object.

tree   — a directory listing. Maps filenames to blob hashes (files) or other tree hashes
         (subdirectories). Each tree represents a snapshot of one directory.

commit — a snapshot of the entire project. Contains:
           - A pointer to the root tree object (the project state)
           - Author name/email/timestamp
           - Committer name/email/timestamp
           - Commit message
           - Pointer(s) to parent commit(s) (0 for initial, 1 normally, 2+ for merges)

tag    — a named pointer to a specific commit (annotated tags also store a message + tagger).
```

These live in `.git/objects/`. Every object is immutable — you can never change an object,
only create new ones. This is what makes Git safe: you can always recover previous states.

---

**Q: What is a branch in Git?**

A: A branch is simply a file containing a 40-character commit SHA. It's a lightweight, movable
pointer to a commit. When you make a new commit on a branch, the branch pointer moves forward.

```
.git/refs/heads/main     — contains the SHA of the latest commit on main
.git/refs/heads/develop  — contains the SHA of the latest commit on develop
.git/HEAD                — contains "ref: refs/heads/main" (points to current branch)
```

Creating a branch is O(1) — just write a 40-byte file. This is why Git branching is cheap.

---

**Q: What is HEAD?**

A: HEAD is a pointer to the current branch reference (or a specific commit in detached state).

```
Normal state:    HEAD -> refs/heads/main -> commit abc123
                 (HEAD points to a branch, branch points to a commit)

Detached HEAD:   HEAD -> commit abc123
                 (HEAD points directly to a commit, not a branch)
                 This happens when you: checkout a tag, checkout a specific SHA,
                 checkout a remote branch without creating a local one.
                 New commits in detached HEAD can be lost if you switch branches
                 without creating a branch or tag.
```

---

**Q: What is the staging area (index)?**

A: An intermediate area between your working directory and the repository. When you `git add`,
files are copied to the staging area. When you `git commit`, the staging area becomes the new
commit snapshot. This allows you to craft commits carefully — you don't have to commit everything.

```
Working Directory  -->  git add  -->  Staging Area (Index)  -->  git commit  -->  Repository
       ^                                                                            |
       |-------------------------- git checkout -----------------------------------|

The index file lives at: .git/index (binary format)
```

---

**Q: What is the reflog?**

A: A local log of every change to HEAD and branch tips. Even if you hard reset, rebase, or
amend, the reflog keeps a record for ~90 days. It is your safety net.

```bash
git reflog                           # show HEAD movements
git reflog show main                 # show main branch movements

# Recover a "lost" commit after hard reset
git reflog
# abc1234 HEAD@{2}: commit: implement feature X
git checkout -b recovered abc1234    # create branch at the lost commit
# or
git reset --hard abc1234             # move current branch back to it
```

The reflog is LOCAL only — not shared between machines or pushed to remotes.

---

## SECTION 2 — Branching Strategies

**Q: Explain Git Flow.**

```
main (or master):
  - Production-ready code. Every commit is a release.
  - Only receives merges from release and hotfix branches.
  - Tagged with version numbers.

develop:
  - Integration branch. Contains the latest delivered features.
  - Receives merges from feature branches.

feature/<name>:
  - Branch from: develop
  - Merge back to: develop
  - One feature = one branch.

release/<version>:
  - Branch from: develop when ready for release.
  - Only bug fixes, no new features.
  - Merge to: main AND develop when released.

hotfix/<name>:
  - Branch from: main (fixes critical production bug).
  - Merge to: main AND develop.
```

---

**Q: Explain Trunk-Based Development.**

```
All developers commit to a single branch (main/trunk) frequently (at least daily).
Short-lived feature branches (< 2 days) are allowed.

Key practices:
  - Feature flags to hide incomplete work behind toggles.
  - Small, incremental commits — never a massive merge.
  - CI runs on every commit to main — must always be green.
  - Release from main directly (or cut a release branch at release time).

Advantages over Git Flow:
  - Simpler workflow, fewer merge conflicts.
  - Faster feedback loop.
  - CI/CD friendly — main is always deployable.
  - Preferred by: Google, Facebook, Netflix, most modern SaaS companies.

Disadvantages:
  - Requires strong CI/CD and test coverage.
  - Requires discipline (no broken commits on main).
  - Feature flags add complexity.
```

---

**Q: GitHub Flow (the practical middle ground).**

```
1. main branch is always deployable.
2. Create a feature branch from main.
3. Make commits on the feature branch.
4. Open a Pull Request.
5. Code review and CI checks pass.
6. Merge to main.
7. Deploy from main.

Simpler than Git Flow. Good for web apps with continuous deployment.
```

---

## SECTION 3 — Merge vs Rebase

**Q: What is the difference between merge and rebase?**

```
git merge:
  - Creates a new MERGE COMMIT that has two parents.
  - Preserves the complete history and branch structure.
  - History is non-linear — you can see when branches diverged and converged.
  - Safe: never rewrites history.

    A---B---C  (main)
         \
          D---E  (feature)

    After: git checkout main && git merge feature

    A---B---C---M  (main)     M = merge commit with parents C and E
         \     /
          D---E  (feature)


git rebase:
  - Replays your branch's commits ON TOP of the target branch.
  - Creates NEW commit objects (different SHAs) — original commits are abandoned.
  - History is linear — looks like all development happened sequentially.
  - REWRITES HISTORY: never rebase commits that have been pushed and shared!

    A---B---C  (main)
         \
          D---E  (feature)

    After: git checkout feature && git rebase main

    A---B---C  (main)
             \
              D'---E'  (feature)     D' and E' are new commits (different SHA)
```

**When to use each:**
```
Use MERGE:
  - On shared/public branches (main, develop)
  - When you want a clear record of branch integration
  - For pull request merges (the default)

Use REBASE:
  - To keep your feature branch up to date with main (before opening a PR)
  - To clean up local history before pushing
  - Interactive rebase to squash/reorder commits before sharing

GOLDEN RULE: NEVER rebase commits that exist on a remote and others have pulled.
             Rebase rewrites SHAs. Other developers' copies will diverge and break.
```

---

**Q: What is interactive rebase and when do you use it?**

```bash
git rebase -i HEAD~5    # interactively rewrite the last 5 commits

# Opens an editor:
pick abc1234 Add user model
pick def5678 Fix typo in user model
pick ghi9012 Add user controller
pick jkl3456 WIP save
pick mno7890 Finish user feature

# Change to:
pick abc1234 Add user model
fixup def5678 Fix typo in user model       # squash into previous, discard message
pick ghi9012 Add user controller
squash jkl3456 WIP save                    # squash into previous, keep message for editing
reword mno7890 Finish user feature         # change commit message

# Commands:
# pick   — use commit as-is
# reword — use commit but edit the message
# edit   — use commit but pause for amending (change files)
# squash — meld into previous commit, combine messages
# fixup  — meld into previous commit, discard this commit's message
# drop   — remove this commit entirely
```

**Use before creating a PR:** clean up "WIP", "fix typo", "oops" commits into logical units.

---

## SECTION 4 — Merge Conflicts

**Q: How do you resolve a merge conflict?**

```bash
# During merge or rebase, Git tells you which files conflict
git status     # shows "both modified" files

# The conflicted file contains markers:
<<<<<<< HEAD
code from current branch (ours)
=======
code from incoming branch (theirs)
>>>>>>> feature-branch

# Resolution:
# 1. Edit the file — remove markers, keep the correct code.
# 2. Stage the resolved file:
git add <resolved-file>
# 3. Continue:
git merge --continue     # if during merge
git rebase --continue    # if during rebase

# Abort (cancel the merge/rebase entirely):
git merge --abort
git rebase --abort
```

---

**Q: How do you resolve with "ours" or "theirs" strategy?**

```bash
# Accept ALL changes from one side for a specific file:
git checkout --ours   <file>     # keep current branch version
git checkout --theirs <file>     # keep incoming branch version
git add <file>

# For the entire merge:
git merge feature --strategy-option ours      # prefer current branch on conflicts
git merge feature --strategy-option theirs    # prefer incoming branch on conflicts

# During rebase (REVERSED — because rebase replays YOUR commits onto target):
# "ours"   = the branch you're rebasing ONTO (e.g., main)
# "theirs" = the commits being replayed (your feature branch)
```

---

## SECTION 5 — Essential Git Commands (Know Every One)

### Day-to-Day Workflow

```bash
# Clone
git clone https://github.com/user/repo.git
git clone --depth 1 https://github.com/user/repo.git    # shallow clone (CI/CD)
git clone --branch develop https://github.com/user/repo.git

# Branching
git branch                           # list local branches
git branch -a                        # list all (local + remote)
git branch feature/auth              # create branch (don't switch)
git checkout -b feature/auth         # create and switch
git switch -c feature/auth           # modern equivalent of checkout -b

git branch -d feature/auth           # delete (safe — only if merged)
git branch -D feature/auth           # force delete (even if unmerged)
git push origin --delete feature/auth  # delete remote branch

# Switching
git checkout main
git switch main                      # modern equivalent

# Staging
git add file.py                      # stage specific file
git add .                            # stage all changes in current dir and below
git add -p                           # interactive staging — pick individual hunks
git add -A                           # stage all changes in entire working tree
git reset HEAD file.py               # unstage a file (keep changes in working dir)
git restore --staged file.py         # modern equivalent of reset HEAD

# Committing
git commit -m "Add feature"
git commit -am "Quick fix"           # add all tracked + commit (skips untracked)
git commit --amend                   # rewrite the last commit (message + content)
git commit --amend --no-edit         # amend without changing message

# Pushing / Pulling
git push origin main
git push -u origin feature/auth      # push and set upstream (first time)
git push --force-with-lease          # safer force push (fails if remote has new commits)

git pull                             # fetch + merge (default)
git pull --rebase                    # fetch + rebase (cleaner history)
git fetch                            # download remote changes without merging
git fetch --prune                    # clean up deleted remote branches

# Status / Diff
git status                           # working tree status
git status -s                        # short format
git diff                             # unstaged changes
git diff --staged                    # staged changes (what will be committed)
git diff main..feature               # difference between two branches
git diff HEAD~3                      # changes in last 3 commits
```

---

### History & Inspection

```bash
git log --oneline --graph --all      # visual branch graph
git log --oneline -20                # last 20 commits, one line each
git log --author="Daksh"             # filter by author
git log --since="2024-01-01" --until="2024-02-01"
git log -- path/to/file.py          # history of a specific file
git log -p -- path/to/file.py       # history with diffs
git log --grep="fix"                # search commit messages

git show abc1234                     # show a specific commit
git show abc1234:path/to/file.py    # show file at a specific commit

git blame path/to/file.py           # who changed each line, when
git blame -L 10,20 file.py          # blame lines 10-20 only

git shortlog -sn --all              # contributor commit counts
```

---

### Stash

```bash
git stash                            # stash uncommitted changes (tracked files)
git stash -u                         # include untracked files
git stash -m "WIP: auth feature"     # with description
git stash list                       # show all stashes
git stash show -p stash@{0}          # show diff of a stash
git stash pop                        # apply latest stash and remove from stash list
git stash apply stash@{2}            # apply specific stash (keep in list)
git stash drop stash@{0}             # delete a specific stash
git stash clear                      # delete ALL stashes

# Typical use: you're on feature branch, need to switch to main for a hotfix
git stash -u -m "WIP: halfway done"
git checkout main
# ... do hotfix work ...
git checkout feature/auth
git stash pop
```

---

### Cherry-Pick

```bash
# Apply a specific commit from another branch onto your current branch
git cherry-pick abc1234
git cherry-pick abc1234 def5678      # multiple commits
git cherry-pick abc1234 --no-commit  # apply changes without committing (stage only)

# Use case: backport a bug fix from develop to a release branch
git checkout release/1.5
git cherry-pick abc1234              # the specific fix commit from develop
```

---

### Bisect (Find the Commit That Introduced a Bug)

```bash
git bisect start
git bisect bad                       # current commit is broken
git bisect good v1.0.0               # this tag/commit was working

# Git checks out a commit in the middle. You test it:
# If the bug is present:
git bisect bad
# If the bug is NOT present:
git bisect good

# Repeat. Git uses binary search. In ~7 steps you can search 128 commits.
# When found:
# abc1234 is the first bad commit

git bisect reset                     # go back to where you started

# Automated bisect (if you have a test script):
git bisect start HEAD v1.0.0
git bisect run ./test-script.sh      # exits 0 = good, exits 1 = bad
```

---

## SECTION 6 — Undoing Things

**Q: How do you undo changes in Git? Explain all methods.**

```bash
# ── Discard unstaged changes ──
git checkout -- file.py              # restore file to last committed state
git restore file.py                  # modern equivalent
git checkout -- .                    # discard ALL unstaged changes

# ── Unstage files ──
git reset HEAD file.py               # unstage but keep changes in working dir
git restore --staged file.py         # modern equivalent

# ── Undo the last commit (keep changes) ──
git reset --soft HEAD~1              # move branch pointer back. Changes stay STAGED.
git reset --mixed HEAD~1             # (default) move back. Changes stay UNSTAGED.
git reset --hard HEAD~1              # move back. Changes are DELETED. Dangerous.

# ── Revert a commit (safe for shared branches) ──
git revert abc1234                   # creates a NEW commit that undoes abc1234
git revert abc1234 --no-commit       # stage the revert without committing
git revert HEAD                      # revert the most recent commit

# When to use each:
# reset --soft:  "I committed too early, want to add more changes"
# reset --mixed: "I committed and staged wrong things, redo from scratch"
# reset --hard:  "Throw everything away and go back" (check reflog if you regret)
# revert:        "Undo a commit on a shared branch without rewriting history"
```

---

**Q: What is the difference between reset and revert?**

```
git reset:
  - MOVES the branch pointer backward. Commits after the reset point are "lost"
    (still in reflog for ~90 days).
  - REWRITES history. Only safe on local/unpushed commits.
  - The unwanted commits disappear from the branch.

git revert:
  - Creates a NEW commit that undoes the changes of a specified commit.
  - Does NOT rewrite history. Safe for shared/pushed branches.
  - The original commit still exists in history.
  - Preferred for production branches (main, develop).
```

---

## SECTION 7 — Tags

```bash
# Lightweight tag (just a pointer, no metadata)
git tag v1.0.0

# Annotated tag (has tagger, date, message — use these for releases)
git tag -a v1.0.0 -m "Release version 1.0.0"

# Tag a specific older commit
git tag -a v0.9.0 abc1234 -m "Retroactive tag"

# List tags
git tag                              # all tags
git tag -l "v1.*"                    # matching pattern

# Push tags to remote
git push origin v1.0.0              # push one tag
git push origin --tags              # push all tags

# Delete a tag
git tag -d v1.0.0                   # delete locally
git push origin --delete v1.0.0     # delete from remote

# Checkout a tag (creates detached HEAD)
git checkout v1.0.0
# Better: create a branch from a tag
git checkout -b hotfix/1.0.1 v1.0.0
```

---

## SECTION 8 — Git Hooks

**Q: What are Git hooks and how are they used in DevOps?**

A: Scripts that run automatically at specific points in the Git workflow. They live in
`.git/hooks/` and are local (not pushed to the remote by default).

```
Client-side hooks:
  pre-commit     — runs before a commit is created.
                   Use for: lint, format check, prevent large files, run unit tests.
                   Exit non-zero to abort the commit.

  commit-msg     — runs after the commit message is entered.
                   Use for: enforce message format (e.g., "JIRA-123: description").

  pre-push       — runs before push. Use for: run tests before pushing.

  prepare-commit-msg — runs before editor opens. Auto-populate message templates.

Server-side hooks:
  pre-receive    — runs on the remote before accepting a push.
                   Use for: enforce branch naming, block force pushes, require signed commits.

  update         — like pre-receive but runs once per branch being updated.

  post-receive   — runs after a push is accepted.
                   Use for: trigger CI/CD, send notifications, deploy.
```

---

**Q: How do you share hooks across a team?**

```bash
# Option 1: Store hooks in the repo and configure Git to use them
mkdir -p .githooks
# Place hook scripts in .githooks/

# In the repo root, set the hooks path:
git config core.hooksPath .githooks

# Or add to .gitconfig of the project:
# [core]
#     hooksPath = .githooks

# Option 2: Use a tool like Husky (Node.js) or pre-commit (Python)
# pre-commit framework (most popular):
# .pre-commit-config.yaml
repos:
  - repo: https://github.com/pre-commit/pre-commit-hooks
    rev: v4.5.0
    hooks:
      - id: trailing-whitespace
      - id: end-of-file-fixer
      - id: check-yaml
      - id: check-added-large-files
        args: ['--maxkb=500']
  - repo: https://github.com/psf/black
    rev: 24.1.1
    hooks:
      - id: black
  - repo: https://github.com/PyCQA/flake8
    rev: 7.0.0
    hooks:
      - id: flake8

# Install: pre-commit install
# Now pre-commit runs automatically on every git commit.
```

---

### Example: pre-commit hook — prevent secrets from being committed

```bash
#!/bin/bash
# .githooks/pre-commit

set -euo pipefail

echo "Running pre-commit checks..."

# Check for hardcoded secrets
FORBIDDEN_PATTERNS=(
    'AWS_SECRET_ACCESS_KEY'
    'PRIVATE_KEY'
    'password\s*='
    'api_key\s*='
    'BEGIN RSA PRIVATE KEY'
)

for pattern in "${FORBIDDEN_PATTERNS[@]}"; do
    if git diff --cached --diff-filter=d | grep -iE "$pattern" > /dev/null 2>&1; then
        echo "ERROR: Potential secret found matching pattern: $pattern"
        echo "Please remove it before committing."
        exit 1
    fi
done

# Check for large files
MAX_SIZE=$((5 * 1024 * 1024))  # 5MB
for file in $(git diff --cached --name-only --diff-filter=d); do
    if [ -f "$file" ]; then
        size=$(wc -c < "$file")
        if [ "$size" -gt "$MAX_SIZE" ]; then
            echo "ERROR: $file is $(( size / 1024 / 1024 ))MB — exceeds 5MB limit."
            echo "Use Git LFS for large files."
            exit 1
        fi
    fi
done

echo "Pre-commit checks passed."
```

---

## SECTION 9 — .gitignore

**Q: How does .gitignore work? What should you ignore?**

```bash
# .gitignore — patterns for files Git should never track

# Dependencies
node_modules/
vendor/
.venv/
__pycache__/
*.pyc

# Build output
dist/
build/
*.o
*.class
*.jar

# Environment / Secrets
.env
.env.local
.env.production
*.pem
*.key

# IDE
.idea/
.vscode/
*.swp
*.swo
*~

# OS
.DS_Store
Thumbs.db

# Docker
docker-compose.override.yml

# Terraform
.terraform/
*.tfstate
*.tfstate.backup
*.tfvars      # if it contains secrets

# Logs
*.log
logs/

# Coverage
coverage/
htmlcov/
.coverage
```

```bash
# Ignore a file that is already tracked:
git rm --cached file.py              # remove from tracking but keep on disk
echo "file.py" >> .gitignore
git commit -m "Stop tracking file.py"

# Global gitignore (for your machine, all repos):
git config --global core.excludesfile ~/.gitignore_global
# Add .DS_Store, .idea/, *.swp to the global file
```

---

## SECTION 10 — Git in CI/CD

**Q: How is Git used in CI/CD pipelines?**

```
1. Pipeline trigger:
   - A push or PR to specific branches triggers the pipeline.
   - CI tool clones the repo (often shallow clone for speed).

2. Shallow clone (saves time and bandwidth):
   git clone --depth 1 --branch main https://github.com/user/repo.git

3. Determining what changed (for selective builds):
   git diff --name-only HEAD~1         # files changed in last commit
   git diff --name-only origin/main    # files changed vs main

4. Tagging releases from CI:
   git tag -a v${VERSION} -m "Release ${VERSION}"
   git push origin v${VERSION}

5. Commit SHA as Docker image tag (immutable, traceable):
   docker build -t myapp:$(git rev-parse --short HEAD) .

6. Git information in build metadata:
   GIT_SHA=$(git rev-parse HEAD)
   GIT_BRANCH=$(git rev-parse --abbrev-ref HEAD)
   GIT_AUTHOR=$(git log -1 --format='%an')
   GIT_MESSAGE=$(git log -1 --format='%s')
```

---

**Q: What is `--force-with-lease` and why should you use it instead of `--force`?**

```bash
git push --force                     # overwrites remote. If someone else pushed, their
                                     # commits are LOST. Extremely dangerous on shared branches.

git push --force-with-lease          # only force-pushes if the remote branch is at the
                                     # commit you THINK it is. If someone else has pushed
                                     # new commits since your last fetch, the push FAILS.
                                     # This prevents accidentally overwriting others' work.

# Use case: after interactive rebase of your own feature branch
git rebase -i HEAD~5
git push --force-with-lease origin feature/auth
```

---

## SECTION 11 — Git Submodules and Subtrees

**Q: What are Git submodules?**

A: A way to include one Git repository inside another. The parent repo stores a reference
(commit SHA) to a specific commit in the submodule repo.

```bash
# Add a submodule
git submodule add https://github.com/org/shared-lib.git libs/shared

# Clone a repo with submodules
git clone --recurse-submodules https://github.com/org/main-app.git
# Or after cloning:
git submodule update --init --recursive

# Update submodule to latest
cd libs/shared
git checkout main && git pull
cd ../..
git add libs/shared
git commit -m "Update shared-lib submodule to latest"

# Problems with submodules:
# - Easy to forget to init/update after clone
# - CI must use --recurse-submodules
# - Detached HEAD inside submodule is confusing
# - Updates require explicit commit in the parent repo
```

---

## SECTION 12 — Practical Interview Questions

**Q: You accidentally committed a secret (API key) to a public repo. What do you do?**

```
IMMEDIATELY:
1. Rotate the secret — assume it is compromised. Generate a new API key.
   This is the FIRST thing you do. Everything else is secondary.

2. Remove from history using git filter-branch or BFG Repo-Cleaner:
   # BFG is faster and simpler
   java -jar bfg.jar --replace-text passwords.txt repo.git
   git reflog expire --expire=now --all
   git gc --prune=now --aggressive
   git push --force --all
   git push --force --tags

3. Force push to overwrite the remote history.

4. Tell teammates to re-clone (their local copies still have the secret).

5. Add the file/pattern to .gitignore.

6. Install pre-commit hooks to prevent this from happening again.

7. Use a secrets manager (Vault, AWS Secrets Manager) instead of files.

IMPORTANT: Even if you remove the commit, GitHub caches old commits.
           GitHub Support can help with cached data removal.
           Always assume the secret is compromised and rotate it.
```

---

**Q: Your teammate pushed to the wrong branch (main instead of feature). How do you fix it?**

```bash
# Step 1: Identify the bad commits
git log main --oneline -5

# Step 2: Revert on main (SAFE — doesn't rewrite shared history)
git checkout main
git revert abc1234 def5678           # revert each bad commit
git push origin main

# Step 3: Cherry-pick or re-apply those commits to the correct branch
git checkout -b feature/auth
git cherry-pick abc1234 def5678
git push -u origin feature/auth

# Alternative if nobody else has pulled main yet (and you're confident):
git checkout main
git reset --hard HEAD~2              # remove last 2 commits
git push --force-with-lease origin main
# Then cherry-pick to the feature branch
```

---

**Q: How do you squash all commits in a PR into one before merging?**

```bash
# Method 1: GitHub/GitLab UI — "Squash and merge" button

# Method 2: Interactive rebase before pushing
git checkout feature/auth
git rebase -i main
# Mark all commits except the first as "squash" or "fixup"

# Method 3: Soft reset
git checkout feature/auth
git reset --soft main                # move branch pointer to main, keep all changes staged
git commit -m "feat: implement auth module"
git push --force-with-lease origin feature/auth
```

---

**Q: How do you keep a long-running feature branch up to date with main?**

```bash
# Method 1: Rebase (preferred — clean linear history)
git checkout feature/auth
git fetch origin
git rebase origin/main
# Resolve any conflicts
git push --force-with-lease origin feature/auth

# Method 2: Merge main into feature (simpler but messier history)
git checkout feature/auth
git merge origin/main
git push origin feature/auth

# Rebase is preferred because:
# - The PR will show only YOUR changes, not merge commits
# - History is linear and easy to review
# - Bisect works better on linear history
```

---

**Q: Explain `git fetch` vs `git pull`.**

```
git fetch:
  - Downloads new commits, branches, and tags from the remote.
  - Updates remote-tracking branches (origin/main, origin/develop).
  - Does NOT modify your working directory or local branches.
  - Safe: you can inspect changes before integrating them.
  - Always do fetch before comparing: git fetch && git diff main origin/main

git pull:
  - Shortcut for: git fetch + git merge (or git fetch + git rebase with --rebase).
  - Modifies your current branch immediately.
  - Can cause unexpected merge commits.
  - Safer alternative: git pull --rebase (avoids merge commits for a linear history).
```

---

**Q: What is a fast-forward merge vs a three-way merge?**

```
Fast-forward:
  - Possible when the target branch has NO new commits since the feature branched off.
  - Git just moves the branch pointer forward. No merge commit created.

    A---B---C  (main)
             \
              D---E  (feature)

    git merge feature → main pointer moves to E. No merge commit.

    A---B---C---D---E  (main)


Three-way merge:
  - Required when BOTH branches have new commits since diverging.
  - Git finds the common ancestor and creates a merge commit.

    A---B---C---F  (main)      (F was committed after feature branched)
         \
          D---E  (feature)

    git merge feature → creates merge commit M.

    A---B---C---F---M  (main)
         \         /
          D---E---+  (feature)
```

---

## SECTION 13 — Git Configuration for DevOps

```bash
# Identity
git config --global user.name "Daksh"
git config --global user.email "daksh@example.com"

# Default branch
git config --global init.defaultBranch main

# Pull strategy (avoid merge commits)
git config --global pull.rebase true

# Push strategy (only push current branch)
git config --global push.default current

# Auto-prune stale remote branches on fetch
git config --global fetch.prune true

# Useful aliases
git config --global alias.st "status -s"
git config --global alias.co "checkout"
git config --global alias.br "branch"
git config --global alias.lg "log --oneline --graph --all --decorate"
git config --global alias.last "log -1 HEAD --stat"
git config --global alias.unstage "reset HEAD --"
git config --global alias.amend "commit --amend --no-edit"
git config --global alias.wip "commit -am 'WIP'"

# Diff tool
git config --global diff.tool vscode
git config --global difftool.vscode.cmd 'code --wait --diff $LOCAL $REMOTE'

# Credential caching (avoid typing password repeatedly)
git config --global credential.helper cache --timeout=3600     # 1 hour
# On macOS: git config --global credential.helper osxkeychain
# On Windows: git config --global credential.helper manager
```

---

## SECTION 14 — Quick Reference

```bash
# Status & Info
git status -s                        # short status
git branch -vv                       # branches with upstream tracking
git remote -v                        # remote URLs
git stash list                       # saved stashes
git tag -l                           # all tags

# Undo
git restore file.py                  # discard working dir changes
git restore --staged file.py         # unstage
git reset --soft HEAD~1              # undo commit, keep staged
git revert HEAD                      # undo commit on shared branch

# History
git log --oneline --graph --all -20
git blame file.py
git bisect start / good / bad

# Cleanup
git clean -fd                        # remove untracked files and dirs
git clean -fdn                       # dry run (show what would be removed)
git gc                               # garbage collect (compress objects)
git prune                            # remove unreachable objects

# Comparison
git diff main..feature               # what feature has that main doesn't
git diff main...feature              # changes on feature since it diverged from main
git log main..feature --oneline      # commits on feature not on main

# Advanced
git cherry-pick abc1234
git rebase -i HEAD~5
git reflog
git stash pop
git push --force-with-lease
```
