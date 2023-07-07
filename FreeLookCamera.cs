// Simple Free-Look Camera script translated to C#.
// Original script is licensed under the MIT license by Adam Viola
// and is available at https://github.com/adamviola/simple-free-look-camera
using System;
using Godot;

namespace voxelgame;

public partial class FreeLookCamera : Camera3D
{
    private const float ShiftMultiplier = 2.5f;
    private const float AltMultiplier = 1.0f / ShiftMultiplier;

    [Export(PropertyHint.Range, "0.0, 1.0")]
    public float Sensitivity = 0.25f;

    private Vector2 _mousePosition = Vector2.Zero;
    private float _totalPitch;

    private Vector3 _direction = Vector3.Zero;
    private Vector3 _velocity = Vector3.Zero;
    private float _acceleration = 30;
    private float _deceleration = -10;
    private float _velMultiplier = 4;

    private bool _w, _s, _a, _d, _q, _e, _shift, _alt;

    public override void _Input(InputEvent @event)
    {
        switch (@event)
        {
            case InputEventMouseMotion mouseMotion:
                _mousePosition = mouseMotion.Relative;
                break;
            case InputEventMouseButton mouseButton:
                // ReSharper disable once SwitchStatementMissingSomeEnumCasesNoDefault
                switch (mouseButton.ButtonIndex)
                {
                    case MouseButton.Right:
                        Input.MouseMode = mouseButton.Pressed ? Input.MouseModeEnum.Captured : Input.MouseModeEnum.Visible;
                        break;
                    case MouseButton.WheelUp:
                        _velMultiplier = Mathf.Clamp(_velMultiplier * 1.1f, 0.2f, 20);
                        break;
                    case MouseButton.WheelDown:
                        _velMultiplier = Mathf.Clamp(_velMultiplier / 1.1f, 0.2f, 20);
                        break;
                }
                break;
            case InputEventKey key:
                // ReSharper disable once SwitchStatementMissingSomeEnumCasesNoDefault
                switch (key.Keycode)
                {
                    case Key.W: _w = key.Pressed; break;
                    case Key.S: _s = key.Pressed; break;
                    case Key.A: _a = key.Pressed; break;
                    case Key.D: _d = key.Pressed; break;
                    case Key.Q: _q = key.Pressed; break;
                    case Key.Shift: _shift = key.Pressed; break;
                    case Key.Alt: _alt = key.Pressed; break;
                }
                break;
        }
    }

    public override void _Process(double delta)
    {
        UpdateMouseLook();
        UpdateMovement(delta);
    }

    private void UpdateMouseLook()
    {
        if (Input.MouseMode != Input.MouseModeEnum.Captured) return;
        
        _mousePosition *= Sensitivity;
        var yaw = _mousePosition.X;
        var pitch = _mousePosition.Y;
        _mousePosition = Vector2.Zero;

        pitch = Mathf.Clamp(pitch, -90 - _totalPitch, 90 - _totalPitch);
        _totalPitch += pitch;
            
        RotateY(Mathf.DegToRad(-yaw));
        RotateObjectLocal(new Vector3(1, 0, 0), Mathf.DegToRad(-pitch));
    }

    private void UpdateMovement(double delta)
    {
        _direction = new Vector3(
            Convert.ToSingle(_d) - Convert.ToSingle(_a),
            Convert.ToSingle(_e) - Convert.ToSingle(_q),
            Convert.ToSingle(_s) - Convert.ToSingle(_w)
        );

        var offset = _direction.Normalized() * _acceleration * _velMultiplier * (float)delta;
        offset += _velocity.Normalized() * _deceleration * _velMultiplier * (float)delta;

        var speedMultiplier = 1.0f;
        if (_shift) speedMultiplier *= ShiftMultiplier;
        if (_alt) speedMultiplier *= AltMultiplier;

        if (_direction == Vector3.Zero && offset.LengthSquared() > _velocity.LengthSquared())
            _velocity = Vector3.Zero;
        else
        {
            _velocity.X = Mathf.Clamp(_velocity.X + offset.X, -_velMultiplier, _velMultiplier);
            _velocity.Y = Mathf.Clamp(_velocity.Y + offset.Y, -_velMultiplier, _velMultiplier);
            _velocity.Z = Mathf.Clamp(_velocity.Z + offset.Z, -_velMultiplier, _velMultiplier);
            
            Translate(_velocity * (float)delta * speedMultiplier);
        }
    }
}