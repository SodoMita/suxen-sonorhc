# Chrono Nexus (suxen-sonorhc)

A Godot **4.7** visual novel. The dialogue UI is the classical balloon from [vn_dialogue_demo](https://github.com/SodoMita/vn_dialogue_demo) (Nathan Hoad's Dialogue Manager), restyled for Chrono Nexus: translucent glass panels, cyan edges, per-speaker name colors, a stat strip, and a title card over the 3D nexus. Dialogic is not used.

Typewriter **sounds** are removed. Lines still reveal character by character; they do not tick.

## Getting started

1. Open this folder in **Godot 4.7**.
2. Press **F5**.

The title card sits on the 3D nexus. **Step through the rift** starts the short opening. **The long night** is the longer classroom prologue. **A flicker in the lab** is the Russian lab branch.

### Controls

- **Read on:** click, Enter, or Space. The first press finishes the text reveal; the next advances.
- **Skip:** hold Ctrl.
- **Log / rollback:** H, or the mouse wheel.
- **Quick save / load:** F5 / F9.
- **Pause:** Esc or right click.
- **Panic screen:** F12.
- **Story map:** a visited header rolls back. An unvisited header is replayed by Dialogue Manager so choices and mutations stay the engine's. A path that rewrites earlier choices waits for the spoiler toggle.

Save, load, settings, auto, and the story map live on the bottom system row.

## Layout

- `scenes/vn_balloon.tscn` — authored UI. Edit it in the Godot editor; the script does not build the chrome.
- `dialogue/chrono_nexus.dialogue` — the story. Stage tags: `#bg=`, `#sprite=key:left|right`, `#focus=`, `#music=`, `#sfx=`.
- `autoloads/game_state.gd` — trust, insight, power, bonds. Choices mutate these; rollback restores them.
- `Sprites/` — original portraits. `assets/characters/` — the same art, trimmed so it fits the left/right slots.
- `bgs/` — backgrounds. `scenes/3d/` — the nexus behind the title card.
- `addons/dialogue_manager/` — Dialogue Manager 4.1.0.

## Palette

The glass look lives on the balloon and the title card: navy panels around `Color(0.035, 0.045, 0.11, 0.8)`, cyan borders, and a soft blue shadow. Speaker names tint the name plate (Aurora cyan, Kira amber, Elara green, Selene violet).
