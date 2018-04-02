#pragma once
#include <string.h>
#include <vector>
/*
.bgd (Basic Geometry Data file) - data structure : x, y, z, tu, tv, nx, ny, nz / Starts with vertex count / each 3 lines from a triangle
.bmp - now supports bmp loading. Consider changing the class name to LoaderHelper
*/
class ModelLoader
{
public:
    struct VertexData
    {
        float position[3];
        float uv[2];
        float normal[3];
        float color[3];
    };
public:
    static bool LoadBGDFile(const std::string& filepath, std::vector<VertexData>& outData);

    //VertexData.uv    -> width/height of image;
    //VertexData.color -> color of the pixel
    static bool LoadBMPFile(const std::string& filepath, std::vector<VertexData>& outData, int32_t& outBMPwidth, int32_t& outBMPheight);
};