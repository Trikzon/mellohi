@tool
class_name Chunk
extends Resource

var blocks: PackedInt32Array = PackedInt32Array()

func _init() -> void:
    for y in range(MellohiSettings.CHUNK_SIZE):
        for z in range(MellohiSettings.CHUNK_SIZE):
            for x in range(MellohiSettings.CHUNK_SIZE):
                # blocks.append(1)
                blocks.append(randi_range(1, 3))
    

func get_block(x: int, y: int, z: int) -> int:
    return blocks[_local_to_index(x, y, z)]


func _local_to_index(x: int, y: int, z: int) -> int:
    const size = MellohiSettings.CHUNK_SIZE
    return y + z * size + x * size * size
