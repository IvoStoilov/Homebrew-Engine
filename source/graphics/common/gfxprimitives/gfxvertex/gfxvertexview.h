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

template<GfxVertexPropertyType Type>
auto& VertexView::GetProperty()
{
    const GfxVertexPropertyData* propertyData = m_Layout.GetPropertyDataByType(Type);
    popAssert(propertyData, "Did not find property data for {}", GetEnumName(Type));

    using DestType = typename GfxVertexPropertyData::PropertyMap<Type>::SysType;
    Byte* property = m_VertexBegin + propertyData->GetOffset();
    return *reinterpret_cast<DestType*>(property);
}

template<GfxVertexPropertyType Type, class ValType>
void VertexView::SetProperty(ValType&& val)
{
    if (const GfxVertexPropertyData* vertexProperty = m_Layout.GetPropertyDataByType(Type))
    {
        Byte* propertyBegin = m_VertexBegin + vertexProperty->GetOffset();
        AssignValue<Type>(propertyBegin, val);
    }
}

template<typename T>
void VertexView::SetPropertyByIndex(u32 i, T&& val)
{
    if (const GfxVertexPropertyData* property = m_Layout.GetPropertyDataByIndex(i))
    {
        GfxVertexPropertyType type = property->GetPropertyType();
        Byte* propertyBegin = m_VertexBegin + property->GetOffset();
        GfxVertexPropertyData::AssignProperty(type, propertyBegin, std::forward<T>(val));
    }
}

template<typename ...Args>
void VertexView::SetAllProperties(Args&&... args)
{
    SetPropertyByIndex(0u, std::forward<Args>(args)...);
}

template <GfxVertexPropertyType DestLayoutType, typename SrcType>
void VertexView::AssignValue(Byte* propertyBegin, SrcType&& val)
{
    using DestType = typename GfxVertexPropertyData::PropertyMap<DestLayoutType>::SysType;
    static_assert(std::is_assignable<DestType, SrcType>::value);
    *reinterpret_cast<DestType*>(propertyBegin) = val;
}

