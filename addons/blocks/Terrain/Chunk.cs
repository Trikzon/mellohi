using Godot;

namespace Blocks.Terrain;

public partial class Chunk : StaticBody3D
{
    public const int Size = 32;
    public const int Area = Size * Size;
    public const int Volume = Size * Size * Size;

    public readonly ChunkData Data;

    public Chunk() : this(Vector3I.Zero) { }

    public Chunk(Vector3I chunkPos)
    {
        Data = new ChunkData(chunkPos, this);
        var noise = new FastNoiseLite();
        noise.Seed = 1;

        Data.Mutex.Lock();
        for (var y = 0; y < Size; y++)
        {
            for (var z = 0; z < Size; z++)
            {
                for (var x = 0; x < Size; x++)
                {
                    var localPos = new Vector3I(x, y, z);
                    var worldPos = Data.ChunkPos * Size + localPos;
                    if (noise.GetNoise3Dv(worldPos) > -0.25)
                        Data.Blocks[x + z * Size + y * Area] = 1;
                }
            }
        }
        Data.Dirty = true;
        Data.Mutex.Unlock();
    }

    public override void _Ready()
    {
        Name = "Chunk " + Data.ChunkPos;
        Position = Data.ChunkPos * Size;
    }

    public void PostGenerate(Terrain terrain)
    {
        Data.Mutex.Lock();
        for (var y = Size - 1; y >= 0; y--)
        for (var z = 0; z < Size; z++)
        for (var x = 0; x < Size; x++)
        {
            if (Data.Blocks[x + z * Size + y * Area] == 0)
                continue;

            var blockAbove = 0;
            var blockAboveAbove = 0;

            if (y + 1 >= Size)
            {
                if (terrain.Chunks.TryGetValue(Data.ChunkPos + Vector3I.Up, out var upChunk))
                {
                    blockAbove = upChunk.Data.Blocks[x + z * Size + 0 * Area];
                    blockAboveAbove = upChunk.Data.Blocks[x + z * Size + 1 * Area];
                }
            }
            else if (y + 2 >= Size)
            {
                if (terrain.Chunks.TryGetValue(Data.ChunkPos + Vector3I.Up, out var upChunk))
                {
                    blockAbove = Data.Blocks[x + z * Size + (y + 1) * Area];
                    blockAboveAbove = upChunk.Data.Blocks[x + z * Size + 0 * Area];
                }
            }
            else
            {
                blockAbove = Data.Blocks[x + z * Size + (y + 1) * Area];
                blockAboveAbove = Data.Blocks[x + z * Size + (y + 2) * Area];
            }

            if (blockAbove == 0)
                Data.Blocks[x + z * Size + y * Area] = 3;
            else if (blockAbove == 3 || blockAboveAbove == 3)
                Data.Blocks[x + z * Size + y * Area] = 2;
        }
        Data.Mutex.Unlock();
    }
}
