using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Threading;
using Godot;
using Mutex = Godot.Mutex;

namespace VoxelGame.addons.blocks.OldTerrain.Terrain;

public partial class ChunkMeshManager : Node
{
    private readonly GodotThread _thread = new();
    private readonly Mutex _mutex = new();
    private volatile bool _shouldExit;

    // TODO: Put the number of chunks to build in the debug overlay.
    private readonly ConcurrentStack<ChunkData> _chunksToBuild = new();
    private readonly AutoResetEvent _runThreadEvent = new(false);

    // ===== Only for the thread to access =====
    public readonly Dictionary<Vector3I, ChunkMeshInstance> ChunkMeshes = new();
    public readonly FaceVisitedTracker FaceVisitedTracker = new();
    // =========================================

    // TODO: Compute block data, such as whether it is rendered or not, and pass through constructor.
    public override void _Ready()
    {
        _thread.Start(Callable.From(ThreadProcess));
    }

    private void ThreadProcess()
    {
        while (!_shouldExit)
        {
            if (_chunksToBuild.IsEmpty && !_runThreadEvent.WaitOne()) continue;

            if (!_chunksToBuild.TryPop(out var realData)) continue;

            realData.Mutex.Lock();
            if (!realData.Dirty)
            {
                realData.Mutex.Unlock();
                continue;
            }

            var copyData = new ChunkData(realData);
            realData.Dirty = false;
            realData.Mutex.Unlock();

            if (!ChunkMeshes.TryGetValue(copyData.ChunkPos, out var meshInstance))
            {
                meshInstance = new ChunkMeshInstance(copyData.ChunkNode, copyData, this);
                ChunkMeshes.Add(copyData.ChunkPos, meshInstance);
            }
            UpdateMeshAndNeighbors(meshInstance);
        }
    }

    private void UpdateMeshAndNeighbors(ChunkMeshInstance meshInstance)
    {
        var chunkPos = meshInstance.Data.ChunkPos;
        meshInstance.UpdateMesh(meshInstance.Data);

        var neighborPos = chunkPos + Vector3I.Up;
        if (ChunkMeshes.TryGetValue(neighborPos, out var neighborMesh))
            neighborMesh.UpdateMesh(neighborMesh.Data);

        neighborPos = chunkPos + Vector3I.Down;
        if (ChunkMeshes.TryGetValue(neighborPos, out neighborMesh))
            neighborMesh.UpdateMesh(neighborMesh.Data);

        neighborPos = chunkPos + Vector3I.Right;
        if (ChunkMeshes.TryGetValue(neighborPos, out neighborMesh))
            neighborMesh.UpdateMesh(neighborMesh.Data);

        neighborPos = chunkPos + Vector3I.Left;
        if (ChunkMeshes.TryGetValue(neighborPos, out neighborMesh))
            neighborMesh.UpdateMesh(neighborMesh.Data);

        neighborPos = chunkPos + Vector3I.Forward;
        if (ChunkMeshes.TryGetValue(neighborPos, out neighborMesh))
            neighborMesh.UpdateMesh(neighborMesh.Data);

        neighborPos = chunkPos + Vector3I.Back;
        if (ChunkMeshes.TryGetValue(neighborPos, out neighborMesh))
            neighborMesh.UpdateMesh(neighborMesh.Data);
    }

    public void AddChunkToBuild(ChunkData chunkData)
    {
        _chunksToBuild.Push(chunkData);
        _runThreadEvent.Set();
    }

    public override void _ExitTree()
    {
        _shouldExit = true;
        _runThreadEvent.Set();
        _thread.WaitToFinish();
    }
}

public class FaceVisitedTracker
{
    public readonly bool[] Up = new bool[Chunk.Volume];
    public readonly bool[] Down = new bool[Chunk.Volume];
    public readonly bool[] Right = new bool[Chunk.Volume];
    public readonly bool[] Left = new bool[Chunk.Volume];
    public readonly bool[] Forward = new bool[Chunk.Volume];
    public readonly bool[] Back = new bool[Chunk.Volume];

    public void Reset()
    {
        for (var i = 0; i < Chunk.Volume; i++)
        {
            Up[i] = false;
            Down[i] = false;
            Right[i] = false;
            Left[i] = false;
            Forward[i] = false;
            Back[i] = false;
        }
    }
}
