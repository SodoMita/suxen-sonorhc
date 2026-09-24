---
name: chrono-sprite
description: >-
  Produce Chrono Nexus standing sprites and expression differentials from the
  character description ladder. Use when creating, revising, or reviewing
  character designs, tachie, expression sets, or when asked to make the
  sprites, design the cast, or raise sprite quality. Encodes silhouette,
  memory point, zoned colour, the locked face box, white/black matting, and
  the rule that each pipeline stage reads a different level of character detail.
license: CC-BY-4.0
compatibility: >-
  Needs an image generator that accepts natural-language edits. Validators
  need python3. Matting needs Pillow and numpy in a dev venv, never in the game.
metadata:
  project: Chrono Nexus
  applies-to: characters/, Sprites/, assets/characters/, ai_agent_docs/character_levels/
  version: "1.0.0"
  cast-registry: assets/cast.json
---

# Chrono Nexus sprites

Produce standing sprites the game can swap without a jitter, at a quality that still reads at thumbnail size.

This skill is adapted from the Seirin character-art pipeline (stages, prompt grammar, white/black triangulation, saved prompts) and fitted to Godot 4.7. The part Seirin left as unanswered briefs is, here, a **description ladder**: the same character at five densities. Read `references/description-levels.md` before writing a prompt.

## Non-negotiables

1. Read `LEGAL.md` and `OPERATIONS.md` before generating, storing, or committing. `CONSTRAINTS.md` is the index. If anything conflicts, stop and report. Do not resolve it yourself.
2. Do not invent a design fact the level docs do not state. An unanswered question at the bottom of a character page is a question, not a gap.
3. Send L4, or a stage prompt from `tools/assemble_prompt.py`. Do not send L3. Do not send lore files.
4. Save every prompt you actually send, verbatim, under `characters/<id>/prompts/`, before you look at the result. See `references/iteration.md`.
5. Copy each character's `banned` list into the exclusion section. The assembler already does this for generated prompts. A hand-written prompt must too.
6. Identity before polish. A beautiful off-model sprite is a defect.
7. Appeal comes from silhouette, memory point, and expression. Not from exposure. See `references/appeal-and-safety.md`.
8. `approved` is false until the project owner sets it. Drafts go to `characters/<id>/_wip/` and are not committed. `tools/install_sprite.py` will not touch `Sprites/` or `assets/characters/` for an unapproved character.

## Design grammar

Five levers. Full notes in `references/design-canon.md`.

1. **Silhouette.** Fill the figure black. If you cannot name them, stop. Big/mid/small mass, and protect the negative space.
2. **One memory point.** Highest detail, accent colour, present in every asset. A second focal point halves both. The secondary hook is for the second look.
3. **Colour by zone.** Head is hair, skin, eyes. Body stays readable. Main colour is what the character *means*, not whichever area is largest. Ren means void black; the cyan is reserved for the eyes.
4. **Shape language.** Do not average the cast toward the middle.
5. **Symbol set.** Eye, brow, mouth, contour, hair, body. Copy these fields literally. Paraphrasing is where drift starts.

Any two characters differ in silhouette class and in main-colour distance. The checker enforces both.

## Workflow

### 0. Pick the level

| You are doing | Read | Send |
|---|---|---|
| Roster or thumbnail | L0, or `CAST_AT_A_GLANCE.md` | nothing |
| Design review | L2 | nothing |
| First sprite | L3 to judge | L4 |
| Edit with a reference | L1 plus one change | the edit prompt |
| Expression | L3 expression lines | `--stage expression` |
| Plates | the approved sprite | `--stage plate-white` then `plate-black` |
| QA | L3 | nothing |

If the level you need is missing a fact, stop and ask. Do not promote L0 into a costume.

### 1. Generate in stage order

Each stage after the first takes the previous approved image as a reference. A prompt alone will not hold identity.

1. **Reference** on a complex in-world background. Never a flat green screen. Saved as `<id>_reference.png`. Not matted. Not shipped.
2. **White plate** — the sprite figure on pure `#FFFFFF`.
3. **Black plate** — an edit of the white plate, same pose, scale, and position, background pure `#000000`. The background must show through every non-opaque pixel. Never ask for "identical character pixels".
4. **Triangulate** the pair. Commit the plates with the master. They are the only way to redo the matte.
5. **Expressions** — head-only edits of the approved sprite, each from that sprite, never chained.
6. **Hero / CG** only after the sprite exists. CGs are not matted.

### 2. Matte

```bash
python3 tools/normalize_plates.py plates/<id>_white.png plates/<id>_black.png
python3 tools/triangulate_matte.py plates/<id>_white.png plates/<id>_black.png <id>_matted.png --alpha-out <id>_alpha.png
python3 tools/check_matte.py <id>_matted.png --report
```

Ren is almost entirely black. A black-key will delete him. Alpha comes only from the plate pair. The same is true of Selene's coat and Nova's jacket.

### 3. Install

Only after `approved` is true, and only to a new filename.

```bash
python3 tools/install_sprite.py characters/ren/ren_neutral.png --id ren --expression neutral
```

Then add a new balloon key. Do not retarget an existing key in the same commit as a redesign. `tools/check_sprite_wiring.py` must still pass.

## Prompt rules that decide the result

- State the memory point twice. Once, and it gets dropped about half the time.
- Positive phrasing beats negation. Keep the exclusion section anyway.
- Hex codes work. L4 carries them. L0 and L1 do not.
- One change per edit.
- Do not open with "anime girl". Open with the asset type and the silhouette class.
- Emoji is not a substitute for a garment.

Details, stage templates, and the character budget: `references/prompt-grammar.md`.

## Files

- `LEGAL.md` — depiction, age, IP, disclosure. Binding.
- `OPERATIONS.md` — repository and workflow limits. Binding.
- `CONSTRAINTS.md` — precedence.
- `assets/cast.json` — generated registry. Do not edit.
- `../../character_levels/` — the description ladder.
- `references/description-levels.md` — how the levels are consumed.
- `references/design-canon.md`
- `references/sprite-spec.md` — canvas, face box, Godot wiring.
- `references/prompt-grammar.md`
- `references/iteration.md`
- `references/qa-checklist.md`
- `references/appeal-and-safety.md`
