using Godot;
using voxelgame.World.Voxels;

namespace voxelgame.World;

public partial class Chunk : Node3D
{
	public const int Size = 16;
	
	private static readonly PackedScene Scene = GD.Load<PackedScene>("res://World/Chunk.tscn");
	
	public Vector3I ChunkPos { get; private set; }

	private readonly Voxel[] _voxels = new Voxel[Size * Size * Size];
	private Dimension? _dimension;

	public static Chunk Create(Dimension dimension, Vector3I chunkPos)
	{
		var self = Scene.Instantiate() as Chunk;
		self!._dimension = dimension;
		self.ChunkPos = chunkPos;
		self.Position = chunkPos * Size;
		for (var i = 0; i < self._voxels.Length; i++) self._voxels[i] = Voxel.Air;
		return self;
	}

	public void GenerateFirstPhase()
	{
		var noise = new FastNoiseLite();

		for (var y = 0; y < Size; y++)
		{
			for (var x = 0; x < Size; x++)
			{
				for (var z = 0; z < Size; z++)
				{
					var localPos = new Vector3I(x, y, z);
					var voxelPos = GetVoxelPos(localPos);
					if (noise.GetNoise3Dv(voxelPos) > -0.25)
						SetVoxel(localPos, Voxel.Stone);
				}
			}
		}
	}
	public void GenerateSecondPhase() {
		for (var y = Size - 1; y >= 0; y--)
		{
			for (var x = 0; x < Size; x++)
			{
				for (var z = 0; z < Size; z++)
				{
					var localPos = new Vector3I(x, y, z);
					if (GetVoxel(localPos) == Voxel.Air) continue;
					
					if (GetVoxel(localPos + Vector3I.Up) == Voxel.Air)
					{
						SetVoxel(localPos, Voxel.Grass);
					}
					
					if (GetVoxel(localPos + Vector3I.Up) == Voxel.Grass || GetVoxel(localPos + Vector3I.Up * 2) == Voxel.Grass)
					{
						SetVoxel(localPos, Voxel.Dirt);
					}
				}
			}
		}
	}

	public void GenerateMesh()
	{
		var chunkMesh = GetNode<ChunkMesh>("Mesh");
		chunkMesh.GenerateMesh(_dimension!, this);
		// chunkMesh.OldGenerateMesh(this);
	}
	
	public Vector3I GetVoxelPos(Vector3I localPos)
	{
		return localPos + ChunkPos * Size;
	}
	
	public Voxel GetVoxel(Vector3I localPos)
	{
		if (localPos.X is < 0 or >= Size || localPos.Y is < 0 or >= Size || localPos.Z is < 0 or >= Size)
			return _dimension!.GetVoxel(GetVoxelPos(localPos));
		return _voxels[localPos.X + localPos.Z * Size + localPos.Y * Size * Size];
	}
	
	public void SetVoxel(Vector3I localPos, Voxel voxel)
	{
		if (localPos.X is < 0 or >= Size || localPos.Y is < 0 or >= Size || localPos.Z is < 0 or >= Size)
			_dimension?.SetVoxel(GetVoxelPos(localPos), voxel);
		_voxels[localPos.X + localPos.Z * Size + localPos.Y * Size * Size] = voxel;
	}
	
	public ChunkMesh GetMesh()
	{
		return GetNode<ChunkMesh>("Mesh");
	}
}
