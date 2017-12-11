#pragma once
#include <string>

struct ID3D11Device;
struct ID3D11ShaderResourceView;
class Texture
{
public:
    Texture();
    ~Texture();

    bool Initialize(ID3D11Device* device, const std::string& texturePath);
    void Shutdown();

    inline ID3D11ShaderResourceView* GetTexture() { return m_Texture; }

private:
    ID3D11ShaderResourceView* m_Texture;

};