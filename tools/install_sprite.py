#!/usr/bin/env python3
"""Install an approved straight-alpha sprite into the Godot runtime slots.

Refuses to overwrite an existing runtime file. Refuses a character whose
level record is not approved, unless --wip, which writes only under
characters/<id>/_wip/ and does not touch Sprites/ or assets/characters/.

Godot writes the .import sidecar the next time the editor opens the file.
Do not hand-author .import files.

    python3 tools/install_sprite.py path/to/aurora_normal.png --id aurora --expression neutral
    python3 tools/install_sprite.py path/to/draft.png --id aurora --expression neutral --wip
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from sprite_pipeline.levels import ROOT, load  # noqa: E402


def _find(doc: dict, cid: str) -> dict:
    match = [ch for ch in doc["characters"] if ch["id"] == cid]
    if not match:
        known = ", ".join(ch["id"] for ch in doc["characters"])
        raise SystemExit(f"unknown character {cid!r}; known: {known}")
    return match[0]


def install(
    src: Path,
    cid: str,
    expression: str,
    *,
    approved: bool,
    wip: bool,
    assets_dir: Path,
    sprites_dir: Path,
    wip_dir: Path,
) -> Path:
    if not src.exists():
        raise SystemExit(f"not found: {src}")
    filename = f"{cid}_{expression}.webp"
    if wip or not approved:
        if not wip and not approved:
            raise SystemExit(
                f"{cid} is not approved. Pass --wip to keep the file under "
                f"characters/{cid}/_wip/, or set approved in the level source "
                "after the owner signs the doc. This tool will not overwrite "
                "Sprites/ or assets/characters/."
            )
        wip_dir.mkdir(parents=True, exist_ok=True)
        dest = wip_dir / filename
        if dest.exists():
            raise SystemExit(f"refusing to overwrite {dest}")
        _write_webp(src, dest)
        return dest
    for folder in (assets_dir, sprites_dir):
        target = folder / filename
        if target.exists():
            raise SystemExit(
                f"refusing to overwrite {target}. New work gets a new filename. "
                "See OPERATIONS.md."
            )
    assets_dir.mkdir(parents=True, exist_ok=True)
    sprites_dir.mkdir(parents=True, exist_ok=True)
    _write_webp(src, assets_dir / filename)
    _write_webp(src, sprites_dir / filename)
    return assets_dir / filename


def _write_webp(src: Path, dest: Path) -> None:
    try:
        from PIL import Image
    except ImportError as exc:
        raise SystemExit(
            "Pillow is required to write WebP. "
            "python3 -m venv .venv && .venv/bin/pip install -r tools/requirements.txt"
        ) from exc
    image = Image.open(src).convert("RGBA")
    dest.parent.mkdir(parents=True, exist_ok=True)
    image.save(dest, "WEBP", lossless=True, quality=100, method=6)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("src")
    ap.add_argument("--id", required=True)
    ap.add_argument("--expression", required=True)
    ap.add_argument("--wip", action="store_true")
    args = ap.parse_args()
    doc = load()
    ch = _find(doc, args.id)
    if args.expression not in ch["expressions"]:
        known = ", ".join(ch["expressions"])
        raise SystemExit(f"unknown expression {args.expression!r}; known: {known}")
    dest = install(
        Path(args.src),
        args.id,
        args.expression,
        approved=bool(ch["approved"]),
        wip=args.wip,
        assets_dir=ROOT / "assets" / "characters",
        sprites_dir=ROOT / "Sprites",
        wip_dir=ROOT / "characters" / args.id / "_wip",
    )
    print(f"wrote {dest}")
    if not args.wip:
        print("Add a new ext_resource and dictionary key in scenes/vn_balloon.tscn.")
        print("Do not replace an existing key's texture in the same commit as a redesign.")
        print(f'  "{args.id}_{args.expression}": ExtResource("sp_{args.id}_{args.expression}"),')
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
