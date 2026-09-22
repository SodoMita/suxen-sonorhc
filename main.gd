extends Node2D

# Chrono Nexus — main entry point.
# Loads the 3D background scene, applies the glassmorphism Dialogic
# style, registers a custom event handler for [mind:] steps that
# integrates with GameState's mind-read mechanic, and starts the story.

const NEXUS_3D_PATH := "res://scenes/3d/nexus_3d.tscn"
const STYLE_NAME := "Chrono Nexus (Glassmorphism)"

var _menu_overlay: Control = null
var _mind_label: Label = null


func _ready() -> void:
	# Small delay to ensure everything (3D scene, autoloads, etc.)
	# has finished initializing.
	await get_tree().create_timer(0.2).timeout

	# Apply the kawaii glassmorphism theme (it's already the default in
	# project.godot, but call this defensively in case the user
	# changes the default).
	Dialogic.Styles.change_style(STYLE_NAME)

	# Connect the mind-read mechanic to the Dialogic timeline engine.
	Dialogic.timeline_ended.connect(_on_timeline_ended)
	Dialogic.signal_event.connect(_on_signal_event)

	# Hook the custom dialog toolbar's Menu button once the layout is built.
	_connect_custom_toolbar()

	# Start the campaign.
	Dialogic.start("res://timelines/TL_001_Opening_RiftSchool.dtl")


# The custom toolbar layer is instantiated by Dialogic after the style loads,
# so we poll briefly for it and then wire the menu_requested signal.
func _connect_custom_toolbar() -> void:
	for _i in range(30):
		var layout = (
			Dialogic.Styles.get_layout_node() if Dialogic.Styles.has_active_layout_node() else null
		)
		if layout and layout.has_node("CustomDialogControls"):
			var toolbar = layout.get_node("CustomDialogControls")
			if toolbar.has_signal(&"menu_requested"):
				toolbar.menu_requested.connect(_on_custom_menu)
			return
		await get_tree().process_frame


# Placeholder menu overlay — extend this with your actual pause menu.
func _on_custom_menu() -> void:
	if _menu_overlay and is_instance_valid(_menu_overlay):
		_menu_overlay.queue_free()
		_menu_overlay = null
		return
	_menu_overlay = Control.new()
	_menu_overlay.name = "CustomMenuOverlay"
	_menu_overlay.set_anchors_preset(Control.PRESET_FULL_RECT)
	_menu_overlay.mouse_filter = Control.MOUSE_FILTER_STOP
	var dim := ColorRect.new()
	dim.color = Color(0, 0, 0, 0.55)
	dim.set_anchors_preset(Control.PRESET_FULL_RECT)
	_menu_overlay.add_child(dim)
	var label := Label.new()
	label.text = "⏸  Menu\n(click to close — hook _on_custom_menu() to open your real menu)"
	label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	label.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
	label.add_theme_color_override("font_color", Color.WHITE)
	label.add_theme_font_size_override("font_size", 20)
	label.set_anchors_preset(Control.PRESET_FULL_RECT)
	_menu_overlay.add_child(label)
	_menu_overlay.gui_input.connect(
		func(ev):
			if ev is InputEventMouseButton and ev.pressed:
				_menu_overlay.queue_free()
				_menu_overlay = null
	)
	get_tree().root.add_child(_menu_overlay)


func _on_timeline_ended(_timeline: Resource) -> void:
	pass


func _on_signal_event(argument: String) -> void:
	var parts: PackedStringArray = argument.strip_edges().split(" ", false)
	if parts.is_empty():
		return
	match parts[0]:
		"add_bond":
			if parts.size() >= 3:
				GameState.add_bond(parts[1], int(parts[2]))
		"add_trust":
			if parts.size() >= 2:
				GameState.add_trust(int(parts[1]))
		"add_insight":
			if parts.size() >= 2:
				GameState.add_insight(int(parts[1]))
		"add_power":
			if parts.size() >= 2:
				GameState.add_power(int(parts[1]))
		"mind_read":
			if parts.size() >= 2 and GameState.can_read_mind(parts[1]):
				_show_mind_read(parts[1])
		_:
			push_warning("Unknown Dialogic event: " + argument)


# Thought-bubble overlay for [mind:] events.
func _show_mind_read(character_id: String) -> void:
	if _mind_label == null:
		_mind_label = Label.new()
		_mind_label.name = "MindReadLabel"
		_mind_label.set_anchors_preset(Control.PRESET_BOTTOM_RIGHT)
		_mind_label.position = Vector2(-340, -100)
		_mind_label.size = Vector2(320, 80)
		_mind_label.add_theme_color_override("font_color", Color(0.78, 0.82, 1.0, 0.92))
		_mind_label.add_theme_font_size_override("font_size", 13)
		_mind_label.add_theme_constant_override("line_spacing", 4)
		_mind_label.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
		_mind_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_RIGHT
		var stylebox: StyleBoxFlat = StyleBoxFlat.new()
		stylebox.bg_color = Color(0.04, 0.04, 0.10, 0.78)
		stylebox.corner_radius_top_left = 14
		stylebox.corner_radius_top_right = 14
		stylebox.corner_radius_bottom_left = 14
		stylebox.corner_radius_bottom_right = 14
		stylebox.border_color = Color(0.47, 0.51, 0.86, 0.4)
		stylebox.set_border_width_all(1)
		stylebox.content_margin_left = 14
		stylebox.content_margin_top = 10
		stylebox.content_margin_right = 14
		stylebox.content_margin_bottom = 10
		_mind_label.add_theme_stylebox_override("normal", stylebox)
		add_child(_mind_label)
	var names: Dictionary = {
		"aurora": "Aurora",
		"aria": "Aria",
		"elara": "Elara",
		"kira": "Kira",
		"nova": "Nova",
		"selene": "Selene",
	}
	var name_label: String = names.get(character_id, character_id.capitalize())
	_mind_label.text = "%s // mind-read" % name_label
	# Reset opacity and fade after 5 seconds.
	_mind_label.modulate.a = 1.0
	var tween: Tween = create_tween()
	tween.tween_property(_mind_label, "modulate:a", 0.0, 0.6).set_delay(5.0)
