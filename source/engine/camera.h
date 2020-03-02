#pragma once
class Camera
{
public:
    Camera();
    ~Camera();

    void SetPosition(float, float, float);
    void SetRotation(float, float, float);

    inline float GetMoveSpeed() const { return m_MoveSpeed; }

    mat4x4 ComputeReflectionMatrix(f32 planeHeight) const;

    vec4 GetPosition() const;
    vec4 GetRotation() const;

    vec4 GetUpAxis() const;

    void Update(float dt);
    void UpdateMovement();

    inline void GetViewMatrix(mat4x4& outViewMatrix) const { outViewMatrix = m_ViewMatrix; }
    inline const mat4x4& GetViewMatrix() const { return m_ViewMatrix; }

private:
    f32 m_positionX, m_positionY, m_positionZ;
    f32 m_rotationX, m_rotationY, m_rotationZ;
    f32 m_MoveSpeed;
    mat4x4 m_ViewMatrix;
    
    
};