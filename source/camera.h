#pragma once
#include "system/math/vec4.h"
#include "system/math/mat4x4.h"

class Camera
{
public:
    Camera();
    ~Camera();

    void SetPosition(float, float, float);
    void SetRotation(float, float, float);

    vec4 GetPosition();
    vec4 GetRotation();

    void Update();
    void UpdateMovement();

    void GetViewMatrix(mat4x4& outViewMatrix);

private:
    float m_positionX, m_positionY, m_positionZ;
    float m_rotationX, m_rotationY, m_rotationZ;
    mat4x4 m_ViewMatrix;

    float m_MoveSpeed;
};