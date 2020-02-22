#pragma once

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>
#include <stdint.h>

class ViewProvider;
class InputManager
{
public:
    enum class Key
    {
        W, A, S, D, Q, E, F1
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

    bool Initialize(ViewProvider& viewProvider);
    void Shutdown();

public:
    static InputManager* GetInstance();
    static void CreateInstance(ViewProvider& viewProvider);
    static void CleanInstance();

    bool Update();

    bool IsEscapePressed();
    bool IsKeyPressed(InputManager::Key key);
    bool IsKeyJustPressed(InputManager::Key key);
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
    char m_KeyBoardStatePrevFrame[256];

    DIMOUSESTATE m_MouseState;

    uint32_t m_Height = 0;
    uint32_t m_Width = 0;
    int32_t m_WindowPosX = 0;
    int32_t m_WindowPosY = 0;

    int32_t m_MouseX = 0;
    int32_t m_MouseY = 0;
};

#define g_InputManager InputManager::GetInstance()