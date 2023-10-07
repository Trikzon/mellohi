@tool
class_name Identifier
extends Resource

@export var mod_name: StringName : set = set_mod_name
@export var address: StringName : set = set_address


func _init(mod_name: StringName=&"", address: StringName=&"") -> void:
    if not mod_name:
        mod_name = _get_game_name()
        set_address(&"empty")
    elif not address:
        set_mod_name(_get_game_name())
        set_address(mod_name)
    else:
        set_mod_name(mod_name)
        set_address(address)


static func from_string(str: String) -> Identifier:
    var parts = str.split(":")
    if parts.size() == 1:
        return Identifier.new(_get_game_name(), parts[0])
    else:
        return Identifier.new(parts[0], parts[1])


func _to_string():
    return mod_name + ":" + address


func set_mod_name(value: StringName) -> void:
    mod_name = _sanitize_string(value)


func set_address(value: StringName) -> void:
    address = _sanitize_string(value)


func _sanitize_string(value: String) -> String:
    value = value.to_lower()
    value = value.replace(" ", "_")
    return value


static func _get_game_name() -> String:
    return ProjectSettings.get_setting("application/config/name")
