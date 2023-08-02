using Godot;

namespace VoxelGame.addons.blocks.OldTerrain.Terrain;

public partial class ChunkMeshInstance : MeshInstance3D
{
    public ChunkData Data;

    private ChunkMeshManager _manager;
    private CollisionShape3D _collisionShape;

    public ChunkMeshInstance(Node chunkNode, ChunkData data, ChunkMeshManager manager)
    {
        Data = data;
        _manager = manager;

        chunkNode.CallDeferred(Node.MethodName.AddChild, this);

        _collisionShape = new CollisionShape3D();
        chunkNode.CallDeferred(Node.MethodName.AddChild, _collisionShape);
    }

    public void UpdateMesh(ChunkData data)
    {
        Data = data;

        GD.Print("Updating chunk mesh at " + Data.ChunkPos);
        Mesh = new ChunkMesh(data, _manager);

        _collisionShape.CallDeferred("set_shape", Mesh.CreateTrimeshShape());

        CallDeferred(Node.MethodName.SetName, $"Mesh ({Time.GetTicksMsec()})");
    }
}
