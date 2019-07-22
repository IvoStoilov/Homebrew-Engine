#pragma once

class Win64_InputHandler
{
public:
    Win64_InputHandler();
    ~Win64_InputHandler();

    void Initialize();

    void KeyDown(u16 key);
    void KeyUp(u16 key);

    bool IsKeyDown(u16 key);

private:
    bool m_Keys[256];
};