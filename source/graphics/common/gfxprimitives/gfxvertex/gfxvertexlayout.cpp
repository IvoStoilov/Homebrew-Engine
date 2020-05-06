#include <graphics/precompile.h>
#include <graphics/common/gfxprimitives/gfxvertex/gfxvertexlayout.h>

void GfxVertexLayout::Append(GfxVertexPropertyType propertyType)
{
    if (!Has(propertyType))
    {
        const u32 offset = GetVertexSizeInBytes();
        m_VertexPropertyData.emplace_back(propertyType, offset);
    }
}

const GfxVertexPropertyData* GfxVertexLayout::GetPropertyDataByType(GfxVertexPropertyType type) const
{
    for (const GfxVertexPropertyData& p : m_VertexPropertyData)
    {
        if (p.GetPropertyType() == type)
            return &p;
    }

    return nullptr;
}

const GfxVertexPropertyData* GfxVertexLayout::GetPropertyDataByIndex(u32 index) const
{
    if(index < m_VertexPropertyData.size())
        return &m_VertexPropertyData[index];

    return nullptr;
}

u32 GfxVertexLayout::GetVertexSizeInBytes() const
{
    u32 result = 0u;
    for (const GfxVertexPropertyData& p : m_VertexPropertyData)
    {
        result += p.GetPropertySize();
    }
    return result;
}

u32 GfxVertexLayout::GetNumOfPropeties() const
{
    return static_cast<u32>(m_VertexPropertyData.size());
}

bool GfxVertexLayout::Has(GfxVertexPropertyType propertyType) const
{
    for (const GfxVertexPropertyData& p : m_VertexPropertyData)
    {
        if (p.GetPropertyType() == propertyType)
            return true;
    }

    return false;
}
