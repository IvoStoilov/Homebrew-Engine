#include <graphics/precompile.h>
#include <graphics/common/gfxprimitives/gfxvertex/gfxvertexview.h>



VertexView::VertexView(Byte* data, const GfxVertexLayout& layout)
    : m_VertexBegin(data)
    , m_Layout(layout)
{}

bool VertexView::operator==(const VertexView& other) const
{
    return m_VertexBegin == other.m_VertexBegin;
}