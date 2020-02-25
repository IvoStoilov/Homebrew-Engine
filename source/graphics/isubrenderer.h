#pragma once
class ISubRenderer
{
public:
    ISubRenderer() : m_IsEnabled(true) {}

    virtual bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) = 0;
    virtual bool Render(D3D11* d3d) = 0;
    virtual void Shutdown() = 0;

    inline void UpdateViewMatrix(const mat4x4& viewMatrix) { m_ViewMatrix = viewMatrix; }
    inline void UpdateReflectionMatrix(const mat4x4& reflectionMatrix) { m_ReflectionMatrix = reflectionMatrix; }

    inline bool IsEnabled() const { return m_IsEnabled;  }
    inline virtual void SetIsEnabled(bool value) { m_IsEnabled = value; }

    //Sets a clip plane for shader Ax + By + Cz + D = 0;
    inline void SetClipPlane(const vec4& clipPlane) { m_ClipPlane = clipPlane; }

protected:
    mat4x4 m_ViewMatrix;
    mat4x4 m_ReflectionMatrix;
    bool m_IsEnabled;

    vec4 m_ClipPlane;
};