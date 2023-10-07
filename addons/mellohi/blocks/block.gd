@tool
class_name Block
extends Resource

@export var id: Identifier = Identifier.new()

@export var material: BlockMaterial

func _to_string():
    return "Block[%s]" % id
