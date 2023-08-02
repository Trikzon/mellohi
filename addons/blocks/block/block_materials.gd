@tool
extends Node


var _shouldRender = PackedByteArray()
var _materials: Array[StandardMaterial3D] = []


func _invalidate():
    _shouldRender = PackedByteArray()
    _materials = []


func _add_block(block: Block):
    _shouldRender.append(block.render)
    
    if not block.render:
        _materials.append(null)
        return
    
    var material = StandardMaterial3D.new()
    material.albedo_texture = block.texture
    material.texture_filter = BaseMaterial3D.TEXTURE_FILTER_NEAREST_WITH_MIPMAPS
    material.uv1_triplanar = true
    _materials.append(material)


func should_render(block: Block) -> bool:
    return bool(_shouldRender[block.index])


func get_material(block: Block) -> StandardMaterial3D:
    return _materials[block.index]
