# Sprite spec

Numbers that keep expression swaps from jittering, and names that keep a card from being shipped as a sprite.

## Canvas

| Asset | Working | Delivered | Format |
|---|---|---|---|
| Reference sheet | as generated | keep, not shipped | PNG, complex background |
| Full-body master | 2048 × 4096 if the generator can hold it | 1024 × 2048 | PNG, straight alpha |
| Runtime slot | — | WebP, lossless, straight alpha | `assets/characters/` and `Sprites/` |
| Expression | same canvas as the master | same | head-only change |
| CG | 3840 × 2160 | 1920 × 1080 | no alpha |

The game canvas is 1280 × 720. Sprites stand in left and right slots, not as full-screen tachie. Work larger than the slot and downsample. Do not upscale a delivered file back up.

Generators drift toward square. Crop and composite onto the sprite canvas. Measure the file. Do not trust the request.

Straight alpha only. The triangulator produces it. Premultiplying it fringes every sprite on a light background.

## Face box

Lock it once, on the first approved master, and write it to `characters/<id>/<id>.sprite.json`. Do not invent pixel numbers before that file exists.

For a 1024 × 2048 delivery the starting assumption, to be measured and then frozen, is:

```
head top     y = 90
chin         y = 380
face box     x = 382 … 642
             y = 90 … 410
anchor       x = 512, y = 2048
```

Rules:

1. An expression changes only pixels inside the face box.
2. The head does not translate, rotate, or scale between differentials.
3. If the expression needs motion outside the box — ears flopping, a shout that lifts the hair — it is a new pose, not a differential.
4. Blush is an overlay inside the box, stacked on the current face, not a new pose.

Heights are unconfirmed. Do not encode a fake world scale. Feet sit on the bottom edge so a later scale pass can bottom-align them.

## Expression set

Required production ids: `neutral`, `happy`, `sad`, `surprised`, `blush`.

Legacy keys already wired in the balloon are listed on each character page. Aurora's neutral is still called `aurora_serious` in dialogue. Do not rename that key by overwriting the file.

`neutral` reads as pleasant and alert, never blank. A dead-eyed neutral makes the game feel cheap. Ren's neutral is steady cyan eyes, not a mouth.

## Naming

Three kinds of file, and the suffix must say which:

1. **Reference** — `<id>_reference.png`. Complex background. Never matted. Never shipped.
2. **Card** — `<id>_card.png`. Text overlays. Never a runtime sprite.
3. **Sprite** — `<id>_<expression>.png` in `characters/`, and the same stem as WebP in the runtime folders. Straight alpha.

Plates, committed, never installed as sprites:

```
characters/<id>/plates/<id>_white.png
characters/<id>/plates/<id>_black.png
```

Lowercase, underscores, ASCII. No generator-default filenames.

If the corners are opaque, or there is text baked in, it is not a runtime sprite, whatever it is called.

## Engine wiring

Dialogue tags look like `#sprite=aurora_serious:right`. The key must exist in the `sprites` dictionary of `scenes/vn_balloon.tscn` and as a file under `assets/characters/`.

```bash
python3 tools/check_sprite_wiring.py
```

`tools/install_sprite.py` writes lossless WebP to both `assets/characters/` and `Sprites/`, and only if the character is approved and the destination does not exist. It prints the dictionary line. It does not edit the scene. Godot writes the `.import` sidecar on next open. Do not hand-author `.import` files.

`Sprites/` holds the original portraits. `assets/characters/` holds the copies the balloon uses. Keep those two in step when you install. Do not "clean up" a name drift by deleting the old file.

## Matting

Nano-Banana-class generators do not emit alpha. Two plates recover it:

```
B = F·a
W = F·a + (1 − a)
a = 1 − (W − B)
F = B / a
```

The difference between the plates is the alpha. Where the figure is opaque, the plates agree. Where it is clear, they differ by the full range. Partial coverage, including hair and Aurora's pane edge, lands in between.

Never instruct the model to keep character pixels identical between plates. That flattens the figure and destroys the signal.

Order:

1. Approve the sprite.
2. Upscale to the working canvas. Matte after the final upscale.
3. White plate, then black plate as an edit of the white plate.
4. `tools/normalize_plates.py` to clamp generator haze to exact white or exact black.
5. `tools/triangulate_matte.py`.
6. `tools/check_matte.py --report` over grey and saturated grounds, not only over white and black. Those two are the inputs, so the sprite always looks right on them.

**Do not chroma-key black.** Ren's body, Selene's coat, and Nova's jacket are near-black. A black key deletes the character. The plate pair is the only route.

Sheets and CGs are never matted. A flat green screen is not a matting background and is not a consistency background. It is retired.

`tools/triangulate_sheet_extract.py` slices a white/black sheet pair into named sprites when one generation produced several poses side by side.
