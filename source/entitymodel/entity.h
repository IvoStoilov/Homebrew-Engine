#pragma once
#include "system/math/mat4x4.h"

class Entity
{
public:
    Entity();
    ~Entity();

    inline Transform& GetGlobalMatrix() { return m_GlobalMatrix; }
    inline void SetGlobalMatrix(const Transform& globalMatrix) { m_GlobalMatrix = globalMatrix; }
    inline void SetGlobalPosition(const vec4& pos) { m_GlobalMatrix.SetTranslate(pos); }

    void Update(float dt);

    void Rotate(float degree);

private:
    Transform m_GlobalMatrix;

    int32_t m_Angle;
};