@tool  # Autoloaded Singleton
extends Node


var _should_render = PackedByteArray()
var _materials: Array[StandardMaterial3D] = []


func _invalidate():
    _should_render = PackedByteArray()
    _materials = []


func _add_block(block: Block):
    _should_render.append(block.render)
    
    if not block.render:
        _materials.append(null)
        return
    
    var material = StandardMaterial3D.new()
    material.albedo_texture = block.texture
    material.texture_filter = BaseMaterial3D.TEXTURE_FILTER_NEAREST_WITH_MIPMAPS
    material.uv1_triplanar = true
    _materials.append(material)


func should_render(index: int) -> bool:
    return !!_should_render[index]


func should_block_render(block: Block) -> bool:
    return !!_should_render[block.index]


func get_material(index: int) -> Material:
    return _materials[index]


func get_block_material(block: Block) -> Material:
    return _materials[block.index]
