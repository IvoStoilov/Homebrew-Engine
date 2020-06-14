#include <graphics/precompile.h>
#include <graphics/common/gfxprimitives/gfxvertex/gfxvertexarray.h>

#include <extern/assimp/assimp-repo/include/assimp/mesh.h>

GfxVertexArray::GfxVertexArray(const GfxVertexLayout& vertexLayout, u32 numOfElements)
    : m_Layout(vertexLayout)
{
    ResizeNumOfElements(numOfElements);
}

GfxVertexArray::GfxVertexArray(const GfxVertexLayout& vertexLayout, const aiMesh& externMesh)
    : m_Layout(vertexLayout)
{
    ResizeNumOfElements(externMesh.mNumVertices);
    for (u32 propertyIdx = 0; propertyIdx < m_Layout.GetNumOfPropeties(); ++propertyIdx)
    {
        for (u32 vertexIdx = 0; vertexIdx < GetNumOfElements(); ++vertexIdx)
        {
            (*this)[vertexIdx].SetPropertyByIndexFromContainer(propertyIdx, externMesh, vertexIdx);
        }
    }
}

u32 GfxVertexArray::GetNumOfElements() const
{
    return static_cast<u32>(m_Buffer.size()) / m_Layout.GetVertexSizeInBytes();
}

u32 GfxVertexArray::GetSizeInBytes() const
{
    return static_cast<u32>(m_Buffer.size());
}

VertexView GfxVertexArray::operator[](u32 index)
{
    const u32 offset = index * m_Layout.GetVertexSizeInBytes();
    Byte* vertexBegin = &m_Buffer[offset];
    return VertexView(vertexBegin, m_Layout);
}

VertexView GfxVertexArray::Front()
{
    return VertexView(m_Buffer.data(), m_Layout);
}

VertexView GfxVertexArray::Back()
{
    if (IsEmpty())
        return Front();

    Byte* lastVertexBegin = m_Buffer.data() + m_Buffer.size() - m_Layout.GetVertexSizeInBytes();
    popAssert(lastVertexBegin >= m_Buffer.data(), "Vertex Ptr points behind the Buffer Ptr. This should never happen");
    return VertexView(lastVertexBegin, m_Layout);
}

void GfxVertexArray::ResizeNumOfElements(u32 newSize)
{
    const u32 currentSize = GetNumOfElements();
    if (currentSize < newSize)
    {
        m_Buffer.resize(m_Layout.GetVertexSizeInBytes() * newSize);
    }
}

bool GfxVertexArray::IsEmpty() const
{
    return m_Buffer.empty();
}
