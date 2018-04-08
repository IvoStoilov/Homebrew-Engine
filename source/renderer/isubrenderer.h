#pragma once

struct ID3D11Device;
struct ID3D11DeviceContext;
class D3D11;
class ISubRenderer
{
public:
    ISubRenderer() : m_Active(true) {}

    virtual bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) = 0;
    virtual void Shutdown() = 0;
    virtual bool Render(D3D11* d3d) = 0;

    inline virtual bool IsActive() const { return m_Active;  }
    inline virtual void Activate()       { m_Active = true;  }
    inline virtual void Deactivate()     { m_Active = false; }

protected:
    bool m_Active;
};