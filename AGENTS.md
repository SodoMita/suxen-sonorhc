# AGENTS.md — Chrono Nexus

Instructions for agents working in this repository. Read this before making changes.

## What this is

A Godot 4.7 visual novel. Dialogue Manager balloon, glass UI, live SceneScore music. The story is in `dialogue/`. Stage tags are `#bg=`, `#sprite=key:left|right`, `#focus=`, `#music=`, `#sfx=`.

`vn-blueprint.md` still mentions Dialogic. That note is stale. Do not migrate the engine as part of another task.

## Sprite work

Character art is governed by the `chrono-sprite` skill:

`ai_agent_docs/skills/chrono-sprite/SKILL.md`

Read `LEGAL.md` and `OPERATIONS.md` there before generating or committing a sprite. The description ladder — the same character at several levels of detail — lives in `ai_agent_docs/character_levels/`. Pipeline map: `ai_agent_docs/ART_PIPELINE.md`.

Do not overwrite files in `Sprites/` or `assets/characters/`. New sprites get new names. `tools/install_sprite.py` enforces that, and it refuses an unapproved character.

## Checks

```bash
python3 tools/render_character_levels.py --check
python3 tools/check_sprite_wiring.py
python3 -m unittest tests/test_sprite_pipeline.py
```

Matting tools need a dev venv (`tools/requirements.txt`). They are not a game dependency.

Godot headless smoke, when a Godot binary is available: `tests/run_headless.sh`.

## Invariants

- Do not commit secrets. The pre-commit hook scans staged files for token shapes.
- Do not force-push or switch branches. Work on the branch assigned to the session.
- Do not add a CDN, a runtime fetch, or a Python import to the game.
- Dialogue `#sprite=` keys must exist in `scenes/vn_balloon.tscn` and as files under `assets/characters/`.
