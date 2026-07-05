extends DialogicBackgroundTransition

# Glitch / rift transition: flips between previous and next
# background in horizontal bands with static noise. Used to mark
# "rift" cuts in the story (time / space tearing open).

func _fade() -> void:
	var shader := set_shader(this_folder.path_join("glitch_transition.gdshader"))
	shader.set_shader_parameter("previous_background", prev_texture)
	shader.set_shader_parameter("next_background", next_texture)
	tween_shader_progress()
