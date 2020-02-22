#include <system/precompile.h>
#include "system/modelloader.h"
#include "system/error.h"
#include <fstream>
#include <windows.h>
#include <stdint.h>
#include <iostream>

using namespace std;

bool ModelLoader::LoadBGDFile(const std::string& filepath, std::vector<VertexData>& outData)
{
    outData.clear();
    
    ifstream reader;
    reader.open(filepath);
    popAssert(!reader.fail(), "ModelLoader::LoadBGDFile failed to find path");

    uint32_t vertexCount = 0;
    char buffer;
    reader.get(buffer);

    while (buffer != ':')
        reader.get(buffer);

    reader >> vertexCount;

    reader.get(buffer); //Read the '/n'

    VertexData line;
    for (uint32_t i = 0; i < vertexCount; ++i)
    {
        reader >> line.position[0] >> line.position[1] >> line.position[2];
        reader >> line.uv[0] >> line.uv[1];
        reader >> line.normal[0] >> line.normal[1] >> line.normal[2];

        outData.push_back(line);
    }

    return true;
}

bool ModelLoader::LoadBMPFile(const std::string& filepath, std::vector<VertexData>& outData, int32_t& outBMPwidth, int32_t& outBMPheight)
{
    //Giving default values
    outData.clear();
    outBMPwidth = -1;
    outBMPheight = -1;

    FILE* filePtr;
    fopen_s(&filePtr, filepath.c_str(), "rb");
    
    BITMAPFILEHEADER bitmapFileHeader;
    BITMAPINFOHEADER bitmapInfoHeader;

    int32_t count = 0;

    count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
    if (count != 1)
        return false;

    count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
    if (count != 1)
        return false;

    uint32_t bytesPerPixel = (uint32_t)bitmapInfoHeader.biBitCount / 8;
    //uint32_t rawDataSize = bitmapInfoHeader.biSizeImage;
    outBMPwidth = bitmapInfoHeader.biWidth;
    outBMPheight = bitmapInfoHeader.biHeight;

    uint8_t widthLinePadding = 0;
    uint32_t rawDataSize = 0;
    if ((outBMPwidth * bytesPerPixel) % 4 != 0)
    {
        widthLinePadding = 4 - (outBMPwidth * bytesPerPixel) % 4;
    }

    rawDataSize = bytesPerPixel * outBMPwidth * outBMPheight;
    rawDataSize += widthLinePadding * outBMPheight;
    
    
    unsigned char* rawData = new unsigned char[rawDataSize];

    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

    count = fread(rawData, 1, rawDataSize, filePtr);
    if (count != rawDataSize)
        return false;

    uint32_t index = 0;
    for (uint32_t i = 0; i < bitmapInfoHeader.biWidth; ++i)
    {
        for (uint32_t j = 0; j < bitmapInfoHeader.biHeight; ++j)
        {
            VertexData vertex;
            vertex.uv[0] = (float)i;
            vertex.uv[1] = (float)j;

            //This will not work if the bytesPerPixel != 3 :/
            for (uint32_t k = 0; k < bytesPerPixel; ++k)
            {
                vertex.color[k] = rawData[index];
                index++;
            }
                
            outData.push_back(vertex);
        }
        index += widthLinePadding;
    }

    delete[] rawData;
    fclose(filePtr);

    return true;
}

bool ModelLoader::LoadOBJFile(const std::string& filepath, RawGeometryData& outData)
{
    
    outData.m_Positions.clear();
    outData.m_Normals    .clear();
    outData.m_UVs        .clear();
    outData.m_PosIndexes.clear();
    outData.m_NormIndexes.clear();
    outData.m_UVIndexes  .clear();

    FILE* file = fopen(filepath.c_str(), "r");
    if (file == nullptr)
        return false;

    while (true)
    {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break;

        if (strcmp(lineHeader, "v") == 0)
        {
            vec4 vert;
            fscanf(file, "%f %f %f\n", &vert.x, &vert.y, &vert.z);
            outData.m_Positions.push_back(vert);
        }
        else if (strcmp(lineHeader, "vt") == 0)
        {
            vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            outData.m_UVs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0)
        {
            vec4 norm;
            fscanf(file, "%f %f %f\n", &norm.x, &norm.y, &norm.z);
            outData.m_Normals.push_back(norm);
        }
        else if (strcmp(lineHeader, "f") == 0)
        {
            uint32_t vertexIndex[3], normalIndex[3];
            uint32_t uvIndex[3] = { 1, 1, 1 };
            
            s8 matches = 0;
            //Vertex Index / UV Index / Normal Index
            if (!outData.m_Positions.empty() && !outData.m_UVs.empty() && !outData.m_Normals.empty())
            {
                matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
                popAssert(matches == 9, "ModelLoader::LoadOBJFile - invalid read format for faces");
            }
            else if (!outData.m_Positions.empty() && outData.m_UVs.empty() && !outData.m_Normals.empty())
            {
                matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], /*&uvIndex[0],*/ &normalIndex[0], &vertexIndex[1], /*&uvIndex[1],*/ &normalIndex[1], &vertexIndex[2], /*&uvIndex[2],*/ &normalIndex[2]);
                popAssert(matches == 6, "ModelLoader::LoadOBJFile - invalid read format for faces");
            }
            else if (!outData.m_Positions.empty() && !outData.m_UVs.empty() && outData.m_Normals.empty())
            {
                matches = fscanf(file, "%d/%d %d/%d %d/%d\n", &vertexIndex[0], &uvIndex[0], &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2]);
                popAssert(matches == 6, "ModelLoader::LoadOBJFile - invalid read format for faces");
            }
            else
            {
                popAssert(false, "ModelLoader::LoadOBJFile - face format not supported");
            }

            outData.m_PosIndexes.push_back(vertexIndex[0] - 1);
            outData.m_PosIndexes.push_back(vertexIndex[1] - 1);
            outData.m_PosIndexes.push_back(vertexIndex[2] - 1);
            outData.m_UVIndexes.push_back(uvIndex[0] - 1);
            outData.m_UVIndexes.push_back(uvIndex[1] - 1);
            outData.m_UVIndexes.push_back(uvIndex[2] - 1);
            outData.m_NormIndexes.push_back(normalIndex[0] - 1);
            outData.m_NormIndexes.push_back(normalIndex[1] - 1);
            outData.m_NormIndexes.push_back(normalIndex[2] - 1);
        }
    }

    return true;
}