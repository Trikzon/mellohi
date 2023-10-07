@tool
class_name ChunkInstance
extends StaticBody3D

@export var chunk: Chunk:
    set(value):
        chunk = value
        update_mesh()


var mesh_instance: MeshInstance3D


func _ready() -> void:
    mesh_instance = get_node_or_null("MeshInstance3D")
    if not mesh_instance:
        mesh_instance = MeshInstance3D.new()
        mesh_instance.name = "MeshInstance3D"
        add_child(mesh_instance)
        mesh_instance.owner = get_tree().edited_scene_root
    
    update_mesh()


func update_mesh() -> void:
    if not mesh_instance or not chunk:
        return
    
    mesh_instance.mesh = ChunkMeshGenerator.generate_mesh(chunk)
