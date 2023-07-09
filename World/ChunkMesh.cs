using Godot;
using System;
using System.Collections.Generic;
using System.Linq;

namespace voxelgame.World;

public partial class ChunkMesh : MeshInstance3D
{
    private readonly List<Vector3> _vertices = new();
    private readonly List<Vector3> _normals = new();
    private readonly List<int> _indices = new();
    private readonly List<Color> _colors = new();

    private static bool IsVoid(Chunk chunk, Vector3I localPos)
    {
        return chunk.GetVoxel(localPos) == 0;
    }

    private void AddQuad(IEnumerable<Vector3> vertices, IEnumerable<int> indices, Color color, Vector3 normal)
    {
        _vertices.AddRange(vertices);
        
        for (var i = 0; i < 4; i++)
            _normals.Add(normal);
        
        var index = _vertices.Count - 4;
        _indices.AddRange(indices.Select(i => i + index).ToArray());
        
        for (var i = 0; i < 4; i++)
            _colors.Add(color);
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
                    if (voxel == 0) continue;

                    var color = voxel switch
                    {
                        1 => Colors.Brown,
                        2 => Colors.Green,
                        _ => Colors.Gray
                    };
                    
                    // Up
                    if (IsVoid(chunk, localPos + new Vector3I(0, 1, 0)))
                    {
                        var v0 = new Vector3(x, y + 1, z);
                        var v1 = new Vector3(x + 1, y + 1, z);
                        var v2 = new Vector3(x + 1, y + 1, z + 1);
                        var v3 = new Vector3(x, y + 1, z + 1);
                        AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 1, 2, 0, 2, 3}, color, Vector3.Up);
                    }
                    
                    // Down
                    if (IsVoid(chunk, localPos + new Vector3I(0, -1, 0)))
                    {
                        var v0 = new Vector3(x, y, z);
                        var v1 = new Vector3(x + 1, y, z);
                        var v2 = new Vector3(x + 1, y, z + 1);
                        var v3 = new Vector3(x, y, z + 1);
                        AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 2, 1, 0, 3, 2}, color, Vector3.Down);
                    }
                    
                    // Right
                    if (IsVoid(chunk, localPos + new Vector3I(1, 0, 0)))
                    {
                        var v0 = new Vector3(x + 1, y, z);
                        var v1 = new Vector3(x + 1, y + 1, z);
                        var v2 = new Vector3(x + 1, y + 1, z + 1);
                        var v3 = new Vector3(x + 1, y, z + 1);
                        AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 2, 1, 0, 3, 2}, color, Vector3.Right);
                    }
                    
                    // Left
                    if (IsVoid(chunk, localPos + new Vector3I(-1, 0, 0)))
                    {
                        var v0 = new Vector3(x, y, z);
                        var v1 = new Vector3(x, y + 1, z);
                        var v2 = new Vector3(x, y + 1, z + 1);
                        var v3 = new Vector3(x, y, z + 1);
                        AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 1, 2, 0, 2, 3}, color, Vector3.Left);
                    }
                    
                    // Forward
                    if (IsVoid(chunk, localPos + new Vector3I(0, 0, -1)))
                    {
                        var v0 = new Vector3(x, y, z);
                        var v1 = new Vector3(x + 1, y, z);
                        var v2 = new Vector3(x + 1, y + 1, z);
                        var v3 = new Vector3(x, y + 1, z);
                        AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 1, 2, 0, 2, 3}, color, Vector3.Forward);
                    }
                    
                    // Back
                    if (IsVoid(chunk, localPos + new Vector3I(0, 0, 1)))
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

        surface[(int)Mesh.ArrayType.Vertex] = _vertices.ToArray();
        surface[(int)Mesh.ArrayType.Normal] = _normals.ToArray();
        surface[(int)Mesh.ArrayType.Color] = _colors.ToArray();
        surface[(int)Mesh.ArrayType.Index] = _indices.ToArray();

        var arrayMesh = new ArrayMesh();
        arrayMesh.AddSurfaceFromArrays(Mesh.PrimitiveType.Triangles, surface);
        Mesh = arrayMesh;

        var material = new StandardMaterial3D();
        material.VertexColorUseAsAlbedo = true;
        MaterialOverride = material;
    }
}
