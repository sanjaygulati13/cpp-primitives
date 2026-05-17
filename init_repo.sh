#!/usr/bin/env bash
# init_repo.sh — one-time bootstrap: init git, push to GitHub.
#
# Prerequisites:
#   - You have a GitHub account (sanjaygulati13)
#   - You have either `gh` CLI installed (preferred) OR you've created the
#     repo manually at https://github.com/new
#
# What this does:
#   1. git init in this directory
#   2. Configure user.name / user.email (if not already set)
#   3. Create the repo on GitHub via `gh` (if available) or prompt for manual
#   4. Push the initial commit
#
# Run from inside the cpp-primitives directory:
#   chmod +x init_repo.sh
#   ./init_repo.sh

set -euo pipefail

REPO_NAME="cpp-primitives"
GITHUB_USER="sanjaygulati13"

cd "$(dirname "$0")"

echo "════════════════════════════════════════"
echo " cpp-primitives — initial repo bootstrap"
echo "════════════════════════════════════════"
echo

# Step 1 — sanity
if [ -d ".git" ]; then
    echo "✋ .git/ already exists — repo already initialized."
    echo "   To start fresh: rm -rf .git && rerun this script."
    exit 1
fi

# Step 2 — git init
echo "▶ git init"
git init -q -b main
git add .
git -c user.email="sanjaygulati13@gmail.com" \
    -c user.name="Sanjay Gulati" \
    commit -q -m "Initial scaffold: lru_cache + CI + tests + benchmark"
echo "   ✓ initial commit created"
echo

# Step 3 — create remote
if command -v gh >/dev/null 2>&1; then
    echo "▶ Creating remote via gh CLI"
    gh repo create "$REPO_NAME" \
        --public \
        --description "Small C++ primitives, done right" \
        --source=. \
        --push \
        --remote=origin
    echo "   ✓ pushed to https://github.com/$GITHUB_USER/$REPO_NAME"
else
    echo "⚠ gh CLI not installed."
    echo
    echo "  Manual steps:"
    echo "    1. Go to https://github.com/new"
    echo "    2. Name: $REPO_NAME · Public · DO NOT initialize with README/license/.gitignore"
    echo "    3. Then run:"
    echo "         git remote add origin https://github.com/$GITHUB_USER/$REPO_NAME.git"
    echo "         git push -u origin main"
    echo
    echo "  Or install gh CLI: https://cli.github.com/"
fi

echo
echo "════════════════════════════════════════"
echo " Next:"
echo "   - Verify CI is green: https://github.com/$GITHUB_USER/$REPO_NAME/actions"
echo "   - Add badge to your sanjaygulati.dev"
echo "   - Tweet about it (optional)"
echo "════════════════════════════════════════"
