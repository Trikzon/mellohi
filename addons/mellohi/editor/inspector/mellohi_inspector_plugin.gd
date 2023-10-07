extends EditorInspectorPlugin

const IDENTIFIER_EDITOR_PROPERTY: GDScript = preload("res://addons/mellohi/editor/inspector/identifier_editor_property.gd")


func _can_handle(object: Object) -> bool:
    return true


func _parse_property(
    object: Object,
    type: Variant.Type,
    name: String, 
    hint_type: PropertyHint, 
    hint_string: String, 
    usage_flags: int, 
    wide: bool
) -> bool:
    if type == TYPE_OBJECT && hint_type == PROPERTY_HINT_RESOURCE_TYPE and hint_string == "Identifier":
        add_property_editor(name, IDENTIFIER_EDITOR_PROPERTY.new())
        return true
    return false
