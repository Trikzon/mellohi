using System.Collections.Generic;
using Godot;

namespace VoxelGame.addons.blocks.OldTerrain.Terrain;

public partial class Terrain : Node3D
{
    private readonly ChunkMeshManager _chunkMeshManager = new();

    public readonly Dictionary<Vector3I, Chunk> Chunks = new();

    public override void _Ready()
    {
        // GetViewport().DebugDraw = Viewport.DebugDrawEnum.Wireframe;
        AddChild(_chunkMeshManager);

        for (var x = 0; x <= 3; x++)
        for (var z = 0; z <= 3; z++)
        for (var y = 0; y <= 1; y++)
            Chunks[new Vector3I(x, y, z)] = new Chunk(new Vector3I(x, y, z));

        // Temporary generation to test other textures
        foreach (var chunk in Chunks.Values)
        {
            chunk.PostGenerate(this);
        }

        foreach (var chunk in Chunks.Values)
        {
            AddChild(chunk);
            _chunkMeshManager.AddChunkToBuild(chunk.Data);
        }
    }
}
