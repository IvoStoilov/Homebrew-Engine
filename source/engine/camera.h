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

    inline float GetMoveSpeed() const { return m_MoveSpeed; }

    DirectX::XMMATRIX ComputeReflectionMatrix(f32 planeHeight) const;

    vec4 GetPosition() const;
    vec4 GetRotation() const;

    vec4 GetUpAxis() const;

    void Update(float dt);
    void UpdateMovement();

    void GetViewMatrix(mat4x4& outViewMatrix);

private:
    float m_positionX, m_positionY, m_positionZ;
    float m_rotationX, m_rotationY, m_rotationZ;
    mat4x4 m_ViewMatrix;
    
    float m_MoveSpeed;
};