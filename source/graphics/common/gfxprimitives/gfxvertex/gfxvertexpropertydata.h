#pragma once
#define POP_ALL_VERTEX_PROPERTY_TYPES\
    X(Position2D) \
    X(Position3D) \
    X(Position4D) \
    X(Texture2D)  \
    X(Normal)     \
    X(Tagent)     \
    X(Bitagent)   \
    X(Color4)     \
    X(Count)

enum class GfxVertexPropertyType
{
#define X(value) value,
    POP_ALL_VERTEX_PROPERTY_TYPES
#undef X
};

constexpr char* GetEnumName(GfxVertexPropertyType type);

#define POP_DEFINE_VERTEX_PROPERTY(EnumValue, SystemType, DxgiFormat, Semantic) \
    template<> struct PropertyMap<GfxVertexPropertyType::EnumValue>             \
    {                                                                           \
        using SysType = SystemType;                                             \
        static constexpr DXGI_FORMAT dxgiFormat = DxgiFormat;                   \
        static constexpr const char* semantic = Semantic;                       \
    };

class GfxVertexPropertyData
{
public:
    template <GfxVertexPropertyType> struct PropertyMap;
    POP_DEFINE_VERTEX_PROPERTY(Position2D, vec2, DXGI_FORMAT_R32G32_FLOAT,       "Position");
    POP_DEFINE_VERTEX_PROPERTY(Position3D, vec3, DXGI_FORMAT_R32G32B32_FLOAT,    "Position");
    POP_DEFINE_VERTEX_PROPERTY(Position4D, vec4, DXGI_FORMAT_R32G32B32A32_FLOAT, "Position");
    POP_DEFINE_VERTEX_PROPERTY(Texture2D,  vec2, DXGI_FORMAT_R32G32_FLOAT,       "Texcoord");
    POP_DEFINE_VERTEX_PROPERTY(Normal,     vec3, DXGI_FORMAT_R32G32B32_FLOAT,    "Normal"  );
    POP_DEFINE_VERTEX_PROPERTY(Tagent,     vec3, DXGI_FORMAT_R32G32B32_FLOAT,    "Tangent" );
    POP_DEFINE_VERTEX_PROPERTY(Bitagent,   vec3, DXGI_FORMAT_R32G32B32_FLOAT,    "Bitangent");
    POP_DEFINE_VERTEX_PROPERTY(Color4,     vec4, DXGI_FORMAT_R32G32B32A32_FLOAT, "Color"   );
    POP_DEFINE_VERTEX_PROPERTY(Count,      u8,   DXGI_FORMAT_R8_UINT,            "Count"   );
public:
    GfxVertexPropertyData(GfxVertexPropertyType type, u32 offset);

    GfxVertexPropertyType GetPropertyType() const { return m_Type; }
    u32 GetOffset() const { return m_Offset; }
    u32 GetOffsetAfter() const;
    u32 GetPropertySize() const;

    template <typename ValType>
    static constexpr void AssignProperty(GfxVertexPropertyType type, Byte* destination, ValType&& val);
    static constexpr u32 GetSizeOf(GfxVertexPropertyType type);

private:
    template<template<GfxVertexPropertyType> class F, typename... Args>
    static constexpr auto Bridge(GfxVertexPropertyType type, Args&&... args);

private:
    GfxVertexPropertyType m_Type;
    u32 m_Offset;
};

template<GfxVertexPropertyType PropertyType>
struct SysSizeLookup
{
    static constexpr auto Exec()
    {
        return static_cast<u32>(sizeof(GfxVertexPropertyData::PropertyMap<PropertyType>::SysType));
    }
};

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

template<template<GfxVertexPropertyType> class F, typename... Args>
constexpr auto GfxVertexPropertyData::Bridge(GfxVertexPropertyType type, Args&&... args)
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
constexpr void GfxVertexPropertyData::AssignProperty(GfxVertexPropertyType type, Byte* dest, ValType&& val)
{
    GfxVertexPropertyData::Bridge<AssignValue>(type, dest, std::forward<ValType>(val));
}