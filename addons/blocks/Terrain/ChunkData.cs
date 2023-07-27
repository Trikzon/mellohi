using Godot;

namespace Blocks.Terrain;

public class ChunkData
{
    public readonly Mutex Mutex = new();
    public bool Dirty = true;

    public readonly Vector3I ChunkPos;
    public readonly Node ChunkNode;
    public int[] Blocks = new int[Chunk.Volume];

    public ChunkData(Vector3I chunkPos, Node chunkNode)
    {
        ChunkPos = chunkPos;
        ChunkNode = chunkNode;
    }

    public ChunkData(ChunkData chunkData)
    {
        ChunkPos = chunkData.ChunkPos;
        ChunkNode = chunkData.ChunkNode;
        Mutex.Lock();
        chunkData.Blocks.CopyTo(Blocks, 0);
        Mutex.Unlock();
    }
}
