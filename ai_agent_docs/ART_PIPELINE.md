# Sprite pipeline

Chrono Nexus produces standing sprites the way [Seirin](https://github.com/SodoMita/Seirin) does — a loadable agent skill, a cast registry, saved prompts, and white/black triangulation for alpha — fitted to this Godot project.

One stage is different on purpose. Before anyone writes a generator prompt, the character has to exist as **docs at several levels of detail**. A thumbnail test, a design review, a construction pass, and a sendable prompt are not the same text. Truncation eats the end of a long prompt, and that end is the exclusion block, so the long text is never what gets sent.

## Where to start

| If you are… | Read |
|---|---|
| An agent about to draw | [`ai_agent_docs/skills/chrono-sprite/SKILL.md`](skills/chrono-sprite/SKILL.md) |
| Checking the roster at a glance | [`ai_agent_docs/character_levels/CAST_AT_A_GLANCE.md`](character_levels/CAST_AT_A_GLANCE.md) |
| Opening one character | [`ai_agent_docs/character_levels/<id>.md`](character_levels/README.md) |
| Running the tools | `tools/sprite_pipeline.sh` |

The engine is Godot 4.7 and Dialogue Manager. `vn-blueprint.md` still mentions Dialogic. That note is stale. Do not migrate the game as part of art work.

## The path a sprite takes

```
lore (docs/vn_specific/)
        ↓  design pass, not a generator
character_levels/src/<id>.json     L0–L3 prose + canon fields
        ↓  tools/render_character_levels.py
character_levels/<id>.md           the docs a stage is allowed to read
        ↓  tools/assemble_prompt.py
characters/<id>/prompts/NN_*.txt   saved before the result is looked at
        ↓  generator, into characters/<id>/_wip/   (not committed)
white plate + black plate
        ↓  tools/normalize_plates.py
        ↓  tools/triangulate_matte.py
        ↓  tools/check_matte.py
approved master in characters/<id>/
        ↓  tools/install_sprite.py     only after approved: true
assets/characters/ and Sprites/      new filename, never an overwrite
        ↓
scenes/vn_balloon.tscn dictionary key
        ↓
dialogue  #sprite=key:left|right
        ↓  tools/check_sprite_wiring.py
```

## Which description level a stage reads

| Stage | Read | Send |
|---|---|---|
| Roster / thumbnail test | L0 | nothing |
| Design review | L2 | nothing |
| First sprite, no reference image | L3 to judge, L4 to send | L4 only |
| Edit with a reference attached | L1, plus the one change | the short edit prompt |
| Expression differential | L3 expression geometry | `assemble_prompt.py --stage expression` |
| White / black plates | the approved sprite, not a new design | `plate-white` / `plate-black` |
| QA before install | L3 | nothing |

L0 is about 30 words. L1 is a card. L2 is the design. L3 is construction and acceptance. L4 is the only text that may be sent for the default sprite, and it is kept under 1500 characters so the exclusion block survives.

`approved` is false on every character. Draft generations stay in `_wip/`. Runtime files in `Sprites/` and `assets/characters/` are not overwritten.

## Commands

```bash
python3 tools/render_character_levels.py --check
python3 tools/check_sprite_wiring.py
python3 tools/sprite_status.py
python3 tools/assemble_prompt.py aurora --stage sprite
python3 tools/assemble_prompt.py kira --stage expression --expression happy
python3 -m unittest tests/test_sprite_pipeline.py
```

Matting needs a dev venv. It is not a game dependency.

```bash
python3 -m venv .venv
.venv/bin/pip install -r tools/requirements.txt
.venv/bin/python -m unittest tests/test_sprite_pipeline.py
```

## What is already in the game

Shipped portraits are opaque-on-black drafts. Some match the production identity (Ren, Elara, Selene, Aria). Some do not (Kira, Aurora, Nova) and are named as non-references in those characters' L3 notes. Do not img2img from a file the level doc forbids. Do not delete the shipped files.
