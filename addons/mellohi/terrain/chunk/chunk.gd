@tool
class_name Chunk
extends Resource

var blocks: PackedInt32Array = PackedInt32Array()

func _init() -> void:
    for y in range(Mellohi.CHUNK_LEN):
        for z in range(Mellohi.CHUNK_LEN):
            for x in range(Mellohi.CHUNK_LEN):
                blocks.append(randi_range(0, 3))
    

func get_block(x: int, y: int, z: int) -> int:
    return blocks[local_to_index(x, y, z)]


func get_blockv(pos: Vector3i) -> int:
    return get_block(pos.x, pos.y, pos.z)


static func local_to_index(x: int, y: int, z: int) -> int:
    return y + z * Mellohi.CHUNK_LEN + x * Mellohi.CHUNK_AREA


static func local_to_indexv(local_pos: Vector3i) -> int:
    return local_to_index(local_pos.x, local_pos.y, local_pos.z)
