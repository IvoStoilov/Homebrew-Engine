#pragma once
#include <vector>
#include <functional>

class InputHandler
{
public:
    void Update();

private:
    bool m_IsInProfiling = false;
};