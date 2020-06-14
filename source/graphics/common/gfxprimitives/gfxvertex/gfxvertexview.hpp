#pragma once
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
        GfxVertexProprtyHelper::AssignProperty(type, propertyBegin, std::forward<T>(val));
    }
}

template <typename ContainerType>
void VertexView::SetPropertyByIndexFromContainer(u32 i, const ContainerType& externMesh, u32 vertexIndex)
{
    if (const GfxVertexPropertyData* property = m_Layout.GetPropertyDataByIndex(i))
    {
        GfxVertexPropertyType type = property->GetPropertyType();
        Byte* propertyBegin = m_VertexBegin + property->GetOffset();
        GfxVertexProprtyHelper::ExtractPropertyValueFromContainer(type, propertyBegin, externMesh, vertexIndex);
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