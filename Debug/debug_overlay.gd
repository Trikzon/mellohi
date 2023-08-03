extends Control

@onready var _label: Label = $Label

func _process(_delta):
    _label.text = str(Engine.get_frames_per_second())
