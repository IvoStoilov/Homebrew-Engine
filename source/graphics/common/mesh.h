#pragma once
#include <graphics/common/gfxprimitives/gfxvertex/gfxvertexarray.h>
class Mesh
{
public:
    Mesh(const GfxVertexLayout& vertexLayout, const aiMesh& externMesh);

private:
    GfxVertexArray m_VertexArray;
    Array<u32> m_IndexArray;
};