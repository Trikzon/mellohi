
using Godot;
using System.Collections.Generic;
using voxelgame.World.Voxels;

namespace voxelgame.World;

public partial class Dimension : Node3D
{
    private const int ChunkRadius = 4;
    
    private readonly Dictionary<Vector3I, Chunk> _chunks = new();

    public override void _Ready()
    {
        for (var x = -ChunkRadius; x < ChunkRadius; x++)
        {
            for (var y = -ChunkRadius; y < ChunkRadius; y++)
            {
                for (var z = -ChunkRadius; z < ChunkRadius; z++)
                {
                    var chunkPos = new Vector3I(x, y, z);
                    var chunk = Chunk.Create(this, chunkPos);
                    chunk.GenerateFirstPhase();
                    AddChild(chunk);
                    _chunks.Add(chunkPos, chunk);
                }
            }
        }
        
        foreach (var chunk in _chunks.Values)
        {
            chunk.GenerateSecondPhase();
            chunk.GenerateMesh();
        }
    }
    
    public Chunk? GetChunk(Vector3I chunkPos)
    {
        return _chunks.TryGetValue(chunkPos, out var chunk) ? chunk : null;
    }

    public Voxel GetVoxel(Vector3I voxelPos)
    {
        return GetChunk(GetChunkPos(voxelPos))?.GetVoxel(GetLocalPos(voxelPos)) ?? Voxel.Air;
    }
    
    public void SetVoxel(Vector3I voxelPos, Voxel voxel)
    {
        GetChunk(GetChunkPos(voxelPos))?.SetVoxel(GetLocalPos(voxelPos), voxel);
    }
    
    public static Vector3I GetVoxelPos(Vector3 worldPos)
    {
        return new Vector3I(
            Mathf.FloorToInt(worldPos.X),
            Mathf.FloorToInt(worldPos.Y),
            Mathf.FloorToInt(worldPos.Z)
        );
    }
    
    public static Vector3I GetChunkPos(Vector3I voxelPos)
    {
        return new Vector3I(
            Mathf.FloorToInt(voxelPos.X / (float)Chunk.Size),
            Mathf.FloorToInt(voxelPos.Y / (float)Chunk.Size),
            Mathf.FloorToInt(voxelPos.Z / (float)Chunk.Size)
        );
    }
    
    public static Vector3I GetLocalPos(Vector3I voxelPos)
    {
        return new Vector3I(
            Mathf.PosMod(voxelPos.X, Chunk.Size),
            Mathf.PosMod(voxelPos.Y, Chunk.Size),
            Mathf.PosMod(voxelPos.Z, Chunk.Size)
        );
    }
}
