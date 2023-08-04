extends CharacterBody3D


@export var speed := 5.0
@export var jump_velocity := 4.5
@export var look_sensitivity := 0.1

@onready var head = $Head
@onready var camera = $Head/Camera3D

var gravity = ProjectSettings.get_setting("physics/3d/default_gravity")


func _ready():
    Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)


func _physics_process(delta):
    # Toggle capturing the mouse.
    if Input.is_action_just_pressed(&"ui_cancel"):
        if Input.get_mouse_mode() == Input.MOUSE_MODE_CAPTURED:
            Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
        else:
            Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
    
    # Return early if the mouse is not captured.
    if Input.get_mouse_mode() != Input.MOUSE_MODE_CAPTURED:
        return
    
    # Add the gravity.
    if not is_on_floor():
        velocity.y -= gravity * delta

    # Handle Jump.
    if Input.is_action_just_pressed(&"player_jump") and is_on_floor():
        velocity.y = jump_velocity

    # Get the input direction and handle the movement/deceleration.
    var input_dir = Input.get_vector(&"player_left", &"player_right", &"player_forward", &"player_back")
    var direction = (transform.basis * Vector3(input_dir.x, 0, input_dir.y)).normalized()
    if direction:
        velocity.x = direction.x * speed
        velocity.z = direction.z * speed
    else:
        velocity.x = move_toward(velocity.x, 0, speed / 10)
        velocity.z = move_toward(velocity.z, 0, speed / 10)

    move_and_slide()


func _input(event):
    # Return early if the mouse is not captured.
    if Input.get_mouse_mode() != Input.MOUSE_MODE_CAPTURED:
        return
    
    # Get mouse input for camera rotation.
    if event is InputEventMouseMotion:
        rotate_y(deg_to_rad(-event.relative.x * look_sensitivity))
        head.rotate_x(deg_to_rad(-event.relative.y * look_sensitivity))
        head.rotation.x = clamp(head.rotation.x, deg_to_rad(-89), deg_to_rad(89))
