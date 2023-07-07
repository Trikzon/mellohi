# Simple Free-Look Camera copied from https://github.com/adamviola/simple-free-look-camera
# Is licensed under the MIT license by Adam Viola
class_name FreeLookCamera extends Camera3D

const SHIFT_MULTIPLIER = 2.5
const ALT_MULTIPLIER = 1.0 / SHIFT_MULTIPLIER

@export_range(0.0, 1.0) var sensitivity: float = 0.25

var _mouse_position = Vector2.ZERO
var _total_pitch = 0.0

var _direction = Vector3.ZERO
var _velocity = Vector3.ZERO
var _acceleration = 30
var _deceleration = -10
var _vel_multiplier = 4

var _w = false
var _s = false
var _a = false
var _d = false
var _q = false
var _e = false
var _shift = false
var _alt = false

func _input(event):
	if event is InputEventMouseMotion:
		_mouse_position = event.relative
	
	if event is InputEventMouseButton:
		match event.button_index:
			MOUSE_BUTTON_RIGHT:
				Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED if event.pressed else Input.MOUSE_MODE_VISIBLE)
			MOUSE_BUTTON_WHEEL_UP:
				_vel_multiplier = clamp(_vel_multiplier * 1.1, 0.2, 20)
			MOUSE_BUTTON_WHEEL_DOWN:
				_vel_multiplier = clamp(_vel_multiplier / 1.1, 0.2, 20)
	
	if event is InputEventKey:
		match event.keycode:
			KEY_W:
				_w = event.pressed
			KEY_S:
				_s = event.pressed
			KEY_A:
				_a = event.pressed
			KEY_D:
				_d = event.pressed
			KEY_Q:
				_q = event.pressed
			KEY_E:
				_e = event.pressed
			KEY_SHIFT:
				_shift = event.pressed
			KEY_ALT:
				_alt = event.pressed

func _process(delta):
	_update_mouselook()
	_update_movement(delta)

func _update_mouselook():
	if Input.get_mouse_mode() == Input.MOUSE_MODE_CAPTURED:
		_mouse_position *= sensitivity
		var yaw = _mouse_position.x
		var pitch = _mouse_position.y
		_mouse_position = Vector2(0, 0)
		
		pitch = clamp(pitch, -90 - _total_pitch, 90 - _total_pitch)
		_total_pitch += pitch
		
		rotate_y(deg_to_rad(-yaw))
		rotate_object_local(Vector3(1, 0, 0), deg_to_rad(-pitch))

func _update_movement(delta):
	_direction = Vector3(
		(_d as float) - (_a as float),
		(_e as float) - (_q as float),
		(_s as float) - (_w as float)
	)
	
	var offset = _direction.normalized() * _acceleration * _vel_multiplier * delta \
			+ _velocity.normalized() * _deceleration * _vel_multiplier * delta
	
	var speed_multi = 1
	if _shift: speed_multi *= SHIFT_MULTIPLIER
	if _alt: speed_multi *= ALT_MULTIPLIER
	
	if _direction == Vector3.ZERO and offset.length_squared() > _velocity.length_squared():
		_velocity = Vector3.ZERO
	else:
		_velocity.x = clamp(_velocity.x + offset.x, -_vel_multiplier, _vel_multiplier)
		_velocity.y = clamp(_velocity.y + offset.y, -_vel_multiplier, _vel_multiplier)
		_velocity.z = clamp(_velocity.z + offset.z, -_vel_multiplier, _vel_multiplier)
		
		translate(_velocity * delta * speed_multi)
