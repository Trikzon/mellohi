using System.Collections.Generic;
using Godot;

namespace Blocks.Terrain;

[GlobalClass]
public partial class Terrain : Node3D
{
    private readonly Dictionary<Vector3I, Chunk> _chunks = new();

    private readonly ChunkMeshManager _chunkMeshManager = new();

    public override void _Ready()
    {
        // GetViewport().DebugDraw = Viewport.DebugDrawEnum.Wireframe;
        AddChild(_chunkMeshManager);

        for (var x = 0; x <= 1; x++)
        for (var z = 0; z <= 1; z++)
        for (var y = 0; y <= 1; y++)
            _chunks[new Vector3I(x, y, z)] = new Chunk(new Vector3I(x, y, z));

        foreach (var chunk in _chunks.Values)
        {
            AddChild(chunk);
            _chunkMeshManager.AddChunkToBuild(chunk.Data);
        }
    }
}
