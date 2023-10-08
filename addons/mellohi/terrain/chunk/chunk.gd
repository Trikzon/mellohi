@tool
class_name Chunk
extends Resource

var blocks: PackedInt32Array = PackedInt32Array()
var block_light: PackedByteArray = PackedByteArray()
var sunlight: PackedByteArray = PackedByteArray()


func _init() -> void:
    blocks.resize(Mellohi.CHUNK_VOL)
    block_light.resize(Mellohi.CHUNK_VOL)
    sunlight.resize(Mellohi.CHUNK_VOL)

    for y in range(Mellohi.CHUNK_LEN - 4):
        for z in range(Mellohi.CHUNK_LEN):
            for x in range(Mellohi.CHUNK_LEN):
                blocks[local_to_index(x, y, z)] = randi_range(0, 3)

    for y in range(Mellohi.CHUNK_LEN):
        for z in range(Mellohi.CHUNK_LEN):
            for x in range(Mellohi.CHUNK_LEN):
                block_light[local_to_index(x, y, z)] = randi_range(0, 31)


func get_block(x: int, y: int, z: int) -> int:
    return blocks[local_to_index(x, y, z)]


func get_blockv(local_pos: Vector3i) -> int:
    return get_block(local_pos.x, local_pos.y, local_pos.z)


func get_block_light(x: int, y: int, z: int) -> int:
    return block_light[local_to_index(x, y, z)]


func get_block_lightv(local_pos: Vector3i) -> int:
    return get_block_light(local_pos.x, local_pos.y, local_pos.z)


func set_block_light(x: int, y: int, z: int, value: int) -> void:
    block_light[local_to_index(x, y, z)] = value


func set_block_lightv(local_pos: Vector3i, value: int) -> void:
    set_block_light(local_pos.x, local_pos.y, local_pos.z, value)

func get_sunlight(x: int, y: int, z: int) -> int:
    return sunlight[local_to_index(x, y, z)]


func get_sunlightv(local_pos: Vector3i) -> int:
    return get_sunlight(local_pos.x, local_pos.y, local_pos.z)


func set_sunlight(x: int, y: int, z: int, value: int) -> void:
    sunlight[local_to_index(x, y, z)] = value


func set_sunlightv(local_pos: Vector3i, value: int) -> void:
    set_sunlight(local_pos.x, local_pos.y, local_pos.z, value)


static func local_to_index(x: int, y: int, z: int) -> int:
    return y + z * Mellohi.CHUNK_LEN + x * Mellohi.CHUNK_AREA


static func local_to_indexv(local_pos: Vector3i) -> int:
    return local_to_index(local_pos.x, local_pos.y, local_pos.z)
