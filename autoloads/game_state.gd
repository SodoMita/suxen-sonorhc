extends Node
## Story state for Chrono Nexus. Dialogue files use `using GameState`.
## `snapshot` / `restore` are what the balloon's rollback and saves call.

signal stat_changed(stat_name: String, value: int)
signal bond_changed(character_id: String, value: int)
signal mind_read_unlocked(unlocked: bool)

@export var player_name: String = "Ren"
@export var trust: int = 0
@export var insight: int = 0
@export var power: int = 0
@export var steadied_aurora: bool = false
@export var reached_for_kira: bool = false
@export var comforted_elara: bool = false
@export var teased_selene: bool = false
@export var supported_aurora: bool = false
@export var protected_nova: bool = false
## Fixed for the playthrough. A from-start replay uses this, not a fresh roll,
## so the same choices produce the same random results. Saves keep it inside
## each history snapshot.
@export var story_seed: int = 1
var rng := RandomNumberGenerator.new()

var bond: Dictionary = {}


func _ready() -> void:
	_reseed()
	# Dialogue Manager is a later autoload, so the first reseed cannot see it yet.
	call_deferred("_reseed")


func _reseed() -> void:
	rng.seed = story_seed
	seed(story_seed)
	var manager := get_tree().root.get_node_or_null("DialogueManager")
	if manager != null and manager.has_method("reseed_randomizer"):
		manager.reseed_randomizer(story_seed)


func can_read_mind(character_id: String) -> bool:
	if character_id.to_lower() == "aurora" and get_bond("aurora") >= 5:
		return true
	if insight >= 10:
		return true
	return false


func get_bond(character_id: String) -> int:
	return int(bond.get(character_id.to_lower(), 0))


func add_bond(character_id: String, amount: int) -> void:
	character_id = character_id.to_lower()
	var new_value: int = get_bond(character_id) + amount
	bond[character_id] = new_value
	bond_changed.emit(character_id, new_value)
	if character_id == "aurora" and new_value >= 5:
		mind_read_unlocked.emit(true)


func add_trust(amount: int) -> void:
	trust += amount
	stat_changed.emit("trust", trust)


func add_insight(amount: int) -> void:
	insight += amount
	stat_changed.emit("insight", insight)
	if insight >= 10:
		mind_read_unlocked.emit(true)


func add_power(amount: int) -> void:
	power += amount
	stat_changed.emit("power", power)


func snapshot() -> Dictionary:
	var data := {
		"player_name": player_name,
		"trust": trust,
		"insight": insight,
		"power": power,
		"steadied_aurora": steadied_aurora,
		"reached_for_kira": reached_for_kira,
		"comforted_elara": comforted_elara,
		"teased_selene": teased_selene,
		"supported_aurora": supported_aurora,
		"protected_nova": protected_nova,
		"story_seed": story_seed,
		"bond": bond.duplicate(true),
	}
	# Strings, not raw ints: a save is JSON, and a 64-bit RNG state does not survive a number.
	data["rng_state"] = str(rng.state)
	var manager := get_tree().root.get_node_or_null("DialogueManager")
	if manager != null:
		var stream = manager.get("_rng")
		if stream != null:
			data["dm_rng_state"] = str(stream.state)
	return data


func restore(data: Dictionary) -> void:
	player_name = str(data.get("player_name", "Ren"))
	trust = int(data.get("trust", 0))
	insight = int(data.get("insight", 0))
	power = int(data.get("power", 0))
	steadied_aurora = bool(data.get("steadied_aurora", false))
	reached_for_kira = bool(data.get("reached_for_kira", false))
	comforted_elara = bool(data.get("comforted_elara", false))
	teased_selene = bool(data.get("teased_selene", false))
	supported_aurora = bool(data.get("supported_aurora", false))
	protected_nova = bool(data.get("protected_nova", false))
	if data.has("story_seed"):
		story_seed = int(data["story_seed"])
	var saved: Variant = data.get("bond", {})
	bond = (saved as Dictionary).duplicate(true) if saved is Dictionary else {}
	if data.has("rng_state"):
		rng.state = int(data["rng_state"])
	else:
		_reseed()
	if data.has("dm_rng_state"):
		var manager := get_tree().root.get_node_or_null("DialogueManager")
		if manager != null:
			var stream = manager.get("_rng")
			if stream != null:
				stream.state = int(data["dm_rng_state"])
	stat_changed.emit("trust", trust)
	stat_changed.emit("insight", insight)
	stat_changed.emit("power", power)


func reset() -> void:
	var seed_now := story_seed
	restore({})
	story_seed = seed_now
	_reseed()
