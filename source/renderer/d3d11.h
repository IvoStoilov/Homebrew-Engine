#pragma once
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>

#include <directxmath.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

class D3D11
{
public:
    D3D11();
    ~D3D11();

    bool Initialize(uint32_t screenWidth, uint32_t screenHeight, HWND hwnd, bool vsync, bool fullscreen, float screenDepth, float screenNear);
    void Shutdown();

    void BeginScene(float r, float g, float b, float a);
    void EndScene();

    void TurnDepthTestOn();
    void TurnDepthTestOff();

    void TurnAlphaBlendingOn();
    void TurnAlphaBlendingOff();

    void SetBackBufferRenderTarget();

    inline ID3D11Device* GetDevice() { return m_Device; }
    inline ID3D11DeviceContext* GetDeviceContext() { return m_DeviceContext; }
    inline ID3D11DepthStencilView* GetDepthStencilView() { return m_DepthStencilView; }

    inline void GetProjectionMatrix(D3DXMATRIX& mat) { mat = m_ProjectionMatrix_DEPRECATED; }
    inline DirectX::XMMATRIX GetProjectionMatrix() const { return m_ProjectionMatrix; }
    inline void GetWorldMatrix(D3DXMATRIX& mat) { mat = m_WorldMatrix; }
    inline void GetOrthoMatrix(D3DXMATRIX& mat) { mat = m_OrthoMatrix_DEPRECATED; }
    inline DirectX::XMMATRIX GetOrthoMatrix() const { return m_OrthoMatrix; }


    void GetVideoCardInfo(char* outCardName, int& outMemorySize);

private:
    bool InitGraphicsCardProperties(uint32_t screenWidth, uint32_t screenHeight, uint32_t& outNumerator, uint32_t& outDenominator);
    bool InitDeviceAndSwapchain(uint32_t screenWidth, uint32_t screenHeight, HWND hwnd, bool fullscreen, uint32_t numerator, uint32_t denominator);
    bool InitDepthStencilView(uint32_t screenWidth, uint32_t screenHeight);
    bool InitDepthStencilState();
    bool InitDepthDisabledStencilState();
    bool InitRasterizerState();
    bool InitAlphaBlendingStates();
    void InitViewPort(uint32_t screenWidth, uint32_t screenHeight);
    void InitMatrices(uint32_t screenWidth, uint32_t screenHeight, float screenNear, float screenDepth);

private:
    bool m_vsync_enabled;
    int m_VideoCardMemory;
    char m_VideoCardDescription[128];
    IDXGISwapChain* m_SwapChain;
    ID3D11Device* m_Device;
    ID3D11DeviceContext* m_DeviceContext;
    ID3D11RenderTargetView* m_RenderTargetView;
    ID3D11Texture2D* m_DepthStencilBuffer;
    ID3D11DepthStencilState* m_DepthStencilState;
    ID3D11DepthStencilState* m_DepthDisabledStencilState;
    ID3D11DepthStencilView* m_DepthStencilView;
    ID3D11RasterizerState* m_RasterState;
    ID3D11BlendState* m_AlphaBlendingStateEnable;
    ID3D11BlendState* m_AlphaBlendingStateDisable;
    //clean this
    D3DXMATRIX m_ProjectionMatrix_DEPRECATED;
    DirectX::XMMATRIX m_ProjectionMatrix;
    D3DXMATRIX m_WorldMatrix;
    D3DXMATRIX m_OrthoMatrix_DEPRECATED;
    DirectX::XMMATRIX m_OrthoMatrix;
};