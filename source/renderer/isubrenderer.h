#pragma once

struct ID3D11Device;
struct ID3D11DeviceContext;
class D3D11;
class mat4x4;
class ISubRenderer
{
public:
    ISubRenderer() : m_Active(true) {}

    virtual bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) = 0;
    virtual bool Render(D3D11* d3d) = 0;
    virtual void Shutdown() = 0;

    virtual void UpdateViewMatrix(const mat4x4& viewMatrix) {}

    inline virtual bool IsActive() const { return m_Active;  }
    inline virtual void Activate()       { m_Active = true;  }
    inline virtual void Deactivate()     { m_Active = false; }

protected:
    bool m_Active;
};