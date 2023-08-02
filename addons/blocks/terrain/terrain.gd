@tool
class_name Terrain extends Node3D


func _ready():
    Blocks.invalidate()
    Blocks.add_block(preload("res://data/blocks/air.tres"))
    Blocks.add_block(preload("res://data/blocks/stone.tres"))
    Blocks.add_block(preload("res://data/blocks/dirt.tres"))
    Blocks.add_block(preload("res://data/blocks/grass.tres"))
    
    var stone = Blocks.id_to_block("stone")
    var dirt = Blocks.id_to_block("dirt")
    var grass = Blocks.id_to_block("grass")
    for y in range(16):
        for z in range(16):
            for x in range(16):
                if y <= 15 - 3:
                    temp_gen_block_mesh(stone, Vector3(x, y, z))
                elif y < 15:
                    temp_gen_block_mesh(dirt, Vector3(x, y, z))
                else:
                    temp_gen_block_mesh(grass, Vector3(x, y, z))


func temp_gen_block_mesh(block: Block, pos: Vector3):
    var meshInstance = MeshInstance3D.new()
    meshInstance.position = pos
    var mesh = BoxMesh.new()
    mesh.material = BlockMaterials.get_material(block)
    meshInstance.mesh = mesh
    add_child(meshInstance)
