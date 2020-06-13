#pragma once
struct aiMesh;
class Mesh
{
public:
    Mesh(const String& objPath);
private:
    Array<aiMesh*> m_Meshes;
};