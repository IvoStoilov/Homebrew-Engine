#pragma once

struct Resolution;
class D3D11
{
public:
    void InitializeDeviceAndStates();
    void InitializeViewPortAndMatrices(const Resolution& renderingResolution, float screenDepth, float screenNear);
    void Shutdown();

    void TurnDepthTestOn();
    void TurnDepthTestOff();

    void TurnAlphaBlendingOn();
    void TurnAlphaBlendingOff();

    inline ID3D11Device* GetDevice() { return m_Device; }
    inline ID3D11DeviceContext* GetDeviceContext() { return m_DeviceContext; }
    
    inline const mat4x4& GetPerspectiveMatrix() const { return m_PerspectiveMatrix; }
    inline void GetPerspectiveMatrix(mat4x4& outPerspectiveMatrix) const { outPerspectiveMatrix = m_PerspectiveMatrix; }
    inline const mat4x4& GetOrthoMatrix() const { return m_OrthoMatrix; }

    void GetVideoCardInfo(char* outCardName, int& outMemorySize);

private:
    void InitDeviceAndContext();
    void InitDepthStencilState();
    void InitDepthDisabledStencilState();
    void InitRasterizerState();
    void InitAlphaBlendingStates();

    void InitGraphicsCardProperties(u32 screenWidth, u32 screenHeight, u32& outNumerator, u32& outDenominator);
    void InitViewPort(u32 screenWidth, u32 screenHeight);
    void InitMatrices(u32 screenWidth, u32 screenHeight, f32 screenNear, f32 screenDepth);

private:
    s32 m_VideoCardMemory = 0;
    char m_VideoCardDescription[128];
    ID3D11Device* m_Device = nullptr;
    ID3D11DeviceContext* m_DeviceContext = nullptr;
    ID3D11DepthStencilState* m_DepthStencilState = nullptr;
    ID3D11DepthStencilState* m_DepthDisabledStencilState = nullptr;
    ID3D11RasterizerState* m_RasterState = nullptr;
    ID3D11BlendState* m_AlphaBlendingStateEnable = nullptr;
    ID3D11BlendState* m_AlphaBlendingStateDisable = nullptr;

    mat4x4 m_PerspectiveMatrix;
    mat4x4 m_OrthoMatrix;
};