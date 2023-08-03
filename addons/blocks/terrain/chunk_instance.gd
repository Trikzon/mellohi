@tool
class_name ChunkInstance extends StaticBody3D


signal chunk_data_changed(chunk: Chunk)


var _chunk: Chunk
var _meshInstance: MeshInstance3D
var _collision_shape: CollisionShape3D


func _init(chunk_pos: Vector3i = Vector3()):
    _chunk = Chunk.new(chunk_pos)
    _meshInstance = MeshInstance3D.new()
    _collision_shape = CollisionShape3D.new()
    ChunkMeshManager.register_chunk(chunk_pos, self)


func _ready():
    position = chunk_position() * BlocksPluginOptions.CHUNK_SIZE
    add_child(_meshInstance)
    add_child(_collision_shape)


func _enter_tree():
    ChunkMeshManager.register_chunk(_chunk.chunk_position(), self)


func _exit_tree():
    ChunkMeshManager.unregister_chunk(_chunk.chunk_position())


func generate():
    var stone = Blocks.id_to_block("stone")
    var dirt = Blocks.id_to_block("dirt")
    var grass = Blocks.id_to_block("grass")
    
    for y in range(BlocksPluginOptions.CHUNK_SIZE):
        for z in range(BlocksPluginOptions.CHUNK_SIZE):
            for x in range(BlocksPluginOptions.CHUNK_SIZE):
                if y == BlocksPluginOptions.CHUNK_SIZE - 1:
                    _chunk.set_block(grass, x, y, z)
                elif y >= BlocksPluginOptions.CHUNK_SIZE - 3:
                    _chunk.set_block(dirt, x, y, z)
                else:
                    _chunk.set_block(stone, x, y, z)
    
    chunk_data_changed.emit(_chunk)


func set_chunk_mesh(mesh: Mesh, shape: Shape3D):
    _meshInstance.mesh = mesh
    _collision_shape.shape = shape


func chunk_position() -> Vector3i:
    return _chunk.chunk_position()


func set_block(block: Block, x: int, y: int, z: int):
    _chunk.set_block(block, x, y, z)
    chunk_data_changed.emit(_chunk)


func set_blockv(block: Block, local_pos: Vector3i):
    _chunk.set_blockv(block, local_pos)
    chunk_data_changed.emit(_chunk)


func get_block(x: int, y: int, z: int) -> Block:
    return _chunk.get_block(x, y, z)
