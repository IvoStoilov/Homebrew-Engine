#include <graphics/precompile.h>
#include <graphics/textrendering/textrenderer.h>
#include <graphics/d3d11.h>
#include <system/error.h>


bool TextRenderer::Render(D3D11* d3d)
{
    d3d->TurnDepthTestOff();

    char fpsText[40]         = "Lorem Ispum";
    char cpuText[40]         = "Lorem Ispum";
    char frameTimeText[40]   = "Lorem Ispum";
    char cameraPosText[50]   = "Lorem Ispum";
    char cameraMovespeed[40] = "Lorem Ispum";
    //std::sprintf(fpsText, "FPS : %d", g_Engine->GetFPS());
    //std::sprintf(cpuText, "CPU: %d%%", g_Engine->GetCPUUsage());
    //std::sprintf(frameTimeText, "Engine: %.1f ms", g_Engine->GetFrameTimeInMS());
    //vec4 cameraPos = g_Engine->GetCamera()->GetPosition();
    //std::sprintf(cameraPosText, "Cam pos: %.1f, %.1f, %.1f", cameraPos.x, cameraPos.y, cameraPos.z);
    //std::sprintf(cameraMovespeed, "Cam mov speed %.1f m/s", g_Engine->GetCamera()->GetMoveSpeed());

    m_2DTexts[0]->SetText(fpsText, 0);
    m_2DTexts[0]->SetText(cpuText, 1);
    m_2DTexts[0]->SetText(frameTimeText, 2);
    m_2DTexts[0]->SetText(cameraPosText, 3);
    m_2DTexts[0]->SetText(cameraMovespeed, 4);

    const mat4x4& orthoMatrix = d3d->GetOrthoMatrix();
    mat4x4 worldMatrix;
    mat4x4 id;
    worldMatrix._43 += 5.f;
    
    {
        d3d->TurnAlphaBlendingOn();

        for(Text* text : m_2DTexts)
            text->Render(d3d->GetDeviceContext(), worldMatrix, orthoMatrix);

        d3d->TurnAlphaBlendingOff();
    }

    d3d->TurnDepthTestOn();

    return true;
}

bool TextRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    Text* text = new Text();
    popAssert(text->Initialize(device, deviceContext, m_ScreenWidth, m_ScreenHeight, 10, 700), "Text Init failed");

    text->AppendLine("Lorem Ipsum", 60, 255, 0, 0);
    text->AppendLine("Lorem Ipsum", 60, 255, 0, 0);
    text->AppendLine("Lorem Ipsum", 60, 255, 0, 0);
    text->AppendLine("Lorem Ipsum", 60, 255, 0, 0);
    text->AppendLine("Lorem Ipsum", 60, 255, 0, 0);

    m_2DTexts.push_back(text);

    return true;
}

void TextRenderer::Shutdown()
{
    for (Text* text : m_2DTexts)
    {
        text->Shutdown();
        delete text;
    }

    m_2DTexts.clear();
}

TextRenderer::TextRenderer(const uint32_t screenWidth, const uint32_t screenHeight) :
    ISubRenderer(),
    m_ScreenWidth(screenWidth),
    m_ScreenHeight(screenHeight)
{}