# Character masters

Production sprites live here, one directory per `id` from the description ladder. The Godot runtime does not load this folder. `assets/characters/` and `Sprites/` do, and only `tools/install_sprite.py` writes into them, after `approved` is true.

```
characters/<id>/
  prompts/
    00_level_sprite.txt         # current L4, rendered, UNSENT
    01_sprite.txt               # the text actually sent, when you send one
    01_sprite.result.md
  plates/                       # white and black plates, committed with the master
  <id>_matted.png               # straight alpha, after triangulation
  _wip/                         # rejected generations, not committed
```

`00_level_sprite.txt` is regenerated from `ai_agent_docs/character_levels/src/`. Do not hand-edit it. When you send a prompt, copy the text into the next numbered file so the record of what was sent cannot be overwritten by a re-render.

Shipped portraits already in `Sprites/` and `assets/characters/` stay where they are. They are drafts, some of them off the production spec. See each character's L3 legacy note.
