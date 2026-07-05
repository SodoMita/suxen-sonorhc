extends Node3D

# Nexus3D — the 3D background for the Chrono Nexus citadel scene.
# Renders a shader-based skybox with a floating crystal cluster.

@export var animation_speed: float = 1.0

var _time: float = 0.0
var _sky_material: ShaderMaterial
var _crystals: Array[MeshInstance3D] = []
var _base_positions: PackedVector3Array

func _ready() -> void:
	_sky_material = $Sky.material_override as ShaderMaterial
	for child in $Crystals.get_children():
		_crystals.append(child)
	_base_positions = PackedVector3Array()
	for c in _crystals:
		_base_positions.append(c.position)

func _process(delta: float) -> void:
	_time += delta * animation_speed
	if _sky_material:
		_sky_material.set_shader_parameter("uTime", _time)
	for i in _crystals.size():
		var crystal: MeshInstance3D = _crystals[i]
		crystal.rotation.y += 0.002 * (0.5 + sin(float(i)) * 0.5)
		crystal.rotation.x += 0.001 * cos(float(i) * 0.7)
		var base: Vector3 = _base_positions[i]
		crystal.position.y = base.y + sin(_time * 0.5 + float(i)) * 0.0008
	$Camera3D.position.x = sin(_time * 0.15) * 0.5
	$Camera3D.position.y = 1.5 + sin(_time * 0.3) * 0.15
	$Camera3D.look_at(Vector3(0, 0.5, 0), Vector3.UP)
