using Godot;
using System;

namespace voxelgame.World.Chunk;

public partial class Chunk : Node3D
{
	// 0 is air, 1 is dirt, 2 is grass
	// TODO: Use a Voxel class instead of an int
	private int[] _voxels = new int[16 * 16 * 16];

	public override void _Ready()
	{
		// Create two layers of dirt
		for (var y = 0; y < 15; y++)
			for (var x = 0; x < 16; x++)
				for (var z = 0; z < 16; z++)
					SetVoxel(x, y, z, 1);
		for (var x = 0; x < 16; x++)
			for (var z = 0; z < 16; z++)
				SetVoxel(x, 15, z, 2);
		
		var chunkMesh = GetNode<ChunkMesh>("Mesh");
		chunkMesh.GenerateMesh(this);
	}
	
	public int GetVoxel(int x, int y, int z)
	{
		return _voxels[x + z * 16 + y * 16 * 16];
	}
	
	public void SetVoxel(int x, int y, int z, int value)
	{
		_voxels[x + z * 16 + y * 16 * 16] = value;
	}
}
