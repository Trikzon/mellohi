using Godot;
using voxelgame.World.Voxels;

namespace voxelgame.World;

public partial class Chunk : Node3D
{
	public const int Size = 16;
	
	private static readonly PackedScene Scene = GD.Load<PackedScene>("res://World/Chunk.tscn");

	private readonly Voxel[] _voxels = new Voxel[Size * Size * Size];
	private Vector3I _chunkPos;
	private Dimension? _dimension;

	public static Chunk Create(Dimension dimension, Vector3I chunkPos)
	{
		var self = Scene.Instantiate() as Chunk;
		self!._dimension = dimension;
		self._chunkPos = chunkPos;
		self.Position = chunkPos * Size;
		return self;
	}
	
	public void GenerateChunkDate()
	{
		// Create two layers of dirt
		for (var y = 0; y < 15; y++)
			for (var x = 0; x < 16; x++)
				for (var z = 0; z < 16; z++)
					SetVoxel(new Vector3I(x, y, z), Voxel.Dirt);
		for (var x = 0; x < 16; x++)
			for (var z = 0; z < 16; z++)
				SetVoxel(new Vector3I(x, 15, z), Voxel.Grass);
	}

	public void GenerateMesh()
	{
		var chunkMesh = GetNode<ChunkMesh>("Mesh");
		chunkMesh.GenerateMesh(this);
	}
	
	public Voxel GetVoxel(Vector3I localPos)
	{
		if (localPos.X is < 0 or >= Size || localPos.Y is < 0 or >= Size || localPos.Z is < 0 or >= Size)
			return _dimension!.GetVoxel(localPos + _chunkPos * Size);
		return _voxels[localPos.X + localPos.Z * Size + localPos.Y * Size * Size];
	}
	
	public void SetVoxel(Vector3I localPos, Voxel voxel)
	{
		if (localPos.X is < 0 or >= Size || localPos.Y is < 0 or >= Size || localPos.Z is < 0 or >= Size)
			_dimension?.SetVoxel(localPos + _chunkPos * Size, voxel);
		_voxels[localPos.X + localPos.Z * Size + localPos.Y * Size * Size] = voxel;
	}
}
