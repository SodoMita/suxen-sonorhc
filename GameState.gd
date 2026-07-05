extends Node

# GameState autoload — tracks stats, bonds, and the mind-read ability.
# Singleton, available everywhere as `GameState`.

signal stat_changed(stat_name: String, value: int)
signal bond_changed(character_id: String, value: int)
signal mind_read_unlocked(unlocked: bool)

# The three primary stats from the blueprint.
var trust: int = 0
var insight: int = 0
var power: int = 0

# Per-character bonds.
var bond: Dictionary = {}

# The "mind" ability is unlocked when:
#   * bond.aurora >= 5 (Aurora's mind is "open source" once you trust her)
#   * OR insight >= 10 (very high insight unlocks the ability for anyone)
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

func reset() -> void:
	trust = 0
	insight = 0
	power = 0
	bond = {}
