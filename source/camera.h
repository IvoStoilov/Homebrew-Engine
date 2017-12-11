#pragma once
#include <d3d11.h>
#include <d3dx10math.h>

class Camera
{
public:
    Camera();
    ~Camera();

    void SetPosition(float, float, float);
    void SetRotation(float, float, float);

    D3DXVECTOR3 GetPosition();
    D3DXVECTOR3 GetRotation();

    void Update();
    void GetViewMatrix(D3DXMATRIX&);

private:
    float m_positionX, m_positionY, m_positionZ;
    float m_rotationX, m_rotationY, m_rotationZ;
    D3DXMATRIX m_viewMatrix;
};