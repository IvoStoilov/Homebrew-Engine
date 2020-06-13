#pragma once
#include <graphics/common/gfxprimitives/gfxvertex/gfxvertexpropertydata.h>

class GfxVertexLayout
{
public:
    void Append(GfxVertexPropertyType propertyType);

    const GfxVertexPropertyData* GetPropertyDataByType(GfxVertexPropertyType type) const;
    const GfxVertexPropertyData* GetPropertyDataByIndex(u32 index) const;

    u32 GetVertexSizeInBytes() const;
    u32 GetNumOfPropeties() const;

    bool Has(GfxVertexPropertyType propertyType) const;
private:
    Array<GfxVertexPropertyData> m_VertexPropertyData;
};