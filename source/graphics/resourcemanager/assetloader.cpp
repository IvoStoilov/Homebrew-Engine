#include <graphics/precompile.h>
#include <graphics/resourcemanager/assetloader.h>

#include <graphics/common/gfxprimitives/gfxvertex/gfxvertexlayout.h>
#include <graphics/common/mesh.h>
#include <graphics/common/material.h>

#include <extern/assimp/assimp-repo/include/assimp/Importer.hpp>
#include <extern/assimp/assimp-repo/include/assimp/scene.h>
#include <extern/assimp/assimp-repo/include/assimp/postprocess.h>

void AssetLoader::LoadObjFile(const String& objPath, const GfxVertexLayout& vertexLayout, Array<Mesh>& outMeshes, Array<Material>& outMaterials)
{
    Assimp::Importer imp;
    constexpr u32 READ_FLAGS =
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ConvertToLeftHanded |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace;

    UniquePtr<const aiScene> scene(imp.ReadFile(objPath.c_str(), READ_FLAGS));
    popAssert(scene, "Failed to read scene from {}", objPath.c_str());

    for (u32 i = 0; i < scene->mNumMeshes; ++i)
    {
        if (const aiMesh* mesh = scene->mMeshes[i])
        {
            outMeshes.emplace_back(vertexLayout, mesh);
        }
    }

    for (u32 i = 0; i < scene->mNumMaterials; ++i)
    {
        if (const aiMaterial* material = scene->mMaterials[i])
        {
            outMaterials.emplace_back(material);
        }
    }
}
