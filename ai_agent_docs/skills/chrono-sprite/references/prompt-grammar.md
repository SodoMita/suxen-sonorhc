# Prompt grammar

For generators that read prose, not tag soup. The assembler in `tools/sprite_pipeline/prompts.py` is the implementation. This file is the spec it follows.

## Seven sections

Order when you have room: FORMAT, STYLE, IDENTITY, WARDROBE, COLOR, STAGING, EXCLUDE.

Order when you might be truncated: a short exclude line inside FORMAT, then the rest, then EXCLUDE again. L4 does both. The budget is 1500 characters. `--check` fails a longer L4.

## Rules

1. State the memory point twice, IDENTITY and STAGING.
2. Positive phrasing for anything structural. "A closed high-neck coat" beats "no transparent suit". Keep the exclusion section anyway.
3. Hex plus a colour name in L2 and L3. L4 carries the hex. L0 and L1 do not.
4. One change per edit. Restate what must stay, then the change.
5. Do not open with "anime girl", "beautiful woman", or "1girl".
6. Name the light direction. Otherwise you get flat frontal light and the silhouette dies.
7. For sprites, state the anchor: feet on the bottom edge, head fully in frame.

## Stages

`python3 tools/assemble_prompt.py <id> --stage <stage>`

| Stage | What it asks for |
|---|---|
| `sprite` | L4. Default standing pose. |
| `turnaround` | Four views on a complex in-world background. Not green. |
| `plate-white` | Edit onto pure white. Honest composite. |
| `plate-black` | Edit of the white plate onto pure black. |
| `expression` | Face only. Requires `--expression`. |
| `hero` | Key visual. Not a runtime sprite. Not matted. |

Plate prompts tell the model to let the background show through every non-opaque pixel. They do not say "keep character pixels identical".

Expression prompts name the brow, eye, and mouth from the character record, and they forbid a head move. Generate each one from the approved sprite. Do not chain happy into sad into blush.

## Budget

If a hand-written prompt will not fit in 1500 characters, cut in this order:

1. Drop the style sentence once a reference image is attached.
2. Trim exclusions to this character's banned list plus the early exclude line.
3. Drop colour names and keep hexes.
4. Cut hedging.
5. Do not cut the memory point, and do not cut the early exclude.

Emoji is a last resort for a plain noun, never for a garment or a hex, and it is untested here. Record the result if you try it.

## Failure modes

| What you see | Likely cause | One change |
|---|---|---|
| Memory point missing | Stated once, or only at the end | Repeat it in STAGING |
| Extra fingers, cropped feet | FORMAT did not say full body and margin | Add that, change nothing else |
| Off-model costume | L3 was pasted and then truncated | Send L4, then add the one missing fact |
| Flat figure, no alpha after triangulation | Plates were asked to be identical | Regenerate the black plate as an honest composite |
| Text in the pane or on the coat | "Code" or "filigree" was read as letters | Say abstract marks, no readable words |
| Second character's prop | Silhouette class was paraphrased | Paste the class string |
