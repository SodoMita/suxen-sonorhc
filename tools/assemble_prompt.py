#!/usr/bin/env python3
"""Print a stage prompt for one character. Does not write a file.

Save whatever you actually send under characters/<id>/prompts/ yourself,
before you look at the result. See references/iteration.md.

    python3 tools/assemble_prompt.py aurora
    python3 tools/assemble_prompt.py aurora --stage expression --expression happy
    python3 tools/assemble_prompt.py aurora --stage plate-black
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from sprite_pipeline.levels import load  # noqa: E402
from sprite_pipeline.prompts import stage_prompt  # noqa: E402


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("character")
    ap.add_argument("--stage", default="sprite")
    ap.add_argument("--expression")
    args = ap.parse_args()
    doc = load()
    match = [ch for ch in doc["characters"] if ch["id"] == args.character]
    if not match:
        known = ", ".join(ch["id"] for ch in doc["characters"])
        raise SystemExit(f"unknown character {args.character!r}; known: {known}")
    text = stage_prompt(doc, match[0], args.stage, args.expression)
    print(text)
    print(f"\n# chars: {len(text)}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
