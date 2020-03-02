#pragma once
#include <engine/entitymodel/components/component.h>

class VisualComponent : public Component
{
public:
    VisualComponent();
    ~VisualComponent();

    inline void SetModelPath(const std::string& value) { m_ModelPath = value; }
    inline std::string GetModelPath() const { return m_ModelPath; }
    inline void SetTexturePath(const std::string& value) { m_TexturePath = value; }
    inline std::string GetTexturePath() const { return m_TexturePath; }
    inline void SetVertexShaderPath(const std::string& value) { m_VertexShaderPath = value; }
    inline std::string GetVertexShaderPath() const { return m_VertexShaderPath; }
    inline void SetPixelShaderPath(const std::string& value) { m_PixelShaderPath = value; }
    inline std::string GetPixelShaderPath() const { return m_PixelShaderPath; }

    inline void SetIs2D(bool value) { m_Is2D = value; }
    inline bool GetIs2D() const { return m_Is2D; }
    

private:
    std::string m_ModelPath;
    std::string m_TexturePath;
    std::string m_VertexShaderPath;
    std::string m_PixelShaderPath;

    bool m_Is2D;
};