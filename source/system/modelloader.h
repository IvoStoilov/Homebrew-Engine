#pragma once
#include "system/math/vec4.h"
#include "system/math/vec2.h"
#include <string.h>
#include <vector>
/*
.bgd (Basic Geometry Data file) - data structure : x, y, z, tu, tv, nx, ny, nz / Starts with vertex count / each 3 lines from a triangle
.bmp - now supports bmp loading. Consider changing the class name to LoaderHelper
*/

struct RawGeometryData
{
    std::vector<vec4> m_Positions;
    std::vector<vec4> m_Normals;
    std::vector<vec2> m_UVs;

    std::vector<uint32_t> m_PosIndexes;
    std::vector<uint32_t> m_NormIndexes;
    std::vector<uint32_t> m_UVIndexes;
};


class ModelLoader
{
public:
    struct VertexData
    {
        //TODO : Deprecate
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
    static bool LoadOBJFile(const std::string& filepath, RawGeometryData& outData);
};