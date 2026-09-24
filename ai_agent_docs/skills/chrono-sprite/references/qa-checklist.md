# QA checklist

Run this before calling a sprite installable. A skipped check is reported as skipped.

## Structure

```bash
python3 tools/render_character_levels.py --check
python3 tools/check_sprite_wiring.py
python3 tools/check_matte.py <master> --report
```

`--report` must show real clear pixels and some partial alpha. A fully opaque result means the plates were flattened. A binary alpha means hair and glows were crushed. Soft-edge colour far from the body colour means the old background leaked in.

Then composite over mid-grey, magenta, and a warm tan. White and black are the inputs. They hide the defect.

## Eye

- Silhouette test. Fill the figure black. Name the character. If you cannot, stop.
- Thumbnail test. At 64 pixels the memory point still reads: Ren's two eyes, Aurora's pane, Kira's ear notches, Elara's bell, Selene's staff, Aria's wing bar, Nova's offset rectangle.
- Memory point present, unobstructed, accent colour where the level doc reserves it.
- No second focal point fighting it.
- Feet on the bottom edge. Hands and feet in frame. Five digits where a hand is visible.
- Expression set: only the face box changed. Overlay the neutral and the happy master. Body pixels match.
- No readable text, watermark, or signature.
- No banned item from that character's list.
- Depiction pass against `LEGAL.md`. This is not automated. Do it every time.
- Legacy drift: if the new file quietly copied a forbidden shipped costume, reject it even if the prompt was L4.

## Do not ship

- A reference sheet or a card as a runtime sprite.
- A black-keyed Ren, Selene, or Nova.
- An overwrite of an existing WebP.
- A file whose prompt was never saved.
