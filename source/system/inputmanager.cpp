#include <system/precompile.h>
#include <system/inputmanager.h>
#include <system/viewprovider/viewprovider.h>
#include <system/error.h>

#define DIRECT_INPUTVERSION 0x0800

InputManager::InputManager()
{
    memset(&m_MouseState, 0, sizeof(m_MouseState));
    memset(m_KeyboardState, 0, sizeof(m_KeyboardState));
    memset(m_KeyBoardStatePrevFrame, 0, sizeof(m_KeyBoardStatePrevFrame));
    Initialize(g_ViewProvider);
}


InputManager::~InputManager()
{
    Shutdown();
}

bool InputManager::Initialize(ViewProvider& viewProvider)
{
    m_Width = viewProvider.GetWindowWidth();
    m_Height = viewProvider.GetWindowHeight();

    m_WindowPosX = viewProvider.GetWindowPosX();
    m_WindowPosY = viewProvider.GetWindowPosY();

    m_MouseX = 0;
    m_MouseY = 0;

#ifdef POP_PLATFORM_WINDOWS
    HRESULT result;
    result = DirectInput8Create(viewProvider.GetHInstnace(), DIRECT_INPUTVERSION, IID_IDirectInput8, (void**)& m_DirectInput, nullptr);
    popAssert(!FAILED(result), "InputManager::Initialize Failed.");

    //Keyboard setup
    result = m_DirectInput->CreateDevice(GUID_SysKeyboard, &m_Keyboard, nullptr);
    popAssert(!FAILED(result), "InputManager::CreateDevice Failed.");

    result = m_Keyboard->SetDataFormat(&c_dfDIKeyboard);
    popAssert(!FAILED(result), "InputManager::SetDataFormat Failed.");

    result = m_Keyboard->SetCooperativeLevel(viewProvider.GetWindowHandle(), DISCL_FOREGROUND | DISCL_EXCLUSIVE);
    popAssert(!FAILED(result), "InputManager::SetCooperativeLevel Failed.");

    result = m_Keyboard->Acquire();
    popAssert(!FAILED(result), "InputManager::Acquire Failed.");

    // Mouse Setup
    result = m_DirectInput->CreateDevice(GUID_SysMouse, &m_Mouse, nullptr);
    popAssert(!FAILED(result), "InputManager::CreateDevice Failed.");

    result = m_Mouse->SetDataFormat(&c_dfDIMouse);
    popAssert(!FAILED(result), "InputManager::SetDataFormat Failed.");

    result = m_Mouse->SetCooperativeLevel(viewProvider.GetWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    popAssert(!FAILED(result), "InputManager::SetCooperativeLevel Failed.");

    result = m_Mouse->Acquire();
    popAssert(!FAILED(result), "InputManager::Acquire Failed.");

    return true;
#endif //POP_PLATFROM_WINDOWS

    return false;
}

void InputManager::Shutdown()
{
    if (m_Mouse)
    {
        m_Mouse->Unacquire();
        m_Mouse->Release();
        m_Mouse = nullptr;
    }

    if (m_Keyboard)
    {
        m_Keyboard->Unacquire();
        m_Keyboard->Release();
        m_Keyboard = nullptr;
    }

    // Release the main interface to direct input.
    if (m_DirectInput)
    {
        m_DirectInput->Release();
        m_DirectInput = nullptr;
    }
}

bool InputManager::Update()
{
    popProfile(InputManager::Update);

    bool result;

    result = ReadKeyboard();
    if (!result)
    {
        return false;
    }

    result = ReadMouse();
    if (!result)
    {
        return false;
    }

    ProcessInput();

    return true;
}

bool InputManager::ReadKeyboard()
{
    HRESULT result;

    // Read the keyboard device.
    memcpy(m_KeyBoardStatePrevFrame, m_KeyboardState, sizeof(m_KeyboardState));
    result = m_Keyboard->GetDeviceState(sizeof(m_KeyboardState), (LPVOID)&m_KeyboardState);
    if (FAILED(result))
    {
        // If the keyboard lost focus or was not acquired then try to get control back.
        if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
        {
            m_Keyboard->Acquire();
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool InputManager::ReadMouse()
{
    HRESULT result;

    // Read the mouse device.
    result = m_Mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_MouseState);
    if (FAILED(result))
    {
        // If the mouse lost focus or was not acquired then try to get control back.
        if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
        {
            m_Mouse->Acquire();
        }
        else
        {
            return false;
        }
    }

    return true;
}

void InputManager::ProcessInput()
{
    // Update the location of the mouse cursor based on the change of the mouse location during the frame.
    m_MouseX += m_MouseState.lX;
    m_MouseY += m_MouseState.lY;

    // Ensure the mouse location doesn't exceed the screen width or height.
    if (m_MouseX < 0) { m_MouseX = 0; }
    if (m_MouseY < 0) { m_MouseY = 0; }

    if (m_MouseX > static_cast<s32>(m_Width)) { m_MouseX = m_Width; }
    if (m_MouseY > static_cast<s32>(m_Height)) { m_MouseY = m_Height; }
}

bool InputManager::IsEscapePressed()
{
    return (m_KeyboardState[DIK_ESCAPE] & 0x80);
}

bool InputManager::IsKeyPressed(InputManager::Key key)
{
    switch (key)
    {
        //istoilov: According to documentation the high bit of the BYTE represents the state
        case Key::W : return (m_KeyboardState[DIK_W] & 0x80); break;
        case Key::A : return (m_KeyboardState[DIK_A] & 0x80); break;
        case Key::S : return (m_KeyboardState[DIK_S] & 0x80); break;
        case Key::D : return (m_KeyboardState[DIK_D] & 0x80); break;
        case Key::Q : return (m_KeyboardState[DIK_Q] & 0x80); break;
        case Key::E : return (m_KeyboardState[DIK_E] & 0x80); break;
        case Key::F1: return (m_KeyboardState[DIK_F1] & 0x80); break;

        default:
            popAssert(false, "InputManager::IsKeyPressed does not support key");
    }

    return false;
}

bool InputManager::IsKeyJustPressed(InputManager::Key key)
{
    switch (key)
    {
        //istoilov: According to documentation the high bit of the BYTE represents the state
        case Key::W:  return (m_KeyboardState[DIK_W]  & 0x80) && (!static_cast<bool>(m_KeyBoardStatePrevFrame[DIK_W]  & 0x80));
        case Key::A:  return (m_KeyboardState[DIK_A]  & 0x80) && (!static_cast<bool>(m_KeyBoardStatePrevFrame[DIK_A]  & 0x80));
        case Key::S:  return (m_KeyboardState[DIK_S]  & 0x80) && (!static_cast<bool>(m_KeyBoardStatePrevFrame[DIK_S]  & 0x80));
        case Key::D:  return (m_KeyboardState[DIK_D]  & 0x80) && (!static_cast<bool>(m_KeyBoardStatePrevFrame[DIK_D]  & 0x80));
        case Key::Q:  return (m_KeyboardState[DIK_Q]  & 0x80) && (!static_cast<bool>(m_KeyBoardStatePrevFrame[DIK_Q]  & 0x80));
        case Key::E:  return (m_KeyboardState[DIK_E]  & 0x80) && (!static_cast<bool>(m_KeyBoardStatePrevFrame[DIK_E]  & 0x80));
        case Key::F1: return (m_KeyboardState[DIK_F1] & 0x80) && (!static_cast<bool>(m_KeyBoardStatePrevFrame[DIK_F1] & 0x80));

        default:
        popAssert(false, "InputManager::IsKeyJustPressed does not support key");
    }

    return false;
}

bool InputManager::IsMouseButtonPressed(InputManager::MouseButton button)
{
    //istoilov: According to documentation the high bit of the BYTE represents the state
    return (m_MouseState.rgbButtons[(int)button] & 0x80);
}

void InputManager::GetMouseLocation(int32_t& mouseX, int32_t& mouseY)
{
    mouseX = m_MouseX;
    mouseY = m_MouseY;
}

void InputManager::GetMouseDelta(int32_t& outXdelta, int32_t& outYdelta)
{
    outXdelta = m_MouseState.lX;
    outYdelta = m_MouseState.lY;
}