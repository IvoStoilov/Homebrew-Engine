#pragma once
#include <graphics/common/gfxprimitives/gfxvertexlayout.h>

class VertexView
{
public:
    template<GfxVertexPropertyType Type>
    auto& GetProperty();

    template<GfxVertexPropertyType Type, class T>
    void SetProperty(T&& val);

    template<typename T>
    void SetPropetyByIndex(u32 i, T&& val);

    bool operator== (const VertexView& other) const;
protected:
    VertexView(byte* data, const GfxVertexLayout& layout);

    template <GfxVertexPropertyType DestLayoutType, typename SrcType>
    void AssignValue(byte* propertyBegin, SrcType&& val);

    template<typename First, typename ...Rest>
    void SetPropetyByIndex(u32 i, First&& first, Rest&&... rest)
    {
        SetPropetyByIndex(i, std::forward<First>(first));
        SetPropetyByIndex(i + 1, std::forward<Rest>(rest)...);
    }
private:
    byte* m_VertexBegin = nullptr;
    const GfxVertexLayout& m_Layout;

    friend class GfxVertexArray;
};

template<GfxVertexPropertyType Type>
auto& VertexView::GetProperty()
{
    byte* property = m_VertexBegin + m_Layout.GetVertexPropety<Type>().GetOffset();
    return *reinterpret_cast<GfxVertexLayout::PropertyMap<Type>::SysType>(property);
}

template<GfxVertexPropertyType Type, class T>
void VertexView::SetProperty(T&& val)
{
    GfxVertexProperty& vertexPropety = m_Layout.GetVertexPropety<Type>();
    byte* propertyBegin = m_VertexBegin + vertexPropety.GetOffset();
    AssignValue<Type>(propertyBegin, val);
}

template <GfxVertexPropertyType DestLayoutType, typename SrcType>
void VertexView::AssignValue(byte* propertyBegin, SrcType&& val)
{
    using Dest = typename GfxVertexLayout::PropertyMap<DestLayoutType>::SysType;
    static_assert(std::is_assignable(Dest, SrcType));
    *reinterpret_cast<Dest*>(propertyBegin) = val;
}

template<typename T>
void VertexView::SetPropetyByIndex(u32 i, T&& val)
{
    const GfxVertexProperty& property = m_Layout.GetVertexPropetyByIndex(i);
    GfxVertexPropertyType type = property.GetPropertyType();
    SetProperty<type>(std::forward<T>(val));
}
class GfxVertexArray
{
public:
    GfxVertexArray(const GfxVertexLayout& vertexLayout, u32 size = 0);

    const byte* GetData() const { return m_Buffer.data(); }
    const GfxVertexLayout& GetVertexLayout() const { return m_Layout; }
    u32 GetNumOfElements() const;
    u32 GetSizeInBytes() const;

    VertexView operator[](u32 index);

    template<typename... Params>
    void EmplaceBack(Params&&... params);
    VertexView Front();
    VertexView Back();

    bool IsEmpty() const;
private:
    Array<byte> m_Buffer;
    GfxVertexLayout m_Layout;
};

template<typename... Params>
void GfxVertexArray::EmplaceBack(Params&&... params)
{
    popAssert(sizeof...(params) == m_Layout.GetNumOfPropeties(), "Param count doesn't match number of vertex elements. Got {}, expected {}", sizeof...(params), m_Layout.GetNumOfPropeties());
    m_Buffer.resize(m_Buffer.size() + m_Layout.GetVertexSizeInBytes());
    Back().SetAllProperties(std::forward<Params>(params)...);
}

