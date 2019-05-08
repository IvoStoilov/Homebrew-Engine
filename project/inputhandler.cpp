#include "precompile.h"
#include "core/input/inputhandler.h"

#include "renderer/d3d11renderer.h"

#include "system/inputmanager.h"

void InputHandler::Update()
{
    if (g_InputManager->IsKeyJustPressed(InputManager::Key::F1))
    {
        bool isEnabled = g_RenderEngine->IsEnabledDebugDisplay();
        g_RenderEngine->EnableDebugDisplay(!isEnabled);
    }
}

