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
    uint32_t rawDataSize = bitmapInfoHeader.biWidth * bitmapInfoHeader.biHeight * bytesPerPixel;
    outBMPwidth  = bitmapInfoHeader.biWidth;
    outBMPheight = bitmapInfoHeader.biHeight;

    unsigned char* rawData = new unsigned char[rawDataSize];

    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

    count = fread(rawData, 1, rawDataSize, filePtr);
    if (count != rawDataSize)
        return false;

    uint32_t index = 0;
    for (uint32_t i = 0; i < bitmapInfoHeader.biHeight; ++i)
    {
        for (uint32_t j = 0; j < bitmapInfoHeader.biWidth; ++j)
        {
            VertexData vertex;
            vertex.uv[0] = (float)i;
            vertex.uv[1] = (float)j;
            for (uint32_t k = 0; k < bytesPerPixel; ++k)
            {
                if (rawData[index] == '\0')
                    index++;

                vertex.color[k] = rawData[index];
                index++;
            }
                
            outData.push_back(vertex);
        }
    }

    delete[] rawData;
    fclose(filePtr);

    return true;
}