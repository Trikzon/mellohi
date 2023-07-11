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

    private Chunk? _chunk;
    private FaceVisitedTracker? _faceTracker;
    private Chunk? _chunkUp;
    private Chunk? _chunkDown;
    private Chunk? _chunkRight;
    private Chunk? _chunkLeft;
    private Chunk? _chunkForward;
    private Chunk? _chunkBack;

    public void GenerateMesh(Dimension dimension, Chunk chunk)
    {
        _chunk = chunk;
        _faceTracker = new FaceVisitedTracker();
        _chunkUp = dimension.GetChunk(chunk.ChunkPos + Vector3I.Up);
        _chunkDown = dimension.GetChunk(chunk.ChunkPos + Vector3I.Down);
        _chunkRight = dimension.GetChunk(chunk.ChunkPos + Vector3I.Right);
        _chunkLeft = dimension.GetChunk(chunk.ChunkPos + Vector3I.Left);
        _chunkForward = dimension.GetChunk(chunk.ChunkPos + Vector3I.Forward);
        _chunkBack = dimension.GetChunk(chunk.ChunkPos + Vector3I.Back);
        
        var localPos = new Vector3I();
        for (localPos.Y = 0; localPos.Y < Chunk.Size; localPos.Y++)
        {
            for (localPos.Z = 0; localPos.Z < Chunk.Size; localPos.Z++)
            {
                for (localPos.X = 0; localPos.X < Chunk.Size; localPos.X++)
                {
                    var voxel = chunk.GetVoxel(localPos);
                    if (voxel == Voxel.Air) continue;

                    CreateStrip(voxel, localPos);
                }
            }
        }
        
        FillArrayMesh();
    }

    private void CreateStrip(Voxel voxel, Vector3I localPos)
    {
        var access = localPos.X + localPos.Z * Chunk.Size + localPos.Y * Chunk.Size * Chunk.Size;

        var length = 0;
        var hiddenLength = 0;
        var width = 1;
        var visitingPos = localPos;
        if (!_faceTracker!.VisitedUp[access] && !IsVoxelSolidUp(localPos + Vector3I.Up))  // Up
        {
            for (visitingPos.Z = localPos.Z; visitingPos.Z < Chunk.Size; visitingPos.Z++)
            {
                if (_chunk!.GetVoxel(visitingPos) != voxel)
                    break;

                if (IsVoxelSolidUp(visitingPos + Vector3I.Up))
                    hiddenLength += 1;
                else
                    hiddenLength = 0;

                _faceTracker.VisitedUp[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size] = true;
                length++;
            }

            if (hiddenLength > 0)
            {
                length -= hiddenLength;
                for (; visitingPos.Z < localPos.Z + length; visitingPos.Z--)
                {
                    _faceTracker.VisitedUp[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size] = false;
                }
            }

            var stop = false;
            for (visitingPos.X = localPos.X + 1; visitingPos.X < Chunk.Size && !stop; visitingPos.X++)
            {
                for (visitingPos.Z = localPos.Z; visitingPos.Z < localPos.Z + length && !stop; visitingPos.Z++)
                {
                    stop |= _chunk!.GetVoxel(visitingPos) != voxel;
                    stop |= _faceTracker.VisitedUp[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size];
                    stop |= IsVoxelSolidUp(visitingPos + Vector3I.Up);
                }

                if (stop) continue;
                
                width += 1;
                for (visitingPos.Z = localPos.Z; visitingPos.Z < localPos.Z + length; visitingPos.Z++)
                    _faceTracker.VisitedUp[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size] = true;
            }
            
            if (length > 0)
            {
                var v0 = new Vector3(localPos.X, localPos.Y + 1, localPos.Z);
                var v1 = new Vector3(localPos.X + width, localPos.Y + 1, localPos.Z);
                var v2 = new Vector3(localPos.X + width, localPos.Y + 1, localPos.Z + length);
                var v3 = new Vector3(localPos.X, localPos.Y + 1, localPos.Z + length);
                AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 1, 2, 0, 2, 3}, voxel.Color, Vector3.Up);
            }
        }

        length = 0;
        hiddenLength = 0;
        width = 1;
        visitingPos = localPos;
        if (!_faceTracker!.VisitedDown[access] && !IsVoxelSolidDown(localPos + Vector3I.Down)) // Down
        {
            for (visitingPos.Z = localPos.Z; visitingPos.Z < Chunk.Size; visitingPos.Z++)
            {
                if (_chunk!.GetVoxel(visitingPos) != voxel)
                    break;

                if (IsVoxelSolidDown(visitingPos + Vector3I.Down))
                    hiddenLength += 1;
                else
                    hiddenLength = 0;

                _faceTracker.VisitedDown[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size] = true;
                length++;
            }
            
            if (hiddenLength > 0)
            {
                length -= hiddenLength;
                for (; visitingPos.Z < localPos.Z + length; visitingPos.Z--)
                {
                    _faceTracker.VisitedDown[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size] = false;
                }
            }

            var stop = false;
            for (visitingPos.X = localPos.X + 1; visitingPos.X < Chunk.Size && !stop; visitingPos.X++)
            {
                for (visitingPos.Z = localPos.Z; visitingPos.Z < localPos.Z + length && !stop; visitingPos.Z++)
                {
                    stop |= _chunk!.GetVoxel(visitingPos) != voxel;
                    stop |= _faceTracker.VisitedDown[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size];
                    stop |= IsVoxelSolidDown(visitingPos + Vector3I.Down);
                }

                if (stop) continue;
                
                width += 1;
                for (visitingPos.Z = localPos.Z; visitingPos.Z < localPos.Z + length; visitingPos.Z++)
                    _faceTracker.VisitedDown[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size] = true;
            }

            if (length > 0)
            {
                var v0 = new Vector3(localPos.X, localPos.Y, localPos.Z);
                var v1 = new Vector3(localPos.X + width, localPos.Y, localPos.Z);
                var v2 = new Vector3(localPos.X + width, localPos.Y, localPos.Z + length);
                var v3 = new Vector3(localPos.X, localPos.Y, localPos.Z + length);
                AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 2, 1, 0, 3, 2}, voxel.Color, Vector3.Down);
            }
        }
        
        length = 0;
        hiddenLength = 0;
        width = 1;
        visitingPos = localPos;
        if (!_faceTracker!.VisitedRight[access] && !IsVoxelSolidRight(localPos + Vector3I.Right))  // Right
        {
            for (visitingPos.Y = localPos.Y; visitingPos.Y < Chunk.Size; visitingPos.Y++)
            {
                if (_chunk!.GetVoxel(visitingPos) != voxel)
                    break;

                if (IsVoxelSolidRight(visitingPos + Vector3I.Right))
                    hiddenLength += 1;
                else
                    hiddenLength = 0;

                _faceTracker.VisitedRight[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size] = true;
                length++;
            }

            if (hiddenLength > 0)
            {
                length -= hiddenLength;
                for (; visitingPos.Y < localPos.Y + length; visitingPos.Y--)
                {
                    _faceTracker.VisitedRight[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size] = false;
                }
            }

            var stop = false;
            for (visitingPos.Z = localPos.Z + 1; visitingPos.Z < Chunk.Size && !stop; visitingPos.Z++)
            {
                for (visitingPos.Y = localPos.Y; visitingPos.Y < localPos.Y + length && !stop; visitingPos.Y++)
                {
                    stop |= _chunk!.GetVoxel(visitingPos) != voxel;
                    stop |= _faceTracker.VisitedRight[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size];
                    stop |= IsVoxelSolidRight(visitingPos + Vector3I.Right);
                }

                if (stop) continue;

                width += 1;
                for (visitingPos.Y = localPos.Y; visitingPos.Y < localPos.Y + length; visitingPos.Y++)
                    _faceTracker.VisitedRight[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size] = true;
            }

            if (length > 0)
            {
                var v0 = new Vector3(localPos.X + 1, localPos.Y, localPos.Z);
                var v1 = new Vector3(localPos.X + 1, localPos.Y + length, localPos.Z);
                var v2 = new Vector3(localPos.X + 1, localPos.Y + length, localPos.Z + width);
                var v3 = new Vector3(localPos.X + 1, localPos.Y, localPos.Z + width);
                AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 2, 1, 0, 3, 2}, voxel.Color, Vector3.Right);
            }
        }
        
        length = 0;
        hiddenLength = 0;
        width = 1;
        visitingPos = localPos;
        if (!_faceTracker!.VisitedLeft[access] && !IsVoxelSolidLeft(localPos + Vector3I.Left))  // Left
        {
            for (visitingPos.Y = localPos.Y; visitingPos.Y < Chunk.Size; visitingPos.Y++)
            {
                if (_chunk!.GetVoxel(visitingPos) != voxel)
                    break;

                if (IsVoxelSolidLeft(visitingPos + Vector3I.Left))
                    hiddenLength += 1;
                else
                    hiddenLength = 0;

                _faceTracker.VisitedLeft[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size] = true;
                length++;
            }

            if (hiddenLength > 0)
            {
                length -= hiddenLength;
                for (; visitingPos.Y < localPos.Y + length; visitingPos.Y--)
                {
                    _faceTracker.VisitedLeft[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size] = false;
                }
            }

            var stop = false;
            for (visitingPos.Z = localPos.Z + 1; visitingPos.Z < Chunk.Size && !stop; visitingPos.Z++)
            {
                for (visitingPos.Y = localPos.Y; visitingPos.Y < localPos.Y + length && !stop; visitingPos.Y++)
                {
                    stop |= _chunk!.GetVoxel(visitingPos) != voxel;
                    stop |= _faceTracker.VisitedLeft[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size];
                    stop |= IsVoxelSolidLeft(visitingPos + Vector3I.Left);
                }

                if (stop) continue;

                width += 1;
                for (visitingPos.Y = localPos.Y; visitingPos.Y < localPos.Y + length; visitingPos.Y++)
                    _faceTracker.VisitedLeft[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size] = true;
            }

            if (length > 0)
            {
                var v0 = new Vector3(localPos.X, localPos.Y, localPos.Z);
                var v1 = new Vector3(localPos.X, localPos.Y + length, localPos.Z);
                var v2 = new Vector3(localPos.X, localPos.Y + length, localPos.Z + width);
                var v3 = new Vector3(localPos.X, localPos.Y, localPos.Z + width);
                AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 1, 2, 0, 2, 3}, voxel.Color, Vector3.Left);
            }
        }

        
        length = 0;
        hiddenLength = 0;
        width = 1;
        visitingPos = localPos;
        if (!_faceTracker!.VisitedForward[access] && !IsVoxelSolidForward(localPos + Vector3I.Forward))  // Forward
        {
            for (visitingPos.Y = localPos.Y; visitingPos.Y < Chunk.Size; visitingPos.Y++)
            {
                if (_chunk!.GetVoxel(visitingPos) != voxel)
                    break;

                if (IsVoxelSolidForward(visitingPos + Vector3I.Forward))
                    hiddenLength += 1;
                else
                    hiddenLength = 0;

                _faceTracker.VisitedForward[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size] = true;
                length++;
            }

            if (hiddenLength > 0)
            {
                length -= hiddenLength;
                for (; visitingPos.Y < localPos.Y + length; visitingPos.Y--)
                {
                    _faceTracker.VisitedForward[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size] = false;
                }
            }

            var stop = false;
            for (visitingPos.X = localPos.X + 1; visitingPos.X < Chunk.Size && !stop; visitingPos.X++)
            {
                for (visitingPos.Y = localPos.Y; visitingPos.Y < localPos.Y + length && !stop; visitingPos.Y++)
                {
                    stop |= _chunk!.GetVoxel(visitingPos) != voxel;
                    stop |= _faceTracker.VisitedForward[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size];
                    stop |= IsVoxelSolidForward(visitingPos + Vector3I.Forward);
                }

                if (stop) continue;

                width += 1;
                for (visitingPos.Y = localPos.Y; visitingPos.Y < localPos.Y + length; visitingPos.Y++)
                    _faceTracker.VisitedForward[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size] = true;
            }

            if (length > 0)
            {
                var v0 = new Vector3(localPos.X, localPos.Y, localPos.Z);
                var v1 = new Vector3(localPos.X + width, localPos.Y, localPos.Z);
                var v2 = new Vector3(localPos.X + width, localPos.Y + length, localPos.Z);
                var v3 = new Vector3(localPos.X, localPos.Y + length, localPos.Z);
                AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 1, 2, 0, 2, 3}, voxel.Color, Vector3.Forward);
            }
        }
        
        length = 0;
        hiddenLength = 0;
        width = 1;
        visitingPos = localPos;
        if (!_faceTracker!.VisitedBack[access] && !IsVoxelSolidBack(localPos + Vector3I.Back))  // Back
        {
            for (visitingPos.Y = localPos.Y; visitingPos.Y < Chunk.Size; visitingPos.Y++)
            {
                if (_chunk!.GetVoxel(visitingPos) != voxel)
                    break;

                if (IsVoxelSolidBack(visitingPos + Vector3I.Back))
                    hiddenLength += 1;
                else
                    hiddenLength = 0;

                _faceTracker.VisitedBack[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size] = true;
                length++;
            }
            
            if (hiddenLength > 0)
            {
                length -= hiddenLength;
                for (; visitingPos.Y < localPos.Y + length; visitingPos.Y--)
                {
                    _faceTracker.VisitedBack[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size] = false;
                }
            }

            var stop = false;
            for (visitingPos.X = localPos.X + 1; visitingPos.X < Chunk.Size && !stop; visitingPos.X++)
            {
                for (visitingPos.Y = localPos.Y; visitingPos.Y < localPos.Y + length && !stop; visitingPos.Y++)
                {
                    stop |= _chunk!.GetVoxel(visitingPos) != voxel;
                    stop |= _faceTracker.VisitedBack[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size];
                    stop |= IsVoxelSolidBack(visitingPos + Vector3I.Back);
                }

                if (stop) continue;

                width += 1;
                for (visitingPos.Y = localPos.Y; visitingPos.Y < localPos.Y + length; visitingPos.Y++)
                    _faceTracker.VisitedBack[visitingPos.X + visitingPos.Z * Chunk.Size + visitingPos.Y * Chunk.Size * Chunk.Size] = true;
            }

            if (length > 0)
            {
                var v0 = new Vector3(localPos.X, localPos.Y, localPos.Z + 1);
                var v1 = new Vector3(localPos.X + width, localPos.Y, localPos.Z + 1);
                var v2 = new Vector3(localPos.X + width, localPos.Y + length, localPos.Z + 1);
                var v3 = new Vector3(localPos.X, localPos.Y + length, localPos.Z + 1);
                AddQuad(new[] {v0, v1, v2, v3}, new[] {0, 2, 1, 0, 3, 2}, voxel.Color, Vector3.Back);
            }
        }
    }

    private bool IsVoxelSolidUp(Vector3I localPos)
    {
        if (localPos.Y >= Chunk.Size)
        {
            if (_chunkUp == null) return false;

            localPos.Y = 0;
            return _chunkUp.GetVoxel(localPos).IsSolid;
        }
        return _chunk!.GetVoxel(localPos).IsSolid;
    }
    
    private bool IsVoxelSolidDown(Vector3I localPos)
    {
        if (localPos.Y < 0)
        {
            if (_chunkDown == null) return false;

            localPos.Y = Chunk.Size - 1;
            return _chunkDown.GetVoxel(localPos).IsSolid;
        }
        return _chunk!.GetVoxel(localPos).IsSolid;
    }
    
    private bool IsVoxelSolidRight(Vector3I localPos)
    {
        if (localPos.X >= Chunk.Size)
        {
            if (_chunkRight == null) return false;

            localPos.X = 0;
            return _chunkRight.GetVoxel(localPos).IsSolid;
        }
        return _chunk!.GetVoxel(localPos).IsSolid;
    }

    private bool IsVoxelSolidLeft(Vector3I localPos)
    {
        if (localPos.X < 0)
        {
            if (_chunkLeft == null) return false;

            localPos.X = Chunk.Size - 1;
            return _chunkLeft.GetVoxel(localPos).IsSolid;
        }
        return _chunk!.GetVoxel(localPos).IsSolid;
    }
    
    private bool IsVoxelSolidForward(Vector3I localPos)
    {
        if (localPos.X < 0)
        {
            if (_chunkForward == null) return false;

            localPos.X = Chunk.Size - 1;
            return _chunkForward.GetVoxel(localPos).IsSolid;
        }
        return _chunk!.GetVoxel(localPos).IsSolid;
    }
    
    private bool IsVoxelSolidBack(Vector3I localPos)
    {
        if (localPos.X >= Chunk.Size)
        {
            if (_chunkBack == null) return false;

            localPos.X = 0;
            return _chunkBack.GetVoxel(localPos).IsSolid;
        }
        return _chunk!.GetVoxel(localPos).IsSolid;
    }

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
    
    private void FillArrayMesh()
     {
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

    // TODO: Allocate one of these per thread and pass to each chunk.
    public class FaceVisitedTracker
    {
        public readonly bool[] VisitedUp = new bool[Chunk.Size * Chunk.Size * Chunk.Size];
        public readonly bool[] VisitedDown = new bool[Chunk.Size * Chunk.Size * Chunk.Size];
        public readonly bool[] VisitedRight = new bool[Chunk.Size * Chunk.Size * Chunk.Size];
        public readonly bool[] VisitedLeft = new bool[Chunk.Size * Chunk.Size * Chunk.Size];
        public readonly bool[] VisitedForward = new bool[Chunk.Size * Chunk.Size * Chunk.Size];
        public readonly bool[] VisitedBack = new bool[Chunk.Size * Chunk.Size * Chunk.Size];
    }
}
