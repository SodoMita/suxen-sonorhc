# Custom Dialogic UI

This directory contains project-specific Dialogic UI customizations that are
**separated from the plugin** (`res://addons/dialogic/` is completely
untouched, so updating or reinstalling the Dialogic plugin will never
overwrite your UI work).

## Files

| File | Purpose |
|---|---|
| `glassmorphism_textbox_layer.tscn` | A copy of Dialogic's default VN textbox layer, with the glassmorphism panels + proper semi-transparent modulate colors baked directly into the scene (no fragile override-dict serialization needed). Used by the "Chrono Nexus (Glassmorphism)" style instead of the plugin's default textbox scene. |
| `custom_dialog_controls.tscn` | A Dialogic layout layer scene: a full-screen `Control` (clicks pass through except on its buttons) that draws the extra utility toolbar. |
| `custom_dialog_controls.gd` | Script wiring the buttons to Dialogic subsystems (History, Auto-Advance, Skip, Save/Load, Pause) and emitting a `menu_requested` signal. |
| `toolbar_button_style.tres` | Shared `StyleBoxFlat` for all toolbar buttons (glassmorphism panel). |

## Toolbar buttons (top-right corner so they never occlude the textbox)

| Label | Action |
|---|---|
| **H** | Opens/closes the Dialogic backlog (`Dialogic.History`) |
| **A** | Toggles Auto-Advance (`enabled_until_user_input`) |
| **»** | Toggles Auto-Skip (fast-forward through seen dialogue) |
| **S** | Quick Save to slot `"quick"` |
| **L** | Quick Load from slot `"quick"` |
| **❚❚** | Pause / Resume Dialogic |
| **≡** | Emits `menu_requested` (wired in `main.gd`) |

A small fading toast label under the toolbar confirms each action. Active
toggles tint bright cyan; inactive ones are dimmed grey-blue. The root
`Control` uses `mouse_filter = IGNORE` and the `HBoxContainer` is set to
`mouse_filter = IGNORE` too, so clicks outside the buttons still advance
dialogue normally (clicks hit the full-screen input layer below).

## How it is wired

The glassmorphism style (`res://themes/glassmorphism/chrono_nexus_style.tres`)
now references TWO custom scenes instead of the plugin defaults:

- Layer 13 (textbox) → `res://ui/dialog/glassmorphism_textbox_layer.tscn`
- Layer 18 (custom controls) → `res://ui/dialog/custom_dialog_controls.tscn`

No files inside `addons/dialogic/` were modified. No override dictionary
serialization is required anymore — glassmorphism panels are referenced
directly in the textbox scene file.

`main.gd` polls `Dialogic.Styles.get_layout_node()` once after startup and
connects the toolbar's `menu_requested` signal to a simple dimmed pause
overlay (click to dismiss); replace `_on_custom_menu()` with your real
menu implementation.

## Customizing further

- To add more buttons, duplicate a `Button` node inside `Toolbar` in
  `custom_dialog_controls.tscn`, give it a `%BtnXxx` unique name, add an
  `@onready` reference in the script, and wire its `pressed` signal.
- To move the toolbar, change the `offset_*` values on the `Toolbar` node.
  It is currently anchored to the top-right.
- To change the look, edit `toolbar_button_style.tres` or the
  `StyleBoxFlat_toast` sub-resource used by the toast in the `.tscn`.
- `glassmorphism_textbox_layer.tscn` can be freely restyled (it is your
  copy of the plugin scene). Change panel styleboxes, fonts, sizes, and
  animations directly in that file; they are applied at scene load with
  no override-dict indirection.
