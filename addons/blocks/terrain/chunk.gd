@tool
class_name Chunk extends Resource


static var _size: int
static var _area: int
static var _volume: int

var _chunk_position: Vector3i
var _block_indices: PackedInt32Array
var _light_levels: PackedInt32Array


func _init(chunk_position: Vector3i = Vector3i(), copy_chunk: Chunk = null):
    # Should never change. Only set here to shorten code lines because they are used a lot.
    _size = BlocksPluginOptions.CHUNK_SIZE
    _area = BlocksPluginOptions.CHUNK_AREA
    _volume = BlocksPluginOptions.CHUNK_VOLUME
    
    _chunk_position = chunk_position
    
    if copy_chunk:
        _block_indices = PackedInt32Array(copy_chunk._block_indices)
        _light_levels = PackedInt32Array(copy_chunk._light_levels)
    else:
        _init_arrays()


func clone() -> Chunk:
    return Chunk.new(_chunk_position, self)
    
    
    
func _init_arrays():
    _block_indices = PackedInt32Array()
    _block_indices.resize(_volume)
    _light_levels = PackedInt32Array()
    _light_levels.resize(_volume)
    
    for y in range(_size):
        for z in range(_size):
            for x in range(_size):
                _block_indices[y + z * _size + x * _area] = 0
                _light_levels[y + z * _size + x * _area] = 0


func chunk_position() -> Vector3i:
    return _chunk_position


func set_block(block: Block, x: int, y: int, z: int):
    _block_indices[y + z * _size + x * _area] = block.index


func set_blockv(block: Block, local_pos: Vector3i):
    _block_indices[local_pos.y + local_pos.z * _size + local_pos.x * _area] = block.index


func get_block_index(x: int, y: int, z: int) -> int:
    return _block_indices[y + z * _size + x * _area]


func get_block_indexv(local_pos: Vector3i) -> int:
    return _block_indices[local_pos.y + local_pos.z * _size + local_pos.x * _area]


func get_block(x: int, y: int, z: int) -> Block:
    var index = _block_indices[y + z * _size + x * _area]
    return Blocks.index_to_block(index)


func get_blockv(local_pos: Vector3i) -> Block:
    var index = _block_indices[local_pos.y + local_pos.z * _size + local_pos.x * _area]
    return Blocks.index_to_block(index)
