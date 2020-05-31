#pragma once
#include <engine/entitymodel/components/component.h>

class VisualComponent : public Component
{
public:
    virtual void OnAddToWorld() override;
    virtual void OnRemoveFromWorld() override;

    inline void SetModelPath(const std::string& value) { m_ModelPath = value; }
    inline String GetModelPath() const { return m_ModelPath; }
    inline void SetTexturePath(const std::string& value) { m_TexturePath = value; }
    inline String GetTexturePath() const { return m_TexturePath; }
    inline void SetVertexShaderPath(const std::string& value) { m_VertexShaderPath = value; }
    inline String GetVertexShaderPath() const { return m_VertexShaderPath; }
    inline void SetPixelShaderPath(const std::string& value) { m_PixelShaderPath = value; }
    inline String GetPixelShaderPath() const { return m_PixelShaderPath; }

private:
    String m_ModelPath;
    String m_TexturePath;
    String m_VertexShaderPath;
    String m_PixelShaderPath;
}; 