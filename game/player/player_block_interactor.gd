extends RayCast3D


@export var block_outline: MeshInstance3D


func _physics_process(_delta):
    if is_colliding():
        var normal = get_collision_normal()
        var collision_pos = get_collision_point() - normal * 0.5
        print(collision_pos)
        var block_x = floor(collision_pos.x) + 0.5
        var block_y = floor(collision_pos.y) + 0.5
        var block_z = floor(collision_pos.z) + 0.5
        var block_pos = Vector3(block_x, block_y, block_z)
        
        block_outline.global_position = block_pos
        block_outline.visible = true
        block_outline.global_rotation = Vector3()
        
        # Break block at block_pos
        # Place block at block_pos + normal
    else:
        block_outline.visible = false
