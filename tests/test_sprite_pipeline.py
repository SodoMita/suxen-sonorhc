"""Sprite pipeline checks. Dev-only; the game does not import this.

Level and wiring checks need only the standard library. The matte round-trip
needs Pillow and numpy (`tools/requirements.txt`).
"""

from __future__ import annotations

import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

from sprite_pipeline.levels import check, load  # noqa: E402
from sprite_pipeline.prompts import stage_prompt  # noqa: E402
from sprite_pipeline.wiring import wiring_errors  # noqa: E402


class LevelDocs(unittest.TestCase):
    def test_rendered_docs_match_source(self):
        errors = check(load(), include_diff=True)
        self.assertEqual(errors, [], "\n".join(errors))

    def test_expression_prompt_rejects_unknown(self):
        doc = load()
        ren = next(ch for ch in doc["characters"] if ch["id"] == "ren")
        with self.assertRaises(SystemExit):
            stage_prompt(doc, ren, "expression", "ecstatic")


class Wiring(unittest.TestCase):
    def test_dialogue_sprite_tags_resolve(self):
        self.assertEqual(wiring_errors(), [])


class InstallGate(unittest.TestCase):
    def test_unapproved_character_is_not_installed(self):
        import importlib.util

        spec = importlib.util.spec_from_file_location(
            "install_sprite", ROOT / "tools" / "install_sprite.py"
        )
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        with tempfile.TemporaryDirectory() as tmp:
            src = Path(tmp) / "draft.png"
            src.write_bytes(b"not a real png")
            with self.assertRaises(SystemExit) as caught:
                module.install(
                    src,
                    "ren",
                    "neutral",
                    approved=False,
                    wip=False,
                    assets_dir=Path(tmp) / "assets",
                    sprites_dir=Path(tmp) / "sprites",
                    wip_dir=Path(tmp) / "wip",
                )
        self.assertIn("not approved", str(caught.exception))


class Matte(unittest.TestCase):
    def test_partial_alpha_round_trip(self):
        try:
            import numpy as np
            from PIL import Image
        except ImportError:
            self.skipTest("Pillow and numpy are not installed")
        spec = importlib_load(ROOT / "tools" / "triangulate_matte.py", "triangulate_matte")
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            white = np.ones((8, 8, 3), dtype=np.float32)
            black = np.zeros((8, 8, 3), dtype=np.float32)
            # Opaque grey pixel, a 0.5 pixel, and a clear pixel.
            colour = np.array([0.8, 0.2, 0.4], dtype=np.float32)
            white[1, 1] = colour
            black[1, 1] = colour
            alpha = 0.5
            white[2, 2] = colour * alpha + (1.0 - alpha)
            black[2, 2] = colour * alpha
            save_rgb(white, tmp / "w.png")
            save_rgb(black, tmp / "b.png")
            spec.triangulate(tmp / "w.png", tmp / "b.png", tmp / "out.png", tmp / "a.png")
            rgba = np.asarray(Image.open(tmp / "out.png").convert("RGBA"), dtype=np.float32) / 255.0
            self.assertAlmostEqual(float(rgba[1, 1, 3]), 1.0, delta=0.02)
            self.assertAlmostEqual(float(rgba[2, 2, 3]), 0.5, delta=0.02)
            self.assertAlmostEqual(float(rgba[0, 0, 3]), 0.0, delta=0.02)
            self.assertAlmostEqual(float(rgba[2, 2, 0]), 0.8, delta=0.03)


def importlib_load(path: Path, name: str):
    import importlib.util

    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def save_rgb(array, path: Path) -> None:
    from PIL import Image

    image = Image.fromarray((array * 255.0 + 0.5).astype("uint8"), "RGB")
    image.save(path)


if __name__ == "__main__":
    unittest.main()
