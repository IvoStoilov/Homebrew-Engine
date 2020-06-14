#include <graphics/precompile.h>
#include <graphics/common/mesh.h>

#include <extern/assimp/assimp-repo/include/assimp/Importer.hpp>
#include <extern/assimp/assimp-repo/include/assimp/scene.h>
#include <extern/assimp/assimp-repo/include/assimp/postprocess.h>

Mesh::Mesh(const String& objPath, const GfxVertexLayout& vertexLayout)
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
            m_VertexArray.emplace_back(vertexLayout, *mesh);
        }

    }
    vec3 pos = (m_VertexArray[0])[46].GetProperty<GfxVertexPropertyType::Position3D>();
    const aiVector3D& other = scene->mMeshes[0]->mVertices[46];

}