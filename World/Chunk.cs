using Godot;
using System;

namespace voxelgame.World;

public partial class Chunk : Node3D
{
	public const int Size = 16;
	
	private static readonly PackedScene Scene = GD.Load<PackedScene>("res://World/Chunk.tscn");

	// 0 is air, 1 is dirt, 2 is grass
	// TODO: Use a Voxel class instead of an int
	private int[] _voxels = new int[Size * Size * Size];
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
					SetVoxel(new Vector3I(x, y, z), 1);
		for (var x = 0; x < 16; x++)
			for (var z = 0; z < 16; z++)
				SetVoxel(new Vector3I(x, 15, z), 2);
	}

	public void GenerateMesh()
	{
		var chunkMesh = GetNode<ChunkMesh>("Mesh");
		chunkMesh.GenerateMesh(this);
	}
	
	public int GetVoxel(Vector3I localPos)
	{
		if (localPos.X is < 0 or >= Size || localPos.Y is < 0 or >= Size || localPos.Z is < 0 or >= Size)
			return _dimension?.GetVoxel(localPos + _chunkPos * Size) ?? 0;
		return _voxels[localPos.X + localPos.Z * Size + localPos.Y * Size * Size];
	}
	
	public void SetVoxel(Vector3I localPos, int value)
	{
		// TODO: Set voxel in neighboring chunk if necessary
		// if (localPos.X is < 0 or >= Size || localPos.Y is < 0 or >= Size || localPos.Z is < 0 or >= Size)
		// 	return _world.SetVoxel(localPos + _chunkPos * Size);
		_voxels[localPos.X + localPos.Z * Size + localPos.Y * Size * Size] = value;
	}
}
