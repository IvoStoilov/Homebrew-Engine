#pragma once

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>
#include <system/singleton/singleton.h>

class ViewProvider;
class InputManager
{
    POP_DECLARE_SINGLETON(InputManager)
public:
    enum class Key
    {
        W, A, S, D, Q, E, F1
    };

    enum class MouseButton
    {
        LEFT = 0,
        MIDDLE = 1,
        RIGHT = 2
    };

    bool Update();

    bool IsEscapePressed();
    bool IsKeyPressed(InputManager::Key key);
    bool IsKeyJustPressed(InputManager::Key key);
    bool IsMouseButtonPressed(InputManager::MouseButton button);

    void GetMouseLocation(int32_t& outX, int32_t& outY);
    void GetMouseDelta(int32_t& outXdelta, int32_t& outYdelta);

private:
    InputManager();
    ~InputManager();

    bool Initialize(ViewProvider& viewProvider);
    void Shutdown();

    bool ReadKeyboard();
    bool ReadMouse();

    void ProcessInput();
private:
    IDirectInput8* m_DirectInput = nullptr;
    IDirectInputDevice8* m_Keyboard = nullptr;
    IDirectInputDevice8* m_Mouse = nullptr;

    char m_KeyboardState[256];
    char m_KeyBoardStatePrevFrame[256];

    DIMOUSESTATE m_MouseState;

    u32 m_Height = 0;
    u32 m_Width = 0;
    s32 m_WindowPosX = 0;
    s32 m_WindowPosY = 0;

    s32 m_MouseX = 0;
    s32 m_MouseY = 0;
};

#define g_InputManager InputManager::GetInstance()