#include "precompile.h"
#include "engine/input/inputhandler.h"

#include "graphics/d3d11renderer.h"

#include "system/inputmanager.h"

void InputHandler::Update()
{
    if (g_InputManager->IsKeyJustPressed(InputManager::Key::F1))
    {
        bool isEnabled = g_RenderEngine->IsEnabledDebugDisplay();
        g_RenderEngine->EnableDebugDisplay(!isEnabled);
    }
}

