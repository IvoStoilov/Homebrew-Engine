#pragma once
#include <graphics/common/gfxprimitives/gfxvertex/gfxvertexlayout.h>

class VertexView
{
public:
    template<GfxVertexPropertyType Type>
    auto& GetProperty();

    template<GfxVertexPropertyType Type, class ValType>
    void SetProperty(ValType&& val);

    template<typename ValType>
    void SetPropertyByIndex(u32 i, ValType&& val);

    template <typename ContainerType>
    void SetPropertyByIndexFromContainer(u32 i, const ContainerType& externMesh, u32 vertexIndex);

    template<typename ...Args>
    void SetAllProperties(Args&&... args);

    bool operator== (const VertexView& other) const;

protected:
    VertexView(Byte* data, const GfxVertexLayout& layout);

    template <GfxVertexPropertyType DestLayoutType, typename SrcType>
    void AssignValue(Byte* propertyBegin, SrcType&& val);

    template<typename First, typename ...Rest>
    void SetPropertyByIndex(u32 i, First&& first, Rest&&... rest)
    {
        SetPropertyByIndex(i, std::forward<First>(first));
        SetPropertyByIndex(i + 1, std::forward<Rest>(rest)...);
    }
private:
    Byte* m_VertexBegin = nullptr;
    const GfxVertexLayout& m_Layout;

    friend class GfxVertexArray;
};

#include <graphics/common/gfxprimitives/gfxvertex/gfxvertexview.hpp>

