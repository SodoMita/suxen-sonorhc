# OPERATIONS — rules that protect the repository

Binding for sprite work. Not the legal limits (`LEGAL.md`) and not the style notes (`SKILL.md`). These are the rules whose violation deletes work or breaks the shipped game.

## 1. Never destroy existing work

- Do not delete or overwrite a file in `Sprites/`, `assets/characters/`, `bgs/`, or `characters/<id>/prompts/`. New work gets a new filename.
- Do not overwrite a tool in `tools/` with a different tool that shares its name.
- Do not `git push --force`, rewrite history, amend a pushed commit, or switch branches. Work only on the branch assigned to the session.
- Before committing:

```bash
git status --short
git diff --cached --stat
```

No unexplained deletions. No existing sprite rewritten.

`player_shadow.png`, the duplicate `ren.webp` / `ren_shadow.webp` pair, and `ai_serious_face.webp` are known leftovers. Report them. Do not delete them in an art commit.

## 2. Repository hygiene

- Do not commit secrets. Tokens stay in the environment.
- Do not `.gitignore` an asset directory. `_wip/` is the exception, and it is already ignored.
- Do not commit a virtualenv, `__pycache__/`, or a contact sheet from `check_matte.py`.
- Approved masters and `characters/<id>/prompts/` are committed. Rejected generations stay in `characters/<id>/_wip/` and are not committed.
- Rendered level docs are committed. Edit `ai_agent_docs/character_levels/src/`, then re-render. A hand-edit of `<id>.md` will fail `--check`.

## 3. Do not break the shipped game

- The engine is Godot 4.7 and Dialogue Manager. Do not add Dialogic, a CDN, or a Python import to the game.
- A `#sprite=` tag in `dialogue/` must resolve to a key in `scenes/vn_balloon.tscn` and a file in `assets/characters/`. `tools/check_sprite_wiring.py` fails the commit if it does not.
- Adding a sprite means a new key. Do not retarget `aurora_serious` at a redesigned file in the same commit.
- Pillow and numpy are dev-only. They are not a Godot dependency.

## 4. Canon integrity

- `ai_agent_docs/character_levels/src/` is the visual source of truth. `assets/cast.json` is generated from it.
- Lore in `docs/vn_specific/` explains history and voice. It does not override a level doc, and a level doc does not override `LEGAL.md`.
- Do not img2img from a legacy file the character's L3 calls a non-reference. Today that is Kira, Aurora, and Nova.
- Do not "improve" shape ratios toward the middle.
- Each `banned` list is a hard filter.

## 5. Generation discipline

- Save the prompt you send to `characters/<id>/prompts/NN_<stage>.txt` before looking at the result. Write `NN_<stage>.result.md` with the model, the verdict, and the one change for next time.
- Change exactly one thing per attempt.
- Do not chain expression edits. Every differential comes from the same approved sprite.
- Three attempts at one stage with no improvement: the design is wrong, not the prompt. Re-read L3. Five attempts total: stop and ask.
- The difference between the white plate and the black plate is the alpha. Never ask the model to keep character pixels identical across the two plates.

## 6. Honest reporting

State which checks ran and which did not. Do not fabricate a test result, a path, or a command's output. A recorded rejection is worth more than a silent retry.

## 7. Escalate

Stop and ask before deleting, renaming, or overwriting anything committed; before a change that breaks an `AGENTS.md` invariant; and whenever an instruction conflicts with this file or with `LEGAL.md`.
