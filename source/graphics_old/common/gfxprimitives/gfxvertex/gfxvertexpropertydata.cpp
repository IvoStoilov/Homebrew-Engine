#include <graphics/precompile.h>
#include <graphics/common/gfxprimitives/gfxvertex/gfxvertexpropertydata.h>
#include <graphics/common/gfxprimitives/gfxvertex/gfxvertexlayout.h>

constexpr char* GetEnumName(GfxVertexPropertyType type)
{
#define X(value) case GfxVertexPropertyType::value: return #value;
        switch (type)
        {
            POP_ALL_VERTEX_PROPERTY_TYPES
        }
        return "Unknown";
#undef X
}

GfxVertexPropertyData::GfxVertexPropertyData(GfxVertexPropertyType type, u32 offset)
    : m_Type(type)
    , m_Offset(offset)
{}

u32 GfxVertexPropertyData::GetOffsetAfter() const
{
    return m_Offset + GetPropertySize();
}

u32 GfxVertexPropertyData::GetPropertySize() const
{
    return GfxVertexPropertyData::GetSizeOf(m_Type);
}

constexpr u32 GfxVertexPropertyData::GetSizeOf(GfxVertexPropertyType type)
{
    return GfxVertexPropertyData::Bridge<SysSizeLookup>(type);
}

constexpr const char* GfxVertexPropertyData::GetPropertyShortName(GfxVertexPropertyType type)
{
    return GfxVertexPropertyData::Bridge<ShortNameLookup>(type);
}
