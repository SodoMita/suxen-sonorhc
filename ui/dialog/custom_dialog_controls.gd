@tool
extends DialogicLayoutLayer
## Custom Dialogic UI layer: adds a control bar with extra buttons that floats
## on top of the Dialogic UI. This file lives OUTSIDE the addons/dialogic
## folder so the plugin is never modified.
##
## Buttons (laid out along the bottom-right edge of the screen):
##   - History      opens/closes the Dialogic backlog
##   - Auto         toggles Auto-Advance  (remains on until the next click)
##   - Skip         toggles Auto-Skip     (fast-forward through seen text)
##   - Save         quick-saves to slot "quick"
##   - Load         quick-loads from slot "quick"
##   - Pause        pauses / resumes Dialogic
##   - Menu         emits a `menu_requested` signal for your game to hook into
##
## The layer is transparent to clicks except on its buttons, so clicking the
## rest of the screen still advances dialogue normally.

signal menu_requested

const BUTTON_SIZE := Vector2(36, 36)
const BUTTON_GAP := 6.0
const MARGIN := Vector2(20.0, 20.0)

var _history_open: bool = false
var _toast_tween: Tween = null

@onready var _toolbar: HBoxContainer = %Toolbar
@onready var _btn_history: Button = %BtnHistory
@onready var _btn_auto: Button = %BtnAuto
@onready var _btn_skip: Button = %BtnSkip
@onready var _btn_save: Button = %BtnSave
@onready var _btn_load: Button = %BtnLoad
@onready var _btn_pause: Button = %BtnPause
@onready var _btn_menu: Button = %BtnMenu
@onready var _toast: Label = %Toast


func _ready() -> void:
	if Engine.is_editor_hint():
		return

	# Wait until Dialogic's subsystems are ready before wiring signals.
	if not Dialogic.Inputs:
		await Dialogic.ready
	await get_tree().process_frame

	_btn_history.pressed.connect(_on_history)
	_btn_auto.pressed.connect(_on_toggle_auto)
	_btn_skip.pressed.connect(_on_toggle_skip)
	_btn_save.pressed.connect(_on_save)
	_btn_load.pressed.connect(_on_load)
	_btn_pause.pressed.connect(_on_toggle_pause)
	_btn_menu.pressed.connect(
		func():
			menu_requested.emit()
			_show_toast("Menu")
	)

	# Track history open/close via subsystem signals.
	if Dialogic.History.has_signal(&"open_requested"):
		Dialogic.History.open_requested.connect(func(): _history_open = true)
	if Dialogic.History.has_signal(&"close_requested"):
		Dialogic.History.close_requested.connect(func(): _history_open = false)

	# Track pause state.
	if Dialogic.has_signal(&"dialogic_paused"):
		Dialogic.dialogic_paused.connect(_refresh_pause_button)
	if Dialogic.has_signal(&"dialogic_resumed"):
		Dialogic.dialogic_resumed.connect(_refresh_pause_button)

	# Track auto-advance / auto-skip state changes (so buttons stay in sync
	# even if something else toggles them).
	Dialogic.Inputs.auto_advance.toggled.connect(func(_en): _refresh_auto_button())
	Dialogic.Inputs.auto_skip.toggled.connect(_refresh_skip_button)

	_refresh_auto_button()
	_refresh_skip_button()
	_refresh_pause_button()


# ---------------------------------------------------------------------------
# Button actions
# ---------------------------------------------------------------------------


func _on_history() -> void:
	if _history_open:
		Dialogic.History.close_history()
		_history_open = false
	else:
		Dialogic.History.open_history()
		_history_open = true
	_show_toast("History")


func _on_toggle_auto() -> void:
	# `enabled_until_user_input` is the flag meant for the player toggling
	# auto-advance; it stays on until the next click.
	Dialogic.Inputs.auto_advance.enabled_until_user_input = not (
		Dialogic.Inputs.auto_advance.enabled_until_user_input
	)
	_show_toast(
		"Auto-Advance " + ("ON" if Dialogic.Inputs.auto_advance.enabled_until_user_input else "OFF")
	)


func _on_toggle_skip() -> void:
	Dialogic.Inputs.auto_skip.enabled = not Dialogic.Inputs.auto_skip.enabled
	_show_toast("Skip " + ("ON" if Dialogic.Inputs.auto_skip.enabled else "OFF"))


func _on_save() -> void:
	var slot := "quick"
	var err := Dialogic.Save.save(slot)
	if err == OK:
		_show_toast("Saved: " + slot)
	else:
		_show_toast("Save failed (err " + str(err) + ")")


func _on_load() -> void:
	var slot := "quick"
	if not Dialogic.Save.has_slot(slot):
		_show_toast("No save in '" + slot + "'")
		return
	_show_toast("Loading " + slot + "...")
	# Give the toast a moment to show before loading.
	await get_tree().create_timer(0.25).timeout
	Dialogic.Save.load(slot)


func _on_toggle_pause() -> void:
	Dialogic.paused = not Dialogic.paused
	_show_toast("Paused" if Dialogic.paused else "Resumed")


# ---------------------------------------------------------------------------
# Visual helpers
# ---------------------------------------------------------------------------


func _refresh_auto_button() -> void:
	_toggle_style(_btn_auto, Dialogic.Inputs.auto_advance.is_enabled())


func _refresh_skip_button() -> void:
	_toggle_style(_btn_skip, Dialogic.Inputs.auto_skip.enabled)


func _refresh_pause_button() -> void:
	_toggle_style(_btn_pause, Dialogic.paused)


func _toggle_style(btn: Button, active: bool) -> void:
	# Brighter / bluer when active, dimmer grey-blue when off.
	btn.modulate = Color(0.55, 0.9, 1.0, 1.0) if active else Color(0.82, 0.86, 1.0, 0.8)


func _show_toast(msg: String) -> void:
	_toast.text = msg
	_toast.modulate.a = 1.0
	if _toast_tween:
		_toast_tween.kill()
	_toast_tween = create_tween()
	_toast_tween.tween_interval(1.2)
	_toast_tween.tween_property(_toast, "modulate:a", 0.0, 0.5)
