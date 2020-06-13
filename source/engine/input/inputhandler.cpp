#include <engine/precompile.h>
#include <engine/input/inputhandler.h>

#include <graphics/RenderingEngine.h>

#include <system/inputmanager.h>

void InputHandler::Update()
{
    if (g_InputManager.IsKeyJustPressed(InputManager::Key::F1))
    {
        //bool isEnabled = g_RenderEngine.IsEnabledDebugDisplay();
        //g_RenderEngine.EnableDebugDisplay(!isEnabled);
    }

    if (g_InputManager.IsKeyJustPressed(InputManager::Key::P))
    {
        m_IsInProfiling = !m_IsInProfiling;
        if (m_IsInProfiling)
            g_ProfileManager.BeginSession();
        else
            g_ProfileManager.EndSession();
    }
}

