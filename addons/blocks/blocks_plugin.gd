@tool
extends EditorPlugin


const AUTOLOAD_PLUGIN_OPTIONS = "BlocksPluginOptions"
const AUTOLOAD_BLOCKS = "Blocks"
const AUTOLOAD_BLOCK_MATERIALS = "BlockMaterials"


func _enter_tree():
    add_autoload_singleton(AUTOLOAD_PLUGIN_OPTIONS, "res://addons/blocks/plugin_options.gd")
    add_autoload_singleton(AUTOLOAD_BLOCKS, "res://addons/blocks/block/blocks.gd")
    add_autoload_singleton(AUTOLOAD_BLOCK_MATERIALS, "res://addons/blocks/block/block_materials.gd")


func _exit_tree():
    remove_autoload_singleton(AUTOLOAD_PLUGIN_OPTIONS)
    remove_autoload_singleton(AUTOLOAD_BLOCKS)
    remove_autoload_singleton(AUTOLOAD_BLOCK_MATERIALS)
