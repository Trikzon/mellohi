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
    }

    public override void _Ready()
    {
        Name = "Chunk " + Data.ChunkPos;
        Position = Data.ChunkPos * Size;

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
}
