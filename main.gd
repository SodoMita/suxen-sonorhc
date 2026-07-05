extends Node2D

# Chrono Nexus — main entry point.
# Loads the 3D background scene, applies the glassmorphism Dialogic
# style, registers a custom event handler for [mind:] steps that
# integrates with GameState's mind-read mechanic, and starts the story.

const NEXUS_3D_PATH := "res://scenes/3d/nexus_3d.tscn"
const STYLE_NAME := "Chrono Nexus (Glassmorphism)"

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

	# Start the campaign.
	Dialogic.start("res://timelines/TL_001_Opening_RiftSchool.dtl")


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
var _mind_label: Label = null
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
