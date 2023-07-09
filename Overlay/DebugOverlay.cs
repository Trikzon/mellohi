using Godot;
using System;

namespace voxelgame.Overlay;

public partial class DebugOverlay : Control
{
	// TODO: Change World namespace to something else.
	private Camera3D? _camera3D;
	private Label? _fpsLabel, _positionLabel;
	
	public override void _Ready()
	{
		_camera3D = GetNode<Camera3D>("/root/World/Camera3D");
		_fpsLabel = GetNode<Label>("VBoxContainer/FpsLabel");
		_positionLabel = GetNode<Label>("VBoxContainer/PositionLabel");
	}

	public override void _Process(double delta)
	{
		_fpsLabel!.Text = $"FPS: {Engine.GetFramesPerSecond()}";
		_positionLabel!.Text = $"Position: {_camera3D!.GlobalPosition}";
	}
}
