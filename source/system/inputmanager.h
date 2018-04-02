#pragma once

#define DIRECT_INPUTVERSION 0x0800

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>
#include <stdint.h>

class InputManager
{
public:
    enum class Key
    {
        W, A, S, D, Q, E
    };

    enum class MouseButton
    {
        LEFT = 0,
        MIDDLE = 1,
        RIGHT =2
    };
private:
    static InputManager* s_Instance;
    InputManager();
    ~InputManager();

    bool Initialize(HINSTANCE hInstance, HWND hwnd, uint32_t width, uint32_t height, int32_t windowPosX, int32_t windowPosY);
    void Shutdown();

public:
    static InputManager* GetInstance();
    static void CreateInstance(HINSTANCE hInstance, HWND hwnd, uint32_t width, uint32_t height, int32_t windowPosX, int32_t windowPosY);
    static void CleanInstance();

    bool Update();

    bool IsEscapePressed();
    bool IsKeyPressed(InputManager::Key key);
    bool IsMouseButtonPressed(InputManager::MouseButton button);

    void GetMouseLocation(int32_t& outX, int32_t& outY);
    void GetMouseDelta(int32_t& outXdelta, int32_t& outYdelta);

private:
    bool ReadKeyboard();
    bool ReadMouse();

    void ProcessInput();

private:
    IDirectInput8* m_DirectInput;
    IDirectInputDevice8* m_Keyboard;
    IDirectInputDevice8* m_Mouse;

    char m_KeyboardState[256];
    DIMOUSESTATE m_MouseState;

    uint32_t m_Height;
    uint32_t m_Width;
    int32_t m_WindowPosX;
    int32_t m_WindowPosY;

    int32_t m_MouseX;
    int32_t m_MouseY;
};

#define g_InputManager InputManager::GetInstance()