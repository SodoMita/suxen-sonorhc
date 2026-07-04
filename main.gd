extends Node2D

func _ready() -> void:
	# Small delay to ensure everything is ready
	await get_tree().create_timer(0.2).timeout
	
	# Start the Dialogic 2 timeline
	# This automatically instantiates the Dialogic UI, loads the background,
	# and manages character portraits, text boxes, and choice menus.
	Dialogic.start("res://main_timeline.dtl")
