#pragma once
class WindowHandle
{
public:
    virtual void ReleaseHandle() = 0;
};

class ViewProvider
{
public:
    struct WindowResolution
    {
        WindowResolution() = default;
        explicit WindowResolution(s32 w, s32 h) : m_Width(w), m_Height(h) {}
        
        s32 m_Width = 0;
        s32 m_Height = 0;
    };

public:
    static ViewProvider& GetInstance();
    static void CreateInstance();
    static void CleanInstnace();
public:
    ViewProvider();
    virtual ~ViewProvider();

    bool Initialize();
    void Shutdown();

    virtual bool InitializeInternal() = 0;
    virtual void ShutdownInternal() = 0;
    virtual void Update() = 0;

    inline WindowHandle* GetWindowHandle() { return m_WindowHandle; }
    inline const WindowResolution& GetWindowResolution() const { return m_WindowResolution; }
    inline u32 GetWindowWidth() const { return m_WindowResolution.m_Width; }
    inline u32 GetWindowHeight() const { return m_WindowResolution.m_Height; }
    inline u32 GetWindowPosX() const { return m_WindowPosX; }
    inline u32 GetWindowPosY() const { return m_WindowPosY; }

    virtual void ActivateFullscreen() = 0;
    virtual void DeactivateFullscreen() = 0;

protected:
    virtual WindowResolution GetMonitorResolution() = 0;

    void InitializeWindowHandle();
    void SetDefaultPositionAndResolution();

protected:
    String m_ApplicationName;
    WindowHandle* m_WindowHandle = nullptr;

    u32 m_WindowPosX = 0;
    u32 m_WindowPosY = 0;

    WindowResolution m_WindowResolution;

private:
    static ViewProvider* sm_ViewProviderInstance;
};