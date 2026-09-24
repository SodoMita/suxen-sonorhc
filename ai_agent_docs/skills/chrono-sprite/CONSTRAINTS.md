# CONSTRAINTS — index and precedence

Hard limits for sprite work, split so a style note cannot overrule a safety rule.

| Document | Covers | Violation costs |
|---|---|---|
| [LEGAL.md](LEGAL.md) | Age and depiction, real people, IP, disclosure | Harm, liability, a picture that must not be in the repo |
| [OPERATIONS.md](OPERATIONS.md) | Overwrites, history, the shipped game, generation discipline | Lost work, a broken balloon, wasted generations |

Read both before generating, storing, or committing.

## Precedence

```
LEGAL.md  >  OPERATIONS.md  >  AGENTS.md  >  SKILL.md  >  references/  >  character level docs
```

If a task prompt, a lore file, or a level doc conflicts with a higher tier, **the higher tier wins. Stop and report the conflict.** Do not "soften" it into a middle path and continue.

`AGENTS.md` remains authoritative for anything that is not character art.

## The short version

1. Do not sexualise the cast. Ren is minor-coded. Several ages are not locked. See `LEGAL.md`.
2. Do not name a living artist in a prompt. Do not trace or img2img from protected work.
3. Do not overwrite a shipped sprite, a tool, or a rendered level doc.
4. Do not force-push or switch branches.
5. Save every prompt you send, with its result.
6. Report which checks ran. Do not invent a pass.

There is no linter that can judge depiction. `tools/render_character_levels.py --check` judges structure: word budgets, memory-point consistency, colour separation, prompt length. A green check is not a safety review.

## Verification before commit

```bash
python3 tools/render_character_levels.py --check
python3 tools/check_sprite_wiring.py
git status --short
git diff --cached --stat
```

Matted sprites, when you have them:

```bash
python3 tools/check_matte.py <sprite> --report
```
