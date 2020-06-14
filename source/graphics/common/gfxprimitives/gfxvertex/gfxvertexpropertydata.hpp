#pragma once

template<GfxVertexPropertyType PropertyType>
struct SysSizeLookup
{
    static constexpr auto Exec()
    {
        return static_cast<u32>(sizeof(GfxVertexPropertyData::PropertyMap<PropertyType>::SysType));
    }
};
//========================================================================================================
template<GfxVertexPropertyType PropertyType>
struct AssignValue
{
    template<class SrcType>
    static constexpr auto Exec(Byte* dest, SrcType&& val)
    {
        using DestType = typename GfxVertexPropertyData::PropertyMap<PropertyType>::SysType;
        if constexpr (std::is_assignable<DestType, SrcType>::value)
        {
            *reinterpret_cast<DestType*>(dest) = val;
        }
        else
        {
            popAssert(false, "Param Attribute missmtch");
        }
    }
};
//========================================================================================================
template<GfxVertexPropertyType PropertyType>
struct ShortNameLookup
{
    static constexpr auto Exec()
    {
        return GfxVertexPropertyData::PropertyMap<PropertyType>::ms_ShortName;
    }
};

//========================================================================================================
template<GfxVertexPropertyType PropertyType>
struct AttributeAiMeshFill
{
    static constexpr void Exec(Byte* dest, const aiMesh& mesh, u32 vertexIdx)
    {
        using DestType = typename GfxVertexPropertyData::PropertyMap<PropertyType>::SysType;
        *reinterpret_cast<DestType*>(dest) = GfxVertexPropertyData::PropertyMap<PropertyType>::Extract(mesh, vertexIdx);
    }
};
//========================================================================================================
template<template<GfxVertexPropertyType> class F, typename... Args>
constexpr auto GfxVertexProprtyHelper::Bridge(GfxVertexPropertyType type, Args&&... args)
{
    switch (type)
    {
#define X(value) case GfxVertexPropertyType::value: return F<GfxVertexPropertyType::value>::Exec(std::forward<Args>(args)...);
        POP_ALL_VERTEX_PROPERTY_TYPES
#undef X
    }
    popAssert(false, "Invalid Property Type : {}", GetEnumName(type));
    return F<GfxVertexPropertyType::Count>::Exec(std::forward<Args>(args)...);
}

template <typename ValType>
constexpr void GfxVertexProprtyHelper::AssignProperty(GfxVertexPropertyType type, Byte* dest, ValType&& val)
{
    GfxVertexProprtyHelper::Bridge<AssignValue>(type, dest, std::forward<ValType>(val));
}

template <typename ContainerType>
constexpr void GfxVertexProprtyHelper::ExtractPropertyValueFromContainer(GfxVertexPropertyType type, Byte* destination, const ContainerType& externMesh, u32 vertexIndex)
{
    static_assert(std::is_same<ContainerType, aiMesh>::value, "Container Type not supported");
    GfxVertexProprtyHelper::Bridge<AttributeAiMeshFill>(type, destination, externMesh, vertexIndex);
}
