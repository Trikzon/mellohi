extends RayCast3D


@export var block_outline: MeshInstance3D


func _physics_process(_delta):
    if is_colliding():
        var normal = get_collision_normal()
        var collision_pos = get_collision_point() - normal * 0.5
        var block_x = floor(collision_pos.x) + 0.5
        var block_y = floor(collision_pos.y) + 0.5
        var block_z = floor(collision_pos.z) + 0.5
        var block_pos = Vector3(block_x, block_y, block_z)
        
        block_outline.global_position = block_pos
        block_outline.visible = true
        block_outline.global_rotation = Vector3()
        
        var air_block = Blocks.id_to_block("air")
        var stone_block = Blocks.id_to_block("stone")
        # Implement a better way to get the terrain.
        # Should this be done in-plugin, or handled by the game?
        var terrain: Terrain = get_node("/root/Main/Terrain")
        if not terrain or not air_block or not stone_block:
            return
        
        if Input.is_action_just_pressed("player_break"):
            terrain.set_blockv(air_block, block_pos)
        elif Input.is_action_just_pressed("player_place"):
            terrain.set_blockv(stone_block, block_pos + normal)
    else:
        block_outline.visible = false
