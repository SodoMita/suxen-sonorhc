#!/usr/bin/env python3
"""Pipeline coverage: which production expressions exist as runtime files.

Does not fail the build. Missing expressions are the work list, not a
regression — the shipped game already has an incomplete set.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from sprite_pipeline.levels import load  # noqa: E402
from sprite_pipeline.wiring import ASSETS, status_lines  # noqa: E402


def main() -> int:
    doc = load()
    print("\n".join(status_lines()))
    print("")
    print("Production expression coverage (legacy filename present?):")
    stems = {p.stem for p in ASSETS.glob("*") if p.suffix != ".import"}
    for ch in doc["characters"]:
        cells = []
        for expr, spec in ch["expressions"].items():
            aliases = spec.get("alias") or []
            hit = any(name in stems for name in aliases)
            mark = "yes" if hit else "MISSING"
            cells.append(f"{expr}={mark}")
        print(f"  {ch['id']:<8} " + "  ".join(cells))
    print("")
    print("approved flags are false. New sprites stay in characters/<id>/_wip/")
    print("until the owner sets approved and tools/install_sprite.py is run.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
