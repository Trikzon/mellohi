@tool
extends EditorPlugin


func _enter_tree():
    var base_control = get_editor_interface().get_base_control()
    var camera_icon = base_control.get_theme_icon("Camera3D", "EditorIcons")
    
    add_custom_type("SimpleFreeLookCamera", "Camera3D", preload("camera.gd"), camera_icon)


func _exit_tree():
    remove_custom_type("SimpleFreeLookCamera")
