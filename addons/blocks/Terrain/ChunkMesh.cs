using System.Collections.Generic;
using System.Linq;
using Godot;

namespace Blocks.Terrain;

public partial class ChunkMesh : ArrayMesh
{
    private readonly List<Vector3> _vertices = new();
    private readonly List<Vector3> _normals = new();
    private readonly List<int> _indices = new();
    private readonly List<Color> _colors = new();

    private readonly ChunkData _data;
    private readonly ChunkData _dataUp;
    private readonly ChunkData _dataDown;
    private readonly ChunkData _dataRight;
    private readonly ChunkData _dataLeft;
    private readonly ChunkData _dataForward;
    private readonly ChunkData _dataBack;
    private readonly FaceVisitedTracker _facesVisited;

    public ChunkMesh(ChunkData data, ChunkMeshManager manager)
    {
        _data = data;
        _dataUp = manager.ChunkMeshes.TryGetValue(data.ChunkPos + Vector3I.Up, out var up) ? up.Data : null;
        _dataDown = manager.ChunkMeshes.TryGetValue(data.ChunkPos + Vector3I.Down, out var down) ? down.Data : null;
        _dataRight = manager.ChunkMeshes.TryGetValue(data.ChunkPos + Vector3I.Right, out var right) ? right.Data : null;
        _dataLeft = manager.ChunkMeshes.TryGetValue(data.ChunkPos + Vector3I.Left, out var left) ? left.Data : null;
        _dataForward = manager.ChunkMeshes.TryGetValue(data.ChunkPos + Vector3I.Forward, out var forward) ? forward.Data : null;
        _dataBack = manager.ChunkMeshes.TryGetValue(data.ChunkPos + Vector3I.Back, out var back) ? back.Data : null;

        _facesVisited = manager.FaceVisitedTracker;
        _facesVisited.Reset();

        for (var y = 0; y < Chunk.Size; y++)
        {
            for (var z = 0; z < Chunk.Size; z++)
            {
                for (var x = 0; x < Chunk.Size; x++)
                {
                    var block = _data.Blocks[x + z * Chunk.Size + y * Chunk.Area];
                    // TODO: Check if block is rendered instead.
                    if (block < 1) continue;

                    CreateGreedyFaces(block, x, y, z);
                }
            }
        }

        if (_vertices.Count == 0) return;

        var surface = new Godot.Collections.Array();
        surface.Resize((int)ArrayType.Max);

        surface[(int)ArrayType.Vertex] = _vertices.ToArray();
        surface[(int)ArrayType.Normal] = _normals.ToArray();
        surface[(int)ArrayType.Index] = _indices.ToArray();
        surface[(int)ArrayType.Color] = _colors.ToArray();

        AddSurfaceFromArrays(PrimitiveType.Triangles, surface);
    }

    private void CreateGreedyFaces(int block, int x, int y, int z)
    {
        FindGreedyFaceSize(block, x, y, z, _facesVisited.Up, Vector3I.Up, out var length, out var width);
        if (length > 0)
        {
            var v0 = new Vector3(x, y + 1, z);
            var v1 = new Vector3(x + width, y + 1, z);
            var v2 = new Vector3(x + width, y + 1, z + length);
            var v3 = new Vector3(x, y + 1, z + length);
            AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 1, 2, 0, 2, 3}, GetColor(block), Vector3.Up);
        }

        FindGreedyFaceSize(block, x, y, z, _facesVisited.Down, Vector3I.Down, out length, out width);
        if (length > 0)
        {
            var v0 = new Vector3(x, y, z);
            var v1 = new Vector3(x + width, y, z);
            var v2 = new Vector3(x + width, y, z + length);
            var v3 = new Vector3(x, y, z + length);
            AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 2, 1, 0, 3, 2}, GetColor(block), Vector3.Down);
        }

        FindGreedyFaceSize(block, x, y, z, _facesVisited.Right, Vector3I.Right, out length, out width);
        if (length > 0)
        {
            var v0 = new Vector3(x + 1, y, z);
            var v1 = new Vector3(x + 1,y + length, z);
            var v2 = new Vector3(x + 1, y + length, z + width);
            var v3 = new Vector3(x + 1, y, z + width);
            AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 2, 1, 0, 3, 2}, GetColor(block), Vector3.Right);
        }

        FindGreedyFaceSize(block, x, y, z, _facesVisited.Left, Vector3I.Left, out length, out width);
        if (length > 0)
        {
            var v0 = new Vector3(x, y, z);
            var v1 = new Vector3(x, y + length, z);
            var v2 = new Vector3(x, y + length, z + width);
            var v3 = new Vector3(x, y, z + width);
            AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 1, 2, 0, 2, 3}, GetColor(block), Vector3.Left);
        }

        FindGreedyFaceSize(block, x, y, z, _facesVisited.Forward, Vector3I.Forward, out length, out width);
        if (length > 0)
        {
            var v0 = new Vector3(x, y, z);
            var v1 = new Vector3(x + width, y, z);
            var v2 = new Vector3(x + width, y + length, z);
            var v3 = new Vector3(x, y + length, z);
            AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 1, 2, 0, 2, 3}, GetColor(block), Vector3.Forward);
        }

        FindGreedyFaceSize(block, x, y, z, _facesVisited.Back, Vector3I.Back, out length, out width);
        if (length > 0)
        {
            var v0 = new Vector3(x, y, z + 1);
            var v1 = new Vector3(x + width, y, z + 1);
            var v2 = new Vector3(x + width, y + length, z + 1);
            var v3 = new Vector3(x, y + length, z + 1);
            AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 2, 1, 0, 3, 2}, GetColor(block), Vector3.Back);
        }
    }

    private void FindGreedyFaceSize(int block, int x, int y, int z, bool[] facesVisited, Vector3I dir, out int length,
        out int width)
    {
        var blockIndex = x + z * Chunk.Size + y * Chunk.Area;
        length = 0;
        width = 1;

        // Return early if this face has already been visited or if the face is hidden.
        if (facesVisited[blockIndex] || IsFaceHidden(x, y, z, dir)) return;

        // ===== 1D Greedy Mesh Pass =====
        // This pass iterates over blocks in one direction and checks if the blocks are the same.
        var visitingPos = new Vector3I(x, y, z);

        // If the direction is up or down, we want to iterate over the z axis. Otherwise, the y axis.
        var iterDir = dir.Abs() switch
        {
            (_, 1, _) => new Vector3I(0, 0, 1),
            _ => new Vector3I(0, 1, 0)
        };
        var iterAxis = (int)iterDir.MaxAxisIndex();
        var maxPos = iterDir * Chunk.Size;

        while (visitingPos[iterAxis] < maxPos[iterAxis])
        {
            // If the block at the visiting position is not the same as the block who's face we're building, stop.
            if (_data.Blocks[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Area] != block) break;
            if (facesVisited[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Area]) break;
            if (IsFaceHidden(visitingPos.X, visitingPos.Y, visitingPos.Z, dir)) break;

            // Mark the face as visited.
            facesVisited[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Area] = true;

            length++;

            visitingPos += iterDir;
        }

        // ===== 2D Greedy Mesh Pass =====
        // This pass iterates in the other direction to see if the 1D mesh can be extended that way.

        // If the direction is left or right we want to iterate over the z axis. Otherwise, the x axis.
        var firstIterDir = dir.Abs() switch
        {
            (1, _, _) => new Vector3I(0, 0, 1),
            _ => new Vector3I(1, 0, 0)
        };
        var firstIterAxis = (int)firstIterDir.MaxAxisIndex();
        var firstMaxPos = firstIterDir * Chunk.Size;

        // If the direction is up or down, we want to iterate over the z axis. Otherwise, the y axis.
        var secondIterDir = dir.Abs() switch
        {
            (_, 1, _) => new Vector3I(0, 0, 1),
            _ => new Vector3I(0, 1, 0)
        };
        var secondIterAxis = (int)secondIterDir.MaxAxisIndex();

        // Start at the first block in the new direction.
        visitingPos = new Vector3I(x, y, z) + firstIterDir;

        // Save the initial value of the second iteration axis so we can reset it later.
        var initialSecondIterValue = visitingPos[secondIterAxis];

        var stop = false;
        while (visitingPos[firstIterAxis] < firstMaxPos[firstIterAxis] && !stop)
        {
            // Check if the block is the same as the block who's face we're building.
            visitingPos[secondIterAxis] = initialSecondIterValue;
            while (visitingPos[secondIterAxis] < initialSecondIterValue + length && !stop)
            {
                stop |= _data.Blocks[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Area] != block;
                stop |= facesVisited[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Area];
                stop |= IsFaceHidden(visitingPos.X, visitingPos.Y, visitingPos.Z, dir);
                visitingPos += secondIterDir;
            }

            if (stop) continue;

            // If not stop, that means the face can be extended in this direction by one.
            width++;

            // Mark all the blocks in this direction as visited.
            visitingPos[secondIterAxis] = initialSecondIterValue;
            while (visitingPos[secondIterAxis] < initialSecondIterValue + length)
            {
                facesVisited[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Area] = true;
                visitingPos += secondIterDir;
            }

            visitingPos += firstIterDir;
        }
    }

    // TODO: Change air check to check if the block is opaque.
    private bool IsFaceHidden(int x, int y, int z, Vector3I dir)
    {
        x += dir.X;
        y += dir.Y;
        z += dir.Z;

        switch (y)
        {
            case >= Chunk.Size:
                return _dataUp != null && _dataUp.Blocks[x + z * Chunk.Size + 0 * Chunk.Area] != 0;
            case < 0:
                return _dataDown != null && _dataDown.Blocks[x + z * Chunk.Size + (Chunk.Size - 1) * Chunk.Area] != 0;
        }

        switch (x)
        {
            case >= Chunk.Size:
                return _dataRight != null && _dataRight.Blocks[0 + z * Chunk.Size + y * Chunk.Area] != 0;
            case < 0:
                return _dataLeft != null && _dataLeft.Blocks[(Chunk.Size - 1) + z * Chunk.Size + y * Chunk.Area] != 0;
        }

        switch (z)
        {
            case < 0:
                return _dataForward != null && _dataForward.Blocks[x + (Chunk.Size - 1) * Chunk.Size + y * Chunk.Area] != 0;
            case >= Chunk.Size:
                return _dataBack != null && _dataBack.Blocks[x + 0 * Chunk.Size + y * Chunk.Area] != 0;
        }

        return _data.Blocks[x + z * Chunk.Size + y * Chunk.Area] != 0;
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

    // TODO: Replace with textures
    private Color GetColor(int block)
    {
        return block switch
        {
            1 => Colors.Green,
            2 => Colors.Brown,
            3 => Colors.Gray,
            _ => Colors.Pink
        };
    }
}

