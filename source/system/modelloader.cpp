#include "system/modelloader.h"
#include "system/error.h"
#include <fstream>
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