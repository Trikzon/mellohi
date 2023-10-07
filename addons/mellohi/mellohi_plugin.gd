@tool
extends EditorPlugin

var MELLOHI_AUTOLOAD: String = "Mellohi"
var BLOCK_REGISTRY_AUTOLOAD: String = "BlockRegistry"
var CHUNK_MESH_GENERATOR_AUTOLOAD: String = "ChunkMeshGenerator"

var MELLOHI_INSPECTOR_PLUGIN: EditorInspectorPlugin = preload("res://addons/mellohi/editor/inspector/mellohi_inspector_plugin.gd").new()


func _enter_tree() -> void:
	add_autoload_singleton(MELLOHI_AUTOLOAD, "res://addons/mellohi/mellohi.gd")
	add_autoload_singleton(BLOCK_REGISTRY_AUTOLOAD, "res://addons/mellohi/blocks/block_registry.gd")
	add_autoload_singleton(CHUNK_MESH_GENERATOR_AUTOLOAD, "res://addons/mellohi/terrain/chunk/chunk_mesh_generator.gd")
	add_inspector_plugin(MELLOHI_INSPECTOR_PLUGIN)


func _exit_tree() -> void:
	remove_autoload_singleton(MELLOHI_AUTOLOAD)
	remove_autoload_singleton(BLOCK_REGISTRY_AUTOLOAD)
	remove_autoload_singleton(CHUNK_MESH_GENERATOR_AUTOLOAD)
	remove_inspector_plugin(MELLOHI_INSPECTOR_PLUGIN)
