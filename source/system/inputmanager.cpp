#include "system\inputmanager.h"
#include "system\error.h"

InputManager* InputManager::s_Instance = nullptr;

InputManager::InputManager() :
    m_DirectInput(nullptr),
    m_Keyboard(nullptr),
    m_Mouse(nullptr)
{}


InputManager::~InputManager()
{}

InputManager* InputManager::GetInstance()
{
    popAssert(s_Instance, "InputManager Create Instance Not called");
    return s_Instance;
}

void InputManager::CleanInstance()
{
    if (s_Instance != nullptr)
    {
        s_Instance->Shutdown();
        delete s_Instance;
    }
}

void InputManager::CreateInstance(HINSTANCE hInstance, HWND hwnd, uint32_t width, uint32_t height, int32_t windowPosX, int32_t windowPosY)
{
    if (s_Instance == nullptr)
    {
        s_Instance = new InputManager();
        s_Instance->Initialize(hInstance, hwnd, width, height, windowPosX, windowPosY);
    }
}

bool InputManager::Initialize(HINSTANCE hInstance, HWND hwnd, uint32_t width, uint32_t height, int32_t windowPosX, int32_t windowPosY)
{
    m_Width = width;
    m_Height = height;

    m_WindowPosX = windowPosX;
    m_WindowPosY = windowPosY;

    m_MouseX = 0;
    m_MouseY = 0;

    HRESULT result;
    result = DirectInput8Create(hInstance, DIRECT_INPUTVERSION, IID_IDirectInput8, (void**)& m_DirectInput, nullptr);
    popAssert(!FAILED(result), "InputManager::Initialize Failed.");

    //Keyboard setup
    result = m_DirectInput->CreateDevice(GUID_SysKeyboard, &m_Keyboard, nullptr);
    popAssert(!FAILED(result), "InputManager::CreateDevice Failed.");

    result = m_Keyboard->SetDataFormat(&c_dfDIKeyboard);
    popAssert(!FAILED(result), "InputManager::SetDataFormat Failed.");

    result = m_Keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
    popAssert(!FAILED(result), "InputManager::SetCooperativeLevel Failed.");

    result = m_Keyboard->Acquire();
    popAssert(!FAILED(result), "InputManager::Acquire Failed.");

    // Mouse Setup
    result = m_DirectInput->CreateDevice(GUID_SysMouse, &m_Mouse, nullptr);
    popAssert(!FAILED(result), "InputManager::CreateDevice Failed.");

    result = m_Mouse->SetDataFormat(&c_dfDIMouse);
    popAssert(!FAILED(result), "InputManager::SetDataFormat Failed.");

    result = m_Mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    popAssert(!FAILED(result), "InputManager::SetCooperativeLevel Failed.");

    result = m_Mouse->Acquire();
    popAssert(!FAILED(result), "InputManager::Acquire Failed.");

    return true;
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

bool InputManager::Frame()
{
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

    if (m_MouseX > m_Width) { m_MouseX = m_Width; }
    if (m_MouseY > m_Height) { m_MouseY = m_Height; }
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