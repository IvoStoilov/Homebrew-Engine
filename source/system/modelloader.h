#pragma once
#include <string.h>
#include <vector>
/*
.bgd (Basic Geometry Data file) - data structure : x, y, z, tu, tv, nx, ny, nz / Starts with vertex count / each 3 lines from a triangle
*/
class ModelLoader
{
public:
    class VertexData
    {
        float position[3];
        float uv[2];
        float normal[3];
    };
public:
    static bool LoadBGDFile(const std::string& filepath, std::vector<VertexData>& outData);
};