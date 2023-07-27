using Godot;
using System;

namespace Game.Debug;

public partial class DebugOverlay : Control
{
    private Label _label;
    private Camera3D _camera;

    public override void _Ready()
    {
        _label = GetNode<Label>("Label");
        _camera = GetTree().Root.GetNode<Camera3D>("Main/SimpleFreeLookCamera");
    }

    public override void _Process(double delta)
    {
        var pos = _camera.Position;
        _label.Text = $"{pos.X:F2}, {pos.Y:F2}, {pos.Z:F2}";
    }
}
