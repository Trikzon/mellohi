
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
            for (var y = 0; y < 1; y++)
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

    public int GetVoxel(Vector3I blockPos)
    {
        var chunkPos = new Vector3I(
            Mathf.FloorToInt(blockPos.X / (float)Chunk.Size),
            Mathf.FloorToInt(blockPos.Y / (float)Chunk.Size),
            Mathf.FloorToInt(blockPos.Z / (float)Chunk.Size)
        );
        var chunk = GetChunk(chunkPos);
        return chunk?.GetVoxel(new Vector3I(
            Mathf.PosMod(blockPos.X, Chunk.Size),
            Mathf.PosMod(blockPos.Y, Chunk.Size),
            Mathf.PosMod(blockPos.Z, Chunk.Size)
        )) ?? 0;
    }
}
