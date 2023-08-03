@tool
class_name Terrain extends Node3D


var _chunk_instances = Dictionary()  # Dictionary[Vector3i, ChunkInstance]


func _ready():
    get_viewport().debug_draw = Viewport.DEBUG_DRAW_WIREFRAME
    # TODO: Move block registry somewhere else
    Blocks.invalidate()
    Blocks.add_block(preload("res://data/blocks/air.tres"))
    Blocks.add_block(preload("res://data/blocks/stone.tres"))
    Blocks.add_block(preload("res://data/blocks/dirt.tres"))
    Blocks.add_block(preload("res://data/blocks/grass.tres"))
    
    for y in range(2):
        for z in range(2):
            for x in range(2):
                var chunk_instance = ChunkInstance.new(Vector3i(x, y, z))
                add_child(chunk_instance)
                _chunk_instances[Vector3i(x, y, z)] = chunk_instance
                
                chunk_instance.generate()
