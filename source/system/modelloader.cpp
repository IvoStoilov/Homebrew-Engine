#include "system/modelloader.h"
#include <fstream.h>
#include <stdint.h>

using namespace std;

bool ModelLoader::LoadBGDFile(const std::string& filepath, std::vector<VertexData>& outData)
{
    outData.clear();
    uint32_t vertexCount = 0;
    ifstream reader;
    reader.open(filepath);
    popAssert(reader.fail(), "ModelLoader::LoadBGDFile failed to find path");


}