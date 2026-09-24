extends Node
## Chrono Nexus entry. A still of the nexus sits behind a glass title card.
## Starting a route hides the card and opens the Dialogue Manager balloon.

const STORY: DialogueResource = preload("res://dialogue/suxen_onorhc.dialogue")

@onready var title_layer: CanvasLayer = $TitleLayer


func _ready() -> void:
	var dm: Node = Engine.get_singleton("DialogueManager")
	if dm != null and not dm.dialogue_ended.is_connected(_on_dialogue_ended):
		dm.dialogue_ended.connect(_on_dialogue_ended)
	_play_title_theme()


func _unhandled_input(event: InputEvent) -> void:
	if title_layer == null or not title_layer.visible:
		return
	if event.is_action_pressed(&"dialogue_advance") or event.is_action_pressed(&"ui_accept"):
		_start("opening")
		get_viewport().set_input_as_handled()


func _start(cue: String) -> void:
	title_layer.hide()
	Engine.get_singleton("DialogueManager").show_dialogue_balloon(STORY, cue)


func _on_begin_pressed() -> void:
	_start("начало")


func _on_dialogue_ended(_resource: Resource) -> void:
	if is_instance_valid(title_layer):
		title_layer.show()
	_play_title_theme()


func _play_title_theme() -> void:
	var audio := get_node_or_null("/root/AudioDirector")
	if audio != null and audio.has_method("play_scene"):
		audio.play_scene("nexus")
