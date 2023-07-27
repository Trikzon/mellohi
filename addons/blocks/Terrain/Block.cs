using Godot;

namespace Blocks.Terrain;

[GlobalClass]
public partial class Block : Resource
{
    public Block() : this("", true)
    {
    }

    public Block(string name, bool isRendered)
    {
        Name = name;
        IsRendered = isRendered;
    }

    [Export] public string Name { get; set; }

    [Export] public bool IsRendered { get; set; }

    public virtual void PrintBlock()
    {
        GD.Print("Hello from block");
    }
}