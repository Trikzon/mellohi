using Godot;
using System.Collections.Generic;
using System.Linq;
using voxelgame.World.Voxels;

namespace voxelgame.World;

public partial class ChunkMesh : MeshInstance3D
{
    public readonly List<Vector3> Vertices = new();
    public readonly List<Vector3> Normals = new();
    public readonly List<int> Indices = new();
    public readonly List<Color> Colors = new();

    private void AddQuad(IEnumerable<Vector3> vertices, IEnumerable<int> indices, Color color, Vector3 normal)
    {
        Vertices.AddRange(vertices);
        
        for (var i = 0; i < 4; i++)
            Normals.Add(normal);
        
        var index = Vertices.Count - 4;
        Indices.AddRange(indices.Select(i => i + index).ToArray());
        
        for (var i = 0; i < 4; i++)
            Colors.Add(color);
    }
    
    public void GenerateMesh(Chunk chunk)
    {
        for (var x = 0; x < 16; x++)
        {
            for (var y = 0; y < 16; y++)
            {
                for (var z = 0; z < 16; z++)
                {
                    var localPos = new Vector3I(x, y, z);
                    var voxel = chunk.GetVoxel(localPos);
                    if (voxel == Voxel.Air) continue;

                    var color = voxel.Color;
                    
                    // Up
                    if (!chunk.GetVoxel(localPos + new Vector3I(0, 1, 0)).IsSolid)
                    {
                        var v0 = new Vector3(x, y + 1, z);
                        var v1 = new Vector3(x + 1, y + 1, z);
                        var v2 = new Vector3(x + 1, y + 1, z + 1);
                        var v3 = new Vector3(x, y + 1, z + 1);
                        AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 1, 2, 0, 2, 3}, color, Vector3.Up);
                    }
                    
                    // Down
                    if (!chunk.GetVoxel(localPos + new Vector3I(0, -1, 0)).IsSolid)
                    {
                        var v0 = new Vector3(x, y, z);
                        var v1 = new Vector3(x + 1, y, z);
                        var v2 = new Vector3(x + 1, y, z + 1);
                        var v3 = new Vector3(x, y, z + 1);
                        AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 2, 1, 0, 3, 2}, color, Vector3.Down);
                    }
                    
                    // Right
                    if (!chunk.GetVoxel(localPos + new Vector3I(1, 0, 0)).IsSolid)
                    {
                        var v0 = new Vector3(x + 1, y, z);
                        var v1 = new Vector3(x + 1, y + 1, z);
                        var v2 = new Vector3(x + 1, y + 1, z + 1);
                        var v3 = new Vector3(x + 1, y, z + 1);
                        AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 2, 1, 0, 3, 2}, color, Vector3.Right);
                    }
                    
                    // Left
                    if (!chunk.GetVoxel(localPos + new Vector3I(-1, 0, 0)).IsSolid)
                    {
                        var v0 = new Vector3(x, y, z);
                        var v1 = new Vector3(x, y + 1, z);
                        var v2 = new Vector3(x, y + 1, z + 1);
                        var v3 = new Vector3(x, y, z + 1);
                        AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 1, 2, 0, 2, 3}, color, Vector3.Left);
                    }
                    
                    // Forward
                    if (!chunk.GetVoxel(localPos + new Vector3I(0, 0, -1)).IsSolid)
                    {
                        var v0 = new Vector3(x, y, z);
                        var v1 = new Vector3(x + 1, y, z);
                        var v2 = new Vector3(x + 1, y + 1, z);
                        var v3 = new Vector3(x, y + 1, z);
                        AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 1, 2, 0, 2, 3}, color, Vector3.Forward);
                    }
                    
                    // Back
                    if (!chunk.GetVoxel(localPos + new Vector3I(0, 0, 1)).IsSolid)
                    {
                        var v0 = new Vector3(x, y, z + 1);
                        var v1 = new Vector3(x + 1, y, z + 1);
                        var v2 = new Vector3(x + 1, y + 1, z + 1);
                        var v3 = new Vector3(x, y + 1, z + 1);
                        AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 2, 1, 0, 3, 2}, color, Vector3.Back);
                    }
                }
            }
        }

        var surface = new Godot.Collections.Array();
        surface.Resize((int)Mesh.ArrayType.Max);

        surface[(int)Mesh.ArrayType.Vertex] = Vertices.ToArray();
        surface[(int)Mesh.ArrayType.Normal] = Normals.ToArray();
        surface[(int)Mesh.ArrayType.Color] = Colors.ToArray();
        surface[(int)Mesh.ArrayType.Index] = Indices.ToArray();

        var arrayMesh = new ArrayMesh();
        if (Vertices.Count > 0)
        {
            arrayMesh.AddSurfaceFromArrays(Mesh.PrimitiveType.Triangles, surface);
        }
        Mesh = arrayMesh;

        var material = new StandardMaterial3D();
        material.VertexColorUseAsAlbedo = true;
        MaterialOverride = material;
    }
}
