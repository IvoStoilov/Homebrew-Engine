#pragma once
#include <graphics/isubrenderer.h>
#include <graphics/textrendering/text.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
class D3D11;
class TextRenderer : public ISubRenderer
{
public:
    TextRenderer(const uint32_t screenWidth, const uint32_t screenHeight);
    virtual bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override;
    virtual void Shutdown()   override;

    virtual bool Render(D3D11* d3d) override;

private:
    std::vector<Text*> m_2DTexts;
    uint32_t m_ScreenWidth;
    uint32_t m_ScreenHeight;
};