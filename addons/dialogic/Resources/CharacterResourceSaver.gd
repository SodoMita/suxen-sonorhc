@tool
class_name DialogicCharacterFormatSaver
extends ResourceFormatSaver


func _get_recognized_extensions(_resource: Resource) -> PackedStringArray:
	return PackedStringArray(["dch"])


## Return true if this resource should be loaded as a DialogicCharacter
func _recognize(resource: Resource) -> bool:
	# Cast instead of using "is" keyword in case is a subclass
	resource = resource as DialogicCharacter

	if resource:
		return true

	return false


## Save the resource
func _save(resource: Resource, path: String = '', _flags: int = 0) -> Error:
	var file := FileAccess.open(path, FileAccess.WRITE)

	if file == null:
		var err := FileAccess.get_open_error()
		push_error("[Dialogic] Error opening file: %s" % err)
		return err

	var data := {}
	for property in resource.get_property_list():
		# Only store real, serializable properties
		if property["usage"] & PROPERTY_USAGE_STORAGE:
			data[property["name"]] = resource.get(property["name"])

	file.store_string(var_to_str(data))
	# print('[Dialogic] Saved character "' , path, '"')
	return OK
