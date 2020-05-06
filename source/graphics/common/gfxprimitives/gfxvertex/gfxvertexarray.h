#pragma once
#include <graphics/common/gfxprimitives/gfxvertex/gfxvertexview.h>
#include <graphics/common/gfxprimitives/gfxvertex/gfxvertexlayout.h>

class GfxVertexArray
{
public:
    GfxVertexArray(const GfxVertexLayout& vertexLayout, u32 size = 0);

    const Byte* GetData() const { return m_Buffer.data(); }
    const GfxVertexLayout& GetVertexLayout() const { return m_Layout; }

    u32 GetNumOfElements() const;
    u32 GetSizeInBytes() const;

    VertexView operator[](u32 index);
    VertexView Front();
    VertexView Back();

    template<typename... Params>
    void EmplaceBack(Params&&... params);
    
    bool IsEmpty() const;
private:
    Array<Byte> m_Buffer;
    GfxVertexLayout m_Layout;
};

template<typename... Params>
void GfxVertexArray::EmplaceBack(Params&&... params)
{
    popAssert(sizeof...(params) == m_Layout.GetNumOfPropeties(), "Param count doesn't match number of vertex elements. Got {}, expected {}", sizeof...(params), m_Layout.GetNumOfPropeties());
    m_Buffer.resize(m_Buffer.size() + m_Layout.GetVertexSizeInBytes());
    Back().SetAllProperties(std::forward<Params>(params)...);
}
