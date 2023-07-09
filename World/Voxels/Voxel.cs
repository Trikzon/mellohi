using Godot;

namespace voxelgame.World.Voxels;

public class Voxel
{
    public string Name { get; }
    public Color Color { get; }

    private Voxel(string name, Color color)
    {
        Name = name;
        Color = color;
    }
    
    public bool IsSolid => this != Air;

    public static readonly Voxel Air = new("air", Colors.Transparent);
    public static readonly Voxel Dirt = new("dirt", Colors.Brown);
    public static readonly Voxel Grass = new("grass", Colors.Green);
    public static readonly Voxel Stone = new("stone", Colors.Gray);
}