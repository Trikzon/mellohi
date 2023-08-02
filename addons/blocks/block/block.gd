@tool
class_name Block extends Resource

## The human-readable identifier for the block.
@export var id = ""

@export_group("Rendering")
## If false, the block is not rendered to the screen and other rendering settings have no effect.
@export var render = true
## If true, the block is rendered with transparency enabled. UNIMPLEMENTED
@export var transparent = false
## The texture that is rendered on all 6 sides of the cube.
@export var texture: Texture2D

## Calculated at runtime and only used to index into arrays to quicken memory access.
## It should never be a negative number after being registered.
var index: int = -1
