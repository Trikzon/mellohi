
using Godot;
using System;
using System.Collections.Generic;

namespace voxelgame.World;

public partial class Dimension : Node3D
{
    private readonly Dictionary<Vector3I, Chunk> _chunks = new();

    public override void _Ready()
    {
        for (var x = -2; x < 2; x++)
        {
            for (var y = -2; y < 2; y++)
            {
                for (var z = -2; z < 2; z++)
                {
                    var chunkPos = new Vector3I(x, y, z);
                    var chunk = Chunk.Create(this, chunkPos);
                    chunk.GenerateChunkDate();
                    AddChild(chunk);
                    _chunks.Add(chunkPos, chunk);
                }
            }
        }
        
        foreach (var chunk in _chunks.Values)
        {
            chunk.GenerateMesh();
        }
    }
    
    public Chunk? GetChunk(Vector3I chunkPos)
    {
        return _chunks.TryGetValue(chunkPos, out var chunk) ? chunk : null;
    }

    public int GetVoxel(Vector3I voxelPos)
    {
        return GetChunk(GetChunkPos(voxelPos))?.GetVoxel(GetLocalPos(voxelPos)) ?? 0;
    }
    
    public void SetVoxel(Vector3I voxelPos, int value)
    {
        GetChunk(GetChunkPos(voxelPos))?.SetVoxel(GetLocalPos(voxelPos), value);
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
