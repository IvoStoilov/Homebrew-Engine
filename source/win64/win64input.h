#pragma once
#include <stdint.h>
class Win64_InputHandler
{
public:
    Win64_InputHandler();
    ~Win64_InputHandler();

    void Initialize();

    void KeyDown(uint16_t key);
    void KeyUp(uint16_t key);

    bool IsKeyDown(uint16_t key);

private:
    bool m_Keys[256];
};