#pragma once
#include <graphics/common/gfxprimitives/gfxvertex/gfxvertexarray.h>
class Mesh
{
public:
    Mesh(const String& objPath, const GfxVertexLayout& vertexLayout);
private:
    Array<GfxVertexArray> m_VertexArray;
};