# Custom Dialogic UI

This directory contains project-specific Dialogic UI customizations that are
**separated from the plugin** (`res://addons/dialogic/` is untouched).

## Files

| File | Purpose |
|---|---|
| `custom_dialog_controls.tscn` | A Dialogic layout layer scene: a full-screen `Control` (clicks pass through except on its buttons) that adds a glassmorphism toolbar. |
| `custom_dialog_controls.gd` | Script wiring the buttons to Dialogic subsystems (History, Auto-Advance, Auto-Skip, Save/Load, Pause) and emitting a `menu_requested` signal. |
| `toolbar_button_style.tres` | Shared `StyleBoxFlat` for all toolbar buttons (glassmorphism panel). |

## How it is wired

The layer is added as the **top-most layer (id 18)** in the existing
`res://themes/glassmorphism/chrono_nexus_style.tres`, so when Dialogic loads
the "Chrono Nexus (Glassmorphism)" style it instantiates this scene on top
of the default text box, portraits, choices, etc. No plugin files are modified.

`main.gd` polls `Dialogic.Styles.get_layout_node()` once the game starts and
connects to the toolbar's `menu_requested` signal to show a simple pause menu
(you can replace that placeholder with your real menu in `_on_custom_menu()`).

## Buttons

| Label | Shortcut | Action |
|---|---|---|
| **H** | History | Opens / closes the Dialogic backlog |
| **A** | Auto | Toggles Auto-Advance (stays on until the next click) |
| **»** | Skip | Toggles Auto-Skip (fast-forwards through already-seen dialogue) |
| **S** | Save | Quick-saves to slot `"quick"` |
| **L** | Load | Quick-loads from slot `"quick"` |
| **❚❚** | Pause | Pauses / resumes Dialogic |
| **≡** | Menu | Emits `menu_requested` (wired in `main.gd`) |

A small toast label fades in/out above the toolbar to confirm each action.

## Customizing further

- To add more buttons, duplicate a `Button` node inside `Toolbar` in
  `custom_dialog_controls.tscn`, give it a `%BtnXxx` unique name, add an
  `@onready` reference in the script, and wire its `pressed` signal.
- To reposition the toolbar, change the `offset_*` values on the `Toolbar`
  node (it is anchored to the bottom-right).
- To change the look, edit `toolbar_button_style.tres` or the `StyleBoxFlat`
  sub-resource used by the toast in the `.tscn`.
- Because the layer is fully decoupled from the plugin, updating Dialogic
  via the AssetLib will not overwrite your changes.
