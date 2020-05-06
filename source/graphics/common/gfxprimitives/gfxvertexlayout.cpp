#include <graphics/precompile.h>
#include <graphics/common/gfxprimitives/gfxvertexlayout.h>

template<GfxVertexPropertyType PropertyType>
struct SysSizeLookup
{
    static constexpr auto Exec()
    {
        return static_cast<u32>(sizeof(GfxVertexLayout::PropertyMap<PropertyType>::SysType));
    }
};

GfxVertexProperty::GfxVertexProperty(GfxVertexPropertyType type, u32 offset)
    : m_Type(type)
    , m_Offset(offset)
{}

u32 GfxVertexProperty::GetOffsetAfter() const
{
    return m_Offset + GetPropertySize();
}

u32 GfxVertexProperty::GetPropertySize() const
{
    return GfxVertexProperty::GetSizeOf(m_Type);
}

constexpr u32 GfxVertexProperty::GetSizeOf(GfxVertexPropertyType type)
{
    return GfxVertexProperty::Bridge<SysSizeLookup>(type);
}

void GfxVertexLayout::Append(GfxVertexPropertyType propertyType)
{
    if (!Has(propertyType))
    {
        m_VertexProperties.emplace_back(propertyType, GetVertexSizeInBytes());
    }
}

const GfxVertexProperty& GfxVertexLayout::GetVertexPropetyByIndex(u32 index) const
{
    return m_VertexProperties[index];
}

u32 GfxVertexLayout::GetVertexSizeInBytes() const
{
    u32 result = 0u;
    for (const GfxVertexProperty& p : m_VertexProperties)
    {
        result += p.GetPropertySize();
    }
    return result;
}

u32 GfxVertexLayout::GetNumOfPropeties() const
{
    return static_cast<u32>(m_VertexProperties.size());
}

bool GfxVertexLayout::Has(GfxVertexPropertyType propertyType) const
{
    for (const GfxVertexProperty& p : m_VertexProperties)
    {
        if (p.GetPropertyType() == propertyType)
            return true;
    }

    return false;
}
