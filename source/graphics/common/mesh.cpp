#include <graphics/precompile.h>
#include <graphics/common/mesh.h>

#include <extern/assimp/assimp-repo/include/assimp/Importer.hpp>
#include <extern/assimp/assimp-repo/include/assimp/scene.h>
#include <extern/assimp/assimp-repo/include/assimp/postprocess.h>

Mesh::Mesh(const String& objPath)
{
    Assimp::Importer imp;
    constexpr u32 READ_FLAGS = 
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ConvertToLeftHanded |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace;

    const aiScene* scene = imp.ReadFile(objPath.c_str(), READ_FLAGS);
    popAssert(scene, "Failed to read scene from {}", objPath.c_str());

    for (u32 i = 0; i < scene->mNumMeshes; ++i)
    {
        m_Meshes.push_back(scene->mMeshes[i]);
    }
}
