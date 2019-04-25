#pragma once
#include "renderer/isubrenderer.h"
#include "system/math/vec4.h"
#include "system/math/mat4x4.h"
#include <vector>

struct ID3D11Device;
class ColorShader;
class DebugDisplayRenderer : public ISubRenderer
{
public:
    struct DebugDisplayElement
    {
        vec4 m_Start;
        vec4 m_End;
        vec4 m_Color;
    };

    struct VertexType
    {
        vec4 m_Position;
        vec4 m_Color;
    };
public:
    virtual bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override;
    virtual bool Render(D3D11* d3d) override;
    virtual void Shutdown() override;
    
    void AddLine(const vec4& start, const vec4& end, const vec4 color);

protected:
    void InitializeBuffers(ID3D11Device* device);
    void RenderGeometry(ID3D11DeviceContext* deviceContext);

private:
    std::vector<DebugDisplayElement> m_Elements;

    ID3D11Buffer* m_VertexBuffer;
    ID3D11Buffer* m_IndexBuffer;

    ColorShader* m_Shader;
};