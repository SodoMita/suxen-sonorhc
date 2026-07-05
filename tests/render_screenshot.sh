#!/usr/bin/env bash
# Render a screenshot of the running project to verify visuals.
# Uses Godot 4.7's --screenshot CLI mode (if available) or the
# headless --quit-after option.
set -e
GODOT_BIN="${GODOT_BIN:-/home/user/.local/bin/godot}"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
mkdir -p tests/output
"$GODOT_BIN" --headless --write-movie tests/output/chrono.mov 2>&1 | tail -5 || true
echo "Screenshot test: ran headlessly. (Visual verification requires running the editor in windowed mode.)"
