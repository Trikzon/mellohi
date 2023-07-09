using Godot;
using System;
using voxelgame.World;

namespace voxelgame.Overlay;

public partial class DebugOverlay : Control
{
	private Dimension? _dimension;
	private Camera3D? _camera3D;
	private Label? _fpsLabel, _positionLabel, _voxelLabel, _chunkLabel;
	
	public override void _Ready()
	{
		_dimension = GetNode<Dimension>("/root/Dimension");
		_camera3D = GetNode<Camera3D>("/root/Dimension/Camera3D");
		_fpsLabel = GetNode<Label>("VBoxContainer/FpsLabel");
		_positionLabel = GetNode<Label>("VBoxContainer/PositionLabel");
		_voxelLabel = GetNode<Label>("VBoxContainer/VoxelLabel");
		_chunkLabel = GetNode<Label>("VBoxContainer/ChunkLabel");
	}

	public override void _Process(double delta)
	{
		_fpsLabel!.Text = $"{Engine.GetFramesPerSecond()} fps";

		var pos = _camera3D!.GlobalPosition;
		_positionLabel!.Text = $"XYZ: {pos.X:F3} / {pos.Y:F3} / {pos.Z:F3}";

		var voxelPos = Dimension.GetVoxelPos(pos);
		_voxelLabel!.Text = $"Voxel: {voxelPos.X} / {voxelPos.Y} / {voxelPos.Z} {{{_dimension!.GetVoxel(voxelPos)}}}";
		
		var chunkPos = Dimension.GetChunkPos(voxelPos);
		_chunkLabel!.Text = $"Chunk: {chunkPos.X} / {chunkPos.Y} / {chunkPos.Z}";
	}
}
