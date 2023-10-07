@tool
extends Node

var _id_to_block: Dictionary = {}  # Dictionary[String, int]

var resources: Array[Block] = []

func _ready():
	_id_to_block = {}
	resources = []

	_temp_register()


func _temp_register():
	register(load("res://addons/mellohi/blocks/air_block.tres"))
	register(load("res://game/blocks/dirt_block.tres"))
	register(load("res://game/blocks/stone_block.tres"))
	register(load("res://game/blocks/grass_block.tres"))


func register(resource: Block):
	var block = _id_to_block.size()
	_id_to_block[resource.id.to_string()] = block
	resources.append(resource)


func get_block_from_id(id: Identifier):
	return _id_to_block.get(id.to_string(), 0)
