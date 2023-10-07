@tool
extends Node


class ChunkMeshData:
    var chunk: Chunk

    var vertices: Array[PackedVector3Array] = []
    var normals: Array[PackedVector3Array] = []
    var indices: Array[PackedInt32Array] = []

    var faces_visited = PackedByteArray()

    func _init(chunk: Chunk):
        self.chunk = chunk

        # TODO: Replace with surface count.
        for i in range(BlockRegistry.resources.size()):
            vertices.append(PackedVector3Array())
            normals.append(PackedVector3Array())
            indices.append(PackedInt32Array())


func generate_mesh(chunk: Chunk) -> Mesh:
    var data = ChunkMeshData.new(chunk)

    _create_greedy_mesh(data, Vector3i.RIGHT)
    _create_greedy_mesh(data, Vector3i.LEFT)
    _create_greedy_mesh(data, Vector3i.UP)
    _create_greedy_mesh(data, Vector3i.DOWN)
    _create_greedy_mesh(data, Vector3i.FORWARD)
    _create_greedy_mesh(data, Vector3i.BACK)

    return _build_surfaces(data)


func _create_greedy_mesh(data: ChunkMeshData, dir: Vector3i) -> void:
    var chunk_size = MellohiSettings.CHUNK_SIZE

    data.faces_visited = PackedByteArray()
    data.faces_visited.resize(chunk_size * chunk_size * chunk_size)

    for i in range(chunk_size * chunk_size * chunk_size):
        data.faces_visited.append(0)
    
    for y in range(chunk_size):
        for z in range(chunk_size):
            for x in range(chunk_size):
                var block = data.chunk.get_block(x, y, z)

                if block == 0:
                    continue

                var size = _find_greedy_quad_size(data, block, x, y, z, dir)

                if size.x <= 0 or size.y <= 0:
                    continue
                
                match dir:
                    Vector3i.RIGHT:
                        _add_right_quad(data, block, x, y, z, size.x, size.y)
                    Vector3i.LEFT:
                        _add_left_quad(data, block, x, y,z, size.x, size.y)
                    Vector3i.UP:
                        _add_up_quad(data, block, x, y,z, size.x, size.y)
                    Vector3i.DOWN:
                        _add_down_quad(data, block, x, y,z, size.x, size.y)
                    Vector3i.FORWARD:
                        _add_forward_quad(data, block, x, y,z, size.x, size.y)
                    Vector3i.BACK:
                        _add_back_quad(data, block, x, y,z, size.x, size.y)


func _find_greedy_quad_size(data: ChunkMeshData, block: int, x: int, y: int, z: int, dir: Vector3i) -> Vector2i:
    const chunk_size = MellohiSettings.CHUNK_SIZE

    var length = 0
    var width = 1

    # Return early if this face has already been visited or if the face is hidden.
    if _is_face_visited(data, x, y, z) or _is_face_hidden(data, x, y, z, dir):
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
        # TODO: Also check if the block has the same rotation, light level, etc.
        if data.chunk.get_block(visiting_pos.x, visiting_pos.y, visiting_pos.z) != block:
            break
        
        if _is_face_visited(data, visiting_pos.x, visiting_pos.y, visiting_pos.z):
            break
        
        if _is_face_hidden(data, visiting_pos.x, visiting_pos.y, visiting_pos.z, dir):
            break
        
        _set_face_visited(data, visiting_pos.x, visiting_pos.y, visiting_pos.z)
        length += 1

        visiting_pos += iter_dir

    return Vector2i(length, width)


func _add_quad_test(data: ChunkMeshData, block: int, x: int, y: int, z: int, length: int, width: int, dir: Vector3i) -> void:
    var v0 = Vector3(0, 0, 1)
    v0.rotated(Vector3.UP, 90)
    pass


func _add_right_quad(data: ChunkMeshData, block: int, x: int, y: int, z: int, length: int, width: int) -> void:
    var v0 = Vector3(x + 1, y,          z        )
    var v1 = Vector3(x + 1, y + length, z        )
    var v2 = Vector3(x + 1, y + length, z + width)
    var v3 = Vector3(x + 1, y,          z + width)

    _add_quad(data, block, width, length, [v0, v1, v2, v3], [0, 2, 1, 0, 3, 2], Vector3.RIGHT)


func _add_left_quad(data: ChunkMeshData, block: int, x: int, y: int, z: int, length: int, width: int) -> void:
    var v0 = Vector3(x, y,          z        )
    var v1 = Vector3(x, y + length, z        )
    var v2 = Vector3(x, y + length, z + width)
    var v3 = Vector3(x, y,          z + width)

    _add_quad(data, block, width, length, [v0, v1, v2, v3], [0, 1, 2, 0, 2, 3], Vector3.LEFT)


func _add_up_quad(data: ChunkMeshData, block: int, x: int, y: int, z: int, length: int, width: int) -> void:
    var v0 = Vector3(x,         y + 1, z         )
    var v1 = Vector3(x + width, y + 1, z         )
    var v2 = Vector3(x + width, y + 1, z + length)
    var v3 = Vector3(x,         y + 1, z + length)

    _add_quad(data, block, width, length, [v0, v1, v2, v3], [0, 1, 2, 0, 2, 3], Vector3.UP)


func _add_down_quad(data: ChunkMeshData, block: int, x: int, y: int, z: int, length: int, width: int) -> void:
    var v0 = Vector3(x,         y, z         )
    var v1 = Vector3(x + width, y, z         )
    var v2 = Vector3(x + width, y, z + length)
    var v3 = Vector3(x,         y, z + length)

    _add_quad(data, block, width, length, [v0, v1, v2, v3], [0, 2, 1, 0, 3, 2], Vector3.DOWN)


func _add_forward_quad(data: ChunkMeshData, block: int, x: int, y: int, z: int, length: int, width: int) -> void:
    var v0 = Vector3(x,         y,          z)
    var v1 = Vector3(x + width, y,          z)
    var v2 = Vector3(x + width, y + length, z)
    var v3 = Vector3(x,         y + length, z)

    _add_quad(data, block, width, length, [v0, v1, v2, v3], [0, 1, 2, 0, 2, 3], Vector3.FORWARD)


func _add_back_quad(data: ChunkMeshData, block: int, x: int, y: int, z: int, length: int, width: int) -> void:
    var v0 = Vector3(x,         y,          z + 1)
    var v1 = Vector3(x + width, y,          z + 1)
    var v2 = Vector3(x + width, y + length, z + 1)
    var v3 = Vector3(x,         y + length, z + 1)

    _add_quad(data, block, width, length, [v0, v1, v2, v3], [0, 2, 1, 0, 3, 2], Vector3.BACK)


func _is_face_visited(data: ChunkMeshData, x: int, y: int, z: int) -> bool:
    const chunk_size = MellohiSettings.CHUNK_SIZE
    return !!data.faces_visited[y + z * chunk_size + x * chunk_size * chunk_size]


func _set_face_visited(data: ChunkMeshData, x: int, y: int, z: int) -> void:
    const chunk_size = MellohiSettings.CHUNK_SIZE
    data.faces_visited[y + z * chunk_size + x * chunk_size * chunk_size] = 1


func _is_face_hidden(data: ChunkMeshData, x: int, y: int, z: int, dir: Vector3i) -> bool:
    x += dir.x
    y += dir.y
    z += dir.z

    const chunk_size = MellohiSettings.CHUNK_SIZE

    if y >= chunk_size or y < 0:
        return false

    if x >= chunk_size or x < 0:
        return false

    if z >= chunk_size or z < 0:
        return false

    var block = data.chunk.get_block(x, y, z)

    return !!block


func _add_quad(
    data: ChunkMeshData,
    block: int,
    length: int, width: int,
    vertices: PackedVector3Array,
    indices: PackedInt32Array,
    normal: Vector3
) -> void:
    var index_offset = data.vertices[block].size()

    data.vertices[block].append_array(vertices)

    for i in range(4):
        data.normals[block].append(normal)
    
    for index in indices:
        data.indices[block].append(index + index_offset)


func _build_surfaces(data: ChunkMeshData) -> ArrayMesh:
    var mesh = ArrayMesh.new()

    for surface in range(data.vertices.size()):
        if data.vertices[surface].size() == 0:
            continue
        
        var surface_arrays = []
        surface_arrays.resize(Mesh.ARRAY_MAX)

        surface_arrays[Mesh.ARRAY_VERTEX] = data.vertices[surface]
        surface_arrays[Mesh.ARRAY_NORMAL] = data.normals[surface]
        surface_arrays[Mesh.ARRAY_INDEX] = data.indices[surface]

        var surface_index = mesh.get_surface_count()
        mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, surface_arrays)

        var material = StandardMaterial3D.new()
        material.albedo_texture = BlockRegistry.resources[surface].material.texture
        material.uv1_triplanar = true
        material.texture_filter = BaseMaterial3D.TEXTURE_FILTER_NEAREST_WITH_MIPMAPS
        mesh.surface_set_material(surface_index, material)

    return mesh
