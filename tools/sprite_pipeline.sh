#!/bin/sh
# Sprite pipeline entry point. Dev-only; never called by the game.
set -e
cd "$(dirname "$0")/.."
case "${1:-}" in
  check)
    python3 tools/render_character_levels.py --check
    python3 tools/check_sprite_wiring.py
    ;;
  render)
    python3 tools/render_character_levels.py
    ;;
  status)
    python3 tools/sprite_status.py
    ;;
  prompt)
    shift
    python3 tools/assemble_prompt.py "$@"
    ;;
  *)
    echo "usage: tools/sprite_pipeline.sh check|render|status|prompt <id> [--stage ...]" >&2
    exit 1
    ;;
esac
