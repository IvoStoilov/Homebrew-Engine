#include <graphics/precompile.h>
#include <graphics/common/gfxprimitives/gfxvertex/gfxvertexarray.h>

GfxVertexArray::GfxVertexArray(const GfxVertexLayout& vertexLayout, u32 numOfElements)
    : m_Layout(vertexLayout)
{
    m_Buffer.resize(numOfElements * m_Layout.GetVertexSizeInBytes());
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

bool GfxVertexArray::IsEmpty() const
{
    return m_Buffer.empty();
}
