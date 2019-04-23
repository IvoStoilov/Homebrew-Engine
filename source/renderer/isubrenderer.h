#pragma once
#include "system/math/mat4x4.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
class D3D11;
class mat4x4;
class ISubRenderer
{
public:
    ISubRenderer() : m_IsEnabled(true) {}

    virtual bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) = 0;
    virtual bool Render(D3D11* d3d) = 0;
    virtual void Shutdown() = 0;

    inline void UpdateViewMatrix(const mat4x4& viewMatrix) { m_ViewMatrix = viewMatrix; }

    inline bool IsEnabled() const { return m_IsEnabled;  }
    inline virtual void SetIsEnabled(bool value) { m_IsEnabled = value; }

protected:
    mat4x4 m_ViewMatrix;
    bool m_IsEnabled;
};