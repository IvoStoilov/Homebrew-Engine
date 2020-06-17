#pragma once
class Mesh;
class Material;
class AssetLoader
{
    static void LoadObjFile(const String& objPath, const GfxVertexLayout& vertexLayout, Array<Mesh>& outMeshes, Array<Material>& outMaterials);
};