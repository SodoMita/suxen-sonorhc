#!/usr/bin/env python3
"""Render the character description ladder from the JSON source.

    python3 tools/render_character_levels.py
    python3 tools/render_character_levels.py --check
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from sprite_pipeline.levels import check, load, write_outputs  # noqa: E402


def main() -> int:
    doc = load()
    if "--check" in sys.argv:
        errors = check(doc, include_diff=True)
        if errors:
            print(f"{len(errors)} level-doc error(s):")
            for err in errors:
                print(f"  - {err}")
            return 1
        print(f"level docs ok ({len(doc['characters'])} characters)")
        return 0
    errors = check(doc, include_diff=False)
    if errors:
        print("refusing to render; source failed checks:")
        for err in errors:
            print(f"  - {err}")
        return 1
    written = write_outputs(doc)
    print(f"rendered {len(written)} files")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
