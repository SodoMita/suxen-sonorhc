extends Node2D

func _ready() -> void:
	# Small delay to ensure everything is ready
	await get_tree().create_timer(0.2).timeout
	
	# Start the Chrono Nexus campaign from the official introduction
	Dialogic.start("res://TL_001_Opening_RiftSchool.dtl")
