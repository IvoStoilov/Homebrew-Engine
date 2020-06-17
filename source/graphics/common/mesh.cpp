#include <graphics/precompile.h>
#include <graphics/common/mesh.h>

Mesh::Mesh(const GfxVertexLayout& vertexLayout, const aiMesh& externMesh)
    : m_VertexArray(vertexLayout, externMesh)
{
    m_IndexArray.reserve(externMesh.mNumFaces * 3u);
    for (u32 i = 0; i < externMesh.mNumFaces; ++i)
    {
        const aiFace& face = externMesh.mFaces[i];
        popAssert(face.mNumIndices == 3u, "Mesh is not triangulated");
        m_IndexArray.push_back(face.mIndices[0]);
        m_IndexArray.push_back(face.mIndices[1]);
        m_IndexArray.push_back(face.mIndices[2]);
    }
}