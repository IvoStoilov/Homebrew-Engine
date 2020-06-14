#pragma once
#include <extern/assimp/assimp-repo/include/assimp/mesh.h>

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

#define POP_DEFINE_AIMESH_EXTRACTOR(aMeshMember) \
    static SysType Extract(const aiMesh& mesh, u32 i) noexcept {return *reinterpret_cast<const SysType*>(&mesh.aMeshMember[i]);}

#define POP_DEFINE_VERTEX_PROPERTY(EnumValue, SystemType, DxgiFormat, Semantic, ShortName, aiMeshMember) \
    template<> struct PropertyMap<GfxVertexPropertyType::EnumValue>                                      \
    {                                                                                                    \
        using SysType = SystemType;                                                                      \
        static constexpr DXGI_FORMAT ms_DxgiFormat = DxgiFormat;                                         \
        static constexpr const char* ms_Semantic = Semantic;                                             \
        static constexpr const char* ms_ShortName = ShortName;                                           \
        POP_DEFINE_AIMESH_EXTRACTOR(aiMeshMember)                                                        \
    };

class GfxVertexPropertyData
{
public:
    template <GfxVertexPropertyType> struct PropertyMap;
    POP_DEFINE_VERTEX_PROPERTY(Position2D, vec2, DXGI_FORMAT_R32G32_FLOAT,       "Position",  "P2"   , mVertices);
    POP_DEFINE_VERTEX_PROPERTY(Position3D, vec3, DXGI_FORMAT_R32G32B32_FLOAT,    "Position",  "P3"   , mVertices);
    POP_DEFINE_VERTEX_PROPERTY(Position4D, vec4, DXGI_FORMAT_R32G32B32A32_FLOAT, "Position",  "P4"   , mVertices);
    POP_DEFINE_VERTEX_PROPERTY(Texture2D,  vec2, DXGI_FORMAT_R32G32_FLOAT,       "Texcoord",  "UV2"  , mTextureCoords[0]);
    POP_DEFINE_VERTEX_PROPERTY(Normal,     vec3, DXGI_FORMAT_R32G32B32_FLOAT,    "Normal",    "N3"   , mNormals);
    POP_DEFINE_VERTEX_PROPERTY(Tagent,     vec3, DXGI_FORMAT_R32G32B32_FLOAT,    "Tangent",   "T3"   , mTangents);
    POP_DEFINE_VERTEX_PROPERTY(Bitagent,   vec3, DXGI_FORMAT_R32G32B32_FLOAT,    "Bitangent", "B3"   , mBitangents);
    POP_DEFINE_VERTEX_PROPERTY(Color4,     vec4, DXGI_FORMAT_R32G32B32A32_FLOAT, "Color",     "C4"   , mColors[0]);
    POP_DEFINE_VERTEX_PROPERTY(Count,      u8,   DXGI_FORMAT_R8_UINT,            "Count",     "Count", mColors[0]);
public:
    GfxVertexPropertyData(GfxVertexPropertyType type, u32 offset);

    GfxVertexPropertyType GetPropertyType() const { return m_Type; }
    u32 GetOffset() const { return m_Offset; }
    u32 GetOffsetAfter() const;
    u32 GetPropertySize() const;

private:
    GfxVertexPropertyType m_Type;
    u32 m_Offset;
};

class GfxVertexProprtyHelper
{
public:
    template <typename ValType>
    static constexpr void AssignProperty(GfxVertexPropertyType type, Byte* destination, ValType&& val);

    template <typename ContainerType>
    static constexpr void ExtractPropertyValueFromContainer(GfxVertexPropertyType type, Byte* destination, const ContainerType& externMesh, u32 vertexIndex);

    static constexpr u32 GetSizeOf(GfxVertexPropertyType type);

    static constexpr const char* GetPropertyShortName(GfxVertexPropertyType type);

private:
    template<template<GfxVertexPropertyType> class F, typename... Args>
    static constexpr auto Bridge(GfxVertexPropertyType type, Args&&... args);
};
#include <graphics/common/gfxprimitives/gfxvertex/gfxvertexpropertydata.hpp>