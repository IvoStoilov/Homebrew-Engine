#pragma once
#include "system/math/mat4x4.h"


class Entity;
class D3D11Model;
class LightShader;
struct ID3D11Device;
struct ID3D11DeviceContext;
class GraphicsNode
{
public:
    GraphicsNode(Entity* owner);
    ~GraphicsNode();

    void Initialize(ID3D11Device* device);
    void Shutdown();

    inline void SetProjectionMatrix(const mat4x4& value) { m_ProjectionMatrix = value; }
    inline void SetViewMatrix(const mat4x4& value) { m_ViewMatrix = value; }


    void Render(ID3D11DeviceContext* deviceContext);

private:
    Entity* m_Owner;

    mat4x4 m_ProjectionMatrix;
    mat4x4 m_ViewMatrix;

    //TODO (istoilov) : Make an abstraction over shader implementations
    LightShader* m_LightShader;
    D3D11Model* m_Model;
};