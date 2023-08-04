@tool
class_name Terrain extends Node3D


var _chunk_instances = Dictionary()  # Dictionary[Vector3i, ChunkInstance]


func _ready():
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


func set_block(block: Block, x: int, y: int, z: int):
    set_blockv(block, Vector3i(x, y, z))


func set_blockv(block: Block, global_pos: Vector3i):
    var chunk_pos = global_to_chunk_pos(global_pos)
    if _chunk_instances.has(chunk_pos):
        var chunk: ChunkInstance = _chunk_instances[chunk_pos]
        chunk.set_blockv(block, global_to_local_pos(global_pos))
    # TODO: Do something if the chunk does not exist yet


func global_to_local_pos(global_pos: Vector3i) -> Vector3i:
    const chunk_size = BlocksPluginOptions.CHUNK_SIZE
    return Vector3i(
        posmod(global_pos.x, chunk_size),
        posmod(global_pos.y, chunk_size),
        posmod(global_pos.z, chunk_size)
    )


func global_to_chunk_pos(global_pos: Vector3i) -> Vector3i:
    const chunk_size = BlocksPluginOptions.CHUNK_SIZE
    return Vector3i(
        floor(global_pos.x / chunk_size),
        floor(global_pos.y / chunk_size),
        floor(global_pos.z / chunk_size)
    )
