using System.Linq;
using Godot;
using voxelgame.World;

namespace voxelgame.Overlay;

public partial class DebugOverlay : Control
{
	private Dimension? _dimension;
	private Camera3D? _camera3D;
	private Label? _fpsLabel, _positionLabel, _voxelLabel, _chunkLabel, _verticesLabel;
	private Label? _drawModeLabel;
	private bool _drawWireframe;
	
	public override void _Ready()
	{
		_dimension = GetNode<Dimension>("/root/Dimension");
		_camera3D = GetNode<Camera3D>("/root/Dimension/Camera3D");
		
		_fpsLabel = GetNode<Label>("LeftVBox/FpsLabel");
		_positionLabel = GetNode<Label>("LeftVBox/PositionLabel");
		_voxelLabel = GetNode<Label>("LeftVBox/VoxelLabel");
		_chunkLabel = GetNode<Label>("LeftVBox/ChunkLabel");
		_verticesLabel = GetNode<Label>("LeftVBox/VerticesLabel");
		
		_drawModeLabel = GetNode<Label>("RightVBox/DrawModeLabel");
	}

	public override void _Process(double delta)
	{
		_fpsLabel!.Text = $"{Engine.GetFramesPerSecond()} fps";

		var pos = _camera3D!.GlobalPosition;
		_positionLabel!.Text = $"XYZ: {pos.X:F3} / {pos.Y:F3} / {pos.Z:F3}";

		var voxelPos = Dimension.GetVoxelPos(pos);
		_voxelLabel!.Text = $"Voxel: {voxelPos.X} / {voxelPos.Y} / {voxelPos.Z} {{{_dimension!.GetVoxel(voxelPos).Name}}}";
		
		var chunkPos = Dimension.GetChunkPos(voxelPos);
		_chunkLabel!.Text = $"Chunk: {chunkPos.X} / {chunkPos.Y} / {chunkPos.Z} ({_dimension.GetChunks().Count})";

		var vertices = _dimension.GetChunks().Values.Sum(chunk => chunk.GetMesh().Vertices.Count);
		_verticesLabel!.Text = $"Vertices: {vertices}";
		
		_drawModeLabel!.Text = $"Draw Mode [0]: {(_drawWireframe ? "Wireframe" : "Normal")}";
	}

	public override void _PhysicsProcess(double delta)
	{
		if (Input.IsActionJustPressed("debug_drawMode"))
		{
			_drawWireframe = !_drawWireframe;
			GetViewport().DebugDraw = _drawWireframe ? Viewport.DebugDrawEnum.Wireframe : Viewport.DebugDrawEnum.Disabled;
		}
	}
}
