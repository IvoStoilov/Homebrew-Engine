#pragma once
#include "system/math/mat4x4.h"

class Entity
{
public:
    Entity();
    ~Entity();

    inline Transform& GetGlobalMatrix() { return m_GlobalMatrix; }
    inline void SetGlobalMatrix(const Transform& globalMatrix) { m_GlobalMatrix = globalMatrix; }

    void Rotate(float degree);

private:
    Transform m_GlobalMatrix;
};