@tool
extends Node


class MeshData:
    var chunk: Chunk

    var vertices: Array[PackedVector3Array] = []
    var normals: Array[PackedVector3Array] = []
    var tex_uvs: Array[PackedVector2Array] = []
    var indices: Array[PackedInt32Array] = []

    var faces_visited = PackedByteArray()


    func _init(chunk: Chunk) -> void:
        self.chunk = chunk

        # TODO: Replace with surface count.
        for i in range(BlockRegistry.resources.size()):
            vertices.append(PackedVector3Array())
            normals.append(PackedVector3Array())
            tex_uvs.append(PackedVector2Array())
            indices.append(PackedInt32Array())

        faces_visited = PackedByteArray()
        faces_visited.resize(Mellohi.CHUNK_VOL)

        for i in range(Mellohi.CHUNK_VOL):
            faces_visited.append(0)


    func clear_faces_visited() -> void:
        for i in range(Mellohi.CHUNK_VOL):
            faces_visited[i] = 0
    

    func is_face_visited(x: int, y: int, z: int) -> bool:
        return !!faces_visited[Chunk.local_to_index(x, y, z)]


    func is_face_visitedv(pos: Vector3i) -> bool:
        return is_face_visited(pos.x, pos.y, pos.z)
    

    func set_face_visited(x: int, y: int, z: int) -> void:
        faces_visited[Chunk.local_to_index(x, y, z)] = 1
    

    func set_face_visitedv(pos: Vector3i) -> void:
        set_face_visited(pos.x, pos.y, pos.z)


    func is_face_hidden(x: int, y: int, z: int, face_dir: Vector3i) -> bool:
        x += face_dir.x
        y += face_dir.y
        z += face_dir.z

        if y >= Mellohi.CHUNK_LEN or y < 0:
            return false

        if x >= Mellohi.CHUNK_LEN or x < 0:
            return false

        if z >= Mellohi.CHUNK_LEN or z < 0:
            return false

        var block = chunk.get_block(x, y, z)

        return !!block


    func is_face_hiddenv(pos: Vector3i, face_dir: Vector3i) -> bool:
        return is_face_hidden(pos.x, pos.y, pos.z, face_dir)


func generate_mesh(chunk: Chunk) -> Mesh:
    var data = MeshData.new(chunk)

    _create_greedy_face(data, Vector3i.FORWARD)
    _create_greedy_face(data, Vector3i.RIGHT)
    _create_greedy_face(data, Vector3i.BACK)
    _create_greedy_face(data, Vector3i.LEFT)
    _create_greedy_face(data, Vector3i.UP)
    _create_greedy_face(data, Vector3i.DOWN)

    return _build_surfaces(data)


func _create_greedy_face(data: MeshData, face_dir: Vector3i) -> void:
    data.clear_faces_visited()

    for y in range(Mellohi.CHUNK_LEN):
        for z in range(Mellohi.CHUNK_LEN):
            for x in range(Mellohi.CHUNK_LEN):
                var block = data.chunk.get_block(x, y, z)

                if block == 0:
                    continue

                var size = _find_greedy_quad_size(data, block, x, y, z, face_dir)

                if size.x <= 0 or size.y <= 0:
                    continue
                
                _add_quad(data, block, x, y, z, face_dir, size)


func _find_greedy_quad_size(data: MeshData, block: int, x: int, y: int, z: int, face_dir: Vector3i) -> Vector2i:
    var width = 1
    var length = 0

    # Return early if this face has already been visited or if the face is hidden.
    if data.is_face_visited(x, y, z) or data.is_face_hidden(x, y, z, face_dir):
        return Vector2i(width, length)

    # ===== 1D Greedy Mesh Pass =====
    # This pass iterates over blocks in one direction and checks if the blocks are the same.
    var visiting_pos = Vector3i(x, y, z)

    # If the direction is up or down, we want to iterate over the z-axis. Otherwise, the y-axis.
    var iter_dir: Vector3i
    if face_dir.abs().y == 1:
        iter_dir = Vector3i(0, 0, 1)
    else:
        iter_dir = Vector3i(0, 1, 0)
    var iter_axis = iter_dir.max_axis_index()
    var max_pos = iter_dir * Mellohi.CHUNK_LEN

    while visiting_pos[iter_axis] < max_pos[iter_axis]:
        # If the block at the visiting position is not the same as the block who's face we're building, stop.
        # TODO: Also check if the block has the same rotation, light level, etc.
        if data.chunk.get_blockv(visiting_pos) != block:
            break
        
        if data.is_face_visitedv(visiting_pos):
            break
        
        if data.is_face_hiddenv(visiting_pos, face_dir):
            break
        
        data.set_face_visitedv(visiting_pos)
        length += 1

        visiting_pos += iter_dir
    
    # ===== 2D Greedy Mesh Pass =====
    # This pass iterates in the other direction to see if the 1D mesh can be extended that way.
    var first_iter_dir: Vector3i
    if face_dir.abs().x == 1:
        first_iter_dir = Vector3i(0, 0, 1)
    else:
        first_iter_dir = Vector3i(1, 0, 0)
    var first_iter_axis = first_iter_dir.max_axis_index()
    var first_max_pos = first_iter_dir * Mellohi.CHUNK_LEN

    # If the direction is up or down, we want to iterate over the z-axis. Otherwise, the y-axis.
    var second_iter_dir: Vector3i
    if face_dir.abs().y == 1:
        second_iter_dir = Vector3i(0, 0, 1)
    else:
        second_iter_dir = Vector3i(0, 1, 0)
    var second_iter_axis = second_iter_dir.max_axis_index()

    # Start at the first block in the new direction.
    visiting_pos = Vector3i(x, y, z) + first_iter_dir

    # Save the initial value of the second iteration axis so we can reset it later.
    var initial_second_iter_value = visiting_pos[second_iter_axis]

    var stop = false
    while visiting_pos[first_iter_axis] < first_max_pos[first_iter_axis] and not stop:
        # Check if the block is the same as the block who's quad we're building
        visiting_pos[second_iter_axis] = initial_second_iter_value
        while visiting_pos[second_iter_axis] < initial_second_iter_value + length and not stop:
            stop = stop or data.chunk.get_blockv(visiting_pos) != block
            stop = stop or data.is_face_visitedv(visiting_pos)
            stop = stop or data.is_face_hiddenv(visiting_pos, face_dir)

            visiting_pos += second_iter_dir
        
        if stop:
            continue
        
        # If not stop, that means the quad can be extended in this direction by one.
        width += 1

        # Mark all the blocks in this direction as visited.
        visiting_pos[second_iter_axis] = initial_second_iter_value
        while visiting_pos[second_iter_axis] < initial_second_iter_value + length:
            data.set_face_visitedv(visiting_pos)

            visiting_pos += second_iter_dir
        
        visiting_pos += first_iter_dir

    return Vector2i(width, length)


func _add_quad(data: MeshData, block: int, x: int, y: int, z: int, face_dir: Vector3i, size: Vector2i):
    var index_offset = data.vertices[block].size()

    var local_pos = Vector3(x, y, z)
    face_dir.max_axis_index()

    var face_dir_rot: Quaternion
    match face_dir:
        Vector3i.UP, Vector3i.DOWN:
            face_dir_rot = Quaternion(Vector3.RIGHT, -Vector3.FORWARD.signed_angle_to(face_dir, Vector3.RIGHT))
        _:
            face_dir_rot = Quaternion(Vector3.UP, -Vector3.FORWARD.signed_angle_to(face_dir, Vector3.UP))

    var vert_scale: Vector3
    match face_dir:
        Vector3i.UP, Vector3i.DOWN:
            vert_scale = Vector3(size.x, 1, size.y)
        Vector3i.LEFT, Vector3i.RIGHT:
            vert_scale = Vector3(1, size.y, size.x)
        Vector3i.FORWARD, Vector3i.BACK:
            vert_scale = Vector3(size.x, size.y, 1)

    var scale_offset = (vert_scale / 2) - Vector3(0.5, 0.5, 0.5)
    var uv_scale = Vector2(size)

    const cube_verts: Array[Vector3] = [
        Vector3(0, 0, 0),
        Vector3(1, 1, 0),
        Vector3(0, 1, 0),
        Vector3(1, 0, 0)
    ]
    data.vertices[block].append_array(cube_verts.map(
        func(v: Vector3) -> Vector3:
            v -= Vector3(0.5, 0.5, 0.5)  # Offset vertice so that the origin is the center of rotation.
            v *= face_dir_rot            # Rotate the vertice.
            v *= vert_scale              # Scale the vertice.
            v += local_pos               # Move the vertice to the block's local position.
            v += scale_offset            # Fix the offset issue of the scale.
            v += Vector3(0.5, 0.5, 0.5)  # Unoffset the vertice to move the origin back to 0, 0.
            return v
    ))

    for i in range(4):
        data.normals[block].append(face_dir)
    
    const cube_uvs: Array[Vector2] = [
        Vector2(1, 1),
        Vector2(0, 0),
        Vector2(1, 0),
        Vector2(0, 1)
    ]
    data.tex_uvs[block].append_array(cube_uvs.map(
        func(v: Vector2) -> Vector2: 
            return v * uv_scale
    ))

    data.indices[block].append_array([0, 1, 2, 0, 3, 1].map(
        func(i: int) -> int:
            return i + index_offset
    ))


func _build_surfaces(data: MeshData) -> ArrayMesh:
    var mesh = ArrayMesh.new()

    for surface in range(data.vertices.size()):
        if data.vertices[surface].size() == 0:
            continue
        
        var surface_arrays = []
        surface_arrays.resize(Mesh.ARRAY_MAX)

        surface_arrays[Mesh.ARRAY_VERTEX] = data.vertices[surface]
        surface_arrays[Mesh.ARRAY_NORMAL] = data.normals[surface]
        surface_arrays[Mesh.ARRAY_TEX_UV] = data.tex_uvs[surface]
        surface_arrays[Mesh.ARRAY_INDEX] = data.indices[surface]

        var surface_index = mesh.get_surface_count()
        mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, surface_arrays)

        var material = StandardMaterial3D.new()
        material.albedo_texture = BlockRegistry.resources[surface].material.texture
        material.texture_filter = BaseMaterial3D.TEXTURE_FILTER_NEAREST_WITH_MIPMAPS
        mesh.surface_set_material(surface_index, material)

    return mesh
