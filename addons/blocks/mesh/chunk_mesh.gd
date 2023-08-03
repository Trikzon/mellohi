@tool
class_name ChunkMesh extends ArrayMesh


# One set of vertices, normals, and indices per surface.
var _vertices: Array[PackedVector3Array] = []
var _normals: Array[PackedVector3Array] = []
var _indices: Array[PackedInt32Array] = []

var _chunk: Chunk

# Neighboring chunks. Potentially null.
var _right: Chunk
var _left: Chunk
var _up: Chunk
var _down: Chunk
var _forward: Chunk
var _back: Chunk

# Stores the faces of the current side that have already been visited.
# Is reset when changing faces.
var _faces_visited = PackedByteArray()


func _init(chunk: Chunk = null):
    if not chunk: return
    
    _chunk = chunk
    
    # TODO: Change to use material count
    for i in range(Blocks.get_block_count()):
        _vertices.append(PackedVector3Array())
        _normals.append(PackedVector3Array())
        _indices.append(PackedInt32Array())


func generate(right: Chunk, left: Chunk, up: Chunk, down: Chunk, forward: Chunk, back: Chunk):
    _right = right
    _left = left
    _up = up
    _down = down
    _forward = forward
    _back = back
    
    _create_greedy_faces(Vector3i.RIGHT)
    _create_greedy_faces(Vector3i.LEFT)
    _create_greedy_faces(Vector3i.UP)
    _create_greedy_faces(Vector3i.DOWN)
    _create_greedy_faces(Vector3i.FORWARD)
    _create_greedy_faces(Vector3i.BACK)
    
    _build_surfaces()


func _create_greedy_faces(dir: Vector3i):
    _faces_visited = PackedByteArray()
    _faces_visited.resize(BlocksPluginOptions.CHUNK_VOLUME)
    for i in BlocksPluginOptions.CHUNK_VOLUME:
        _faces_visited.append(0)
    
    for y in range(BlocksPluginOptions.CHUNK_SIZE):
        for z in range(BlocksPluginOptions.CHUNK_SIZE):
            for x in range(BlocksPluginOptions.CHUNK_SIZE):
                var block = _chunk.get_block_index(x, y, z)
                
                if not BlockMaterials.should_render(block): continue
                
                var size = _find_greedy_quad_size(block, x, y, z, dir)
                
                if size.x <= 0 or size.y <= 0: continue
                
                match dir:
                    Vector3i.RIGHT: _add_right_quad(block, x, y, z, size.x, size.y)
                    Vector3i.LEFT: _add_left_quad(block, x, y, z, size.x, size.y)
                    Vector3i.UP: _add_up_quad(block, x, y, z, size.x, size.y)
                    Vector3i.DOWN: _add_down_quad(block, x, y, z, size.x, size.y)
                    Vector3i.FORWARD: _add_forward_quad(block, x, y, z, size.x, size.y)
                    Vector3i.BACK: _add_back_quad(block, x, y, z, size.x, size.y)


func _add_right_quad(block: int, x: int, y: int, z: int, length: int, width: int):
    var v0 = Vector3(x + 1, y,          z        )
    var v1 = Vector3(x + 1, y + length, z        )
    var v2 = Vector3(x + 1, y + length, z + width)
    var v3 = Vector3(x + 1, y,          z + width)
    
    _add_quad(block, [v0, v1, v2, v3], [0, 2, 1, 0, 3, 2], Vector3.RIGHT)


func _add_left_quad(block: int, x: int, y: int, z: int, length: int, width: int):
    var v0 = Vector3(x, y,          z        )
    var v1 = Vector3(x, y + length, z        )
    var v2 = Vector3(x, y + length, z + width)
    var v3 = Vector3(x, y,          z + width)
    
    _add_quad(block, [v0, v1, v2, v3], [0, 1, 2, 0, 2, 3], Vector3.LEFT)


func _add_up_quad(block: int, x: int, y: int, z: int, length: int, width: int):
    var v0 = Vector3(x,         y + 1, z         )
    var v1 = Vector3(x + width, y + 1, z         )
    var v2 = Vector3(x + width, y + 1, z + length)
    var v3 = Vector3(x,         y + 1, z + length)
    
    _add_quad(block, [v0, v1, v2, v3], [0, 1, 2, 0, 2, 3], Vector3.UP)


func _add_down_quad(block: int, x: int, y: int, z: int, length: int, width: int):
    var v0 = Vector3(x,         y, z         )
    var v1 = Vector3(x + width, y, z         )
    var v2 = Vector3(x + width, y, z + length)
    var v3 = Vector3(x,         y, z + length)
    
    _add_quad(block, [v0, v1, v2, v3], [0, 2, 1, 0, 3, 2], Vector3.DOWN)


func _add_forward_quad(block: int, x: int, y: int, z: int, length: int, width: int):
    var v0 = Vector3(x,         y,          z)
    var v1 = Vector3(x + width, y,          z)
    var v2 = Vector3(x + width, y + length, z)
    var v3 = Vector3(x,         y + length, z)
    
    _add_quad(block, [v0, v1, v2, v3], [0, 1, 2, 0, 2, 3], Vector3.FORWARD)


func _add_back_quad(block: int, x: int, y: int, z: int, length: int, width: int):
    var v0 = Vector3(x,         y,          z + 1)
    var v1 = Vector3(x + width, y,          z + 1)
    var v2 = Vector3(x + width, y + length, z + 1)
    var v3 = Vector3(x,         y + length, z + 1)
    
    _add_quad(block, [v0, v1, v2, v3], [0, 2, 1, 0, 3, 2], Vector3.BACK)


func _find_greedy_quad_size(block: int, x: int, y: int, z: int, dir: Vector3i) -> Vector2i:
    const chunk_size = BlocksPluginOptions.CHUNK_SIZE
    const chunk_area = BlocksPluginOptions.CHUNK_AREA
    var length = 0
    var width = 1
    
    # Return early if this face has already been visited or if the face is hidden.
    if _is_face_visited(x, y, z) or _is_face_hidden(x, y, z, dir):
        return Vector2i(length, width)
    
    # ===== 1D Greedy Mesh Pass =====
    # This pass iterates over blocks in one direction and checks if the blocks are the same.
    var visiting_pos = Vector3i(x, y, z)
    
    # If the direction is up or down, we want to iterate over the z-axis. Otherwise, the y-axis.
    var iter_dir: Vector3i
    if dir.abs().y == 1:
        iter_dir = Vector3i(0, 0, 1)
    else:
        iter_dir = Vector3i(0, 1, 0)
    var iter_axis = iter_dir.max_axis_index()
    var max_pos = iter_dir * chunk_size
    
    while (visiting_pos[iter_axis] < max_pos[iter_axis]):
        # If the block at the visiting position is not the same as the block who's face we're building, stop.
        # TODO: Also check if the block has the same light level
        if _chunk.get_block_indexv(visiting_pos) != block: break
        if _is_face_visitedv(visiting_pos):
            break
        if _is_face_hiddenv(visiting_pos, dir): break
        
        # Mark the faces as visited and extend the length by one
        _set_face_visitedv(visiting_pos)
        length += 1
        
        visiting_pos += iter_dir
    
    
    # ===== 2D Greedy Mesh Pass =====
    # This pass iterates in the other direction to see if the 1D mesh can be extended that way.
    
    # If the direction is left or right we want to iterate over the z-axis. Otherwise, the x-axis.
    var first_iter_dir: Vector3i
    if (dir.abs().x == 1):
        first_iter_dir = Vector3i(0, 0, 1)
    else:
        first_iter_dir = Vector3i(1, 0, 0)
    var first_iter_axis = first_iter_dir.max_axis_index()
    var first_max_pos = first_iter_dir * chunk_size
    
    # If the direction is up or down, we want to iterate over the z-axis. Otherwise, the y-axis.
    var second_iter_dir: Vector3i
    if (dir.abs().y == 1):
        second_iter_dir = Vector3i(0, 0, 1)
    else:
        second_iter_dir = Vector3i(0, 1, 0)
    var second_iter_axis = second_iter_dir.max_axis_index()
    
    # Start at the first block in the new direction.
    visiting_pos = Vector3i(x, y, z) + first_iter_dir
    
    # Save the initial value of the second iteration axis so we can reset it later.
    var initial_second_iter_value = visiting_pos[second_iter_axis]
    
    var stop = false
    while (visiting_pos[first_iter_axis] < first_max_pos[first_iter_axis] and not stop):
        # Check if the block is the same as the block who's quad we're building
        visiting_pos[second_iter_axis] = initial_second_iter_value
        while (visiting_pos[second_iter_axis] < initial_second_iter_value + length and not stop):
            stop = stop or _chunk.get_block_indexv(visiting_pos) != block
            stop = stop or _is_face_visitedv(visiting_pos)
            stop = stop or _is_face_hiddenv(visiting_pos, dir)
            
            visiting_pos += second_iter_dir
        
        if stop: continue
        
        # If not stop, that means the quad can be extended in this direction by one.
        width += 1
        
        # Mark all the blocks in this direction as visited
        visiting_pos[second_iter_axis] = initial_second_iter_value
        while (visiting_pos[second_iter_axis] < initial_second_iter_value + length):
            _set_face_visitedv(visiting_pos)
            
            visiting_pos += second_iter_dir
        
        visiting_pos += first_iter_dir
    
    return Vector2i(length, width)


func _is_face_visited(x: int, y: int, z: int) -> bool:
    return !!_faces_visited[y + z * BlocksPluginOptions.CHUNK_SIZE + x * BlocksPluginOptions.CHUNK_AREA]


func _is_face_visitedv(local_pos: Vector3i) -> bool:
    return _is_face_visited(local_pos.x, local_pos.y, local_pos.z)


func _set_face_visited(x: int, y: int, z: int):
    _faces_visited[y + z * BlocksPluginOptions.CHUNK_SIZE + x * BlocksPluginOptions.CHUNK_AREA] = 1


func _set_face_visitedv(local_pos: Vector3i):
    _set_face_visited(local_pos.x, local_pos.y, local_pos.z)


# x, y, and z should only be blocks inside this chunk represented as a local position
func _is_face_hidden(x: int, y: int, z: int, dir: Vector3i) -> bool:
    x += dir.x
    y += dir.y
    z += dir.z
    
    const chunk_size = BlocksPluginOptions.CHUNK_SIZE
    var materials = BlockMaterials
    
    if y >= chunk_size:
        return _up && materials.should_render(_up.get_block_index(x, 0, z))
    elif y < 0:
        return _down && materials.should_render(_down.get_block_index(x, chunk_size - 1, z))
    
    if x >= chunk_size:
        return _right && materials.should_render(_right.get_block_index(0, y, z))
    elif x < 0:
        return _left && materials.should_render(_left.get_block_index(chunk_size - 1, y, z))
    
    if z < 0:
        return _forward && materials.should_render(_forward.get_block_index(x, y, chunk_size - 1))
    elif z >= chunk_size:
        return _back && materials.should_render(_back.get_block_index(x, y, 0))
    
    return materials.should_render(_chunk.get_block_index(x, y, z))


func _is_face_hiddenv(local_pos: Vector3i, dir: Vector3i) -> bool:
    return _is_face_hidden(local_pos.x, local_pos.y, local_pos.z, dir)


# TODO: Add color data for light data
func _add_quad(block: int, vertices: PackedVector3Array, indices: PackedInt32Array, normal: Vector3):
    var index_offset = _vertices[block].size()
    # Add the four vertices.
    _vertices[block].append_array(vertices)
    # Add a normal for each of the four vertices.
    for i in range(4):
        _normals[block].append(normal)
    # Add the six indices to form two triangles from the four vertices.
    for index in indices:
        _indices[block].append(index + index_offset)


func _build_surfaces():
    for block in range(_vertices.size()):
        if _vertices[block].size() == 0: continue
        
        var surface = []
        surface.resize(Mesh.ARRAY_MAX)
        
        surface[Mesh.ARRAY_VERTEX] = _vertices[block]
        surface[Mesh.ARRAY_NORMAL] = _normals[block]
        surface[Mesh.ARRAY_INDEX] = _indices[block]
        
        var surface_index = get_surface_count()
        add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, surface)
        surface_set_material(surface_index, BlockMaterials.get_material(block))
