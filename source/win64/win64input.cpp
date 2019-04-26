#include "precompile.h"
#include "win64/win64input.h"


Win64_InputHandler::Win64_InputHandler()
{
}

Win64_InputHandler::~Win64_InputHandler()
{
}


void Win64_InputHandler::Initialize()
{
    // Initialize all the keys to being released and not pressed.
    for (u16 i = 0; i < 256; i++)
    {
        m_Keys[i] = false;
    }

    return;
}


void Win64_InputHandler::KeyDown(u16 input)
{
    // If a key is pressed then save that state in the key array.
    m_Keys[input] = true;
    return;
}


void Win64_InputHandler::KeyUp(u16 input)
{
    // If a key is released then clear that state in the key array.
    m_Keys[input] = false;
    return;
}


bool Win64_InputHandler::IsKeyDown(u16 key)
{
    // Return what state the key is in (pressed/not pressed).
    return m_Keys[key];
}