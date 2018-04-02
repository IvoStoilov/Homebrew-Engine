#include "camera.h"

#include "system/inputmanager.h"

#include <d3d11.h>
#include <d3dx10math.h>

Camera::Camera()
{
    m_positionX = 0.0f;
    m_positionY = 0.0f;
    m_positionZ = 0.0f;

    m_rotationX = 0.0f;
    m_rotationY = 0.0f;
    m_rotationZ = 0.0f;

    m_MoveSpeed = .5f;
}

Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
    m_positionX = x;
    m_positionY = y;
    m_positionZ = z;
    return;
}


void Camera::SetRotation(float x, float y, float z)
{
    m_rotationX = x;
    m_rotationY = y;
    m_rotationZ = z;
    return;
}

vec4 Camera::GetPosition() const
{
    return vec4(m_positionX, m_positionY, m_positionZ, 1.f);
}


vec4 Camera::GetRotation() const
{
    return vec4(m_rotationX, m_rotationY, m_rotationZ, 1.f);
}

vec4 Camera::GetUpAxis() const
{
    return vec4((m_ViewMatrix[0][1]), (m_ViewMatrix[1][1]), (m_ViewMatrix[2][1]), 0.f);
}

void Camera::UpdateMovement()
{
    int32_t mouseDeltaX;
    int32_t mouseDeltaY;

    if (g_InputManager->IsMouseButtonPressed(InputManager::MouseButton::LEFT))
    {
        g_InputManager->GetMouseDelta(mouseDeltaX, mouseDeltaY);
        m_rotationY += mouseDeltaX * 0.1f;
        m_rotationX += mouseDeltaY * 0.1f;
    }
}

void Camera::Update()
{
    UpdateMovement();

    if (g_InputManager->IsKeyPressed(InputManager::Key::Q))
    {
        m_MoveSpeed += .5f;
    }

    if (g_InputManager->IsKeyPressed(InputManager::Key::E) && (m_MoveSpeed - .5f) > 0)
    {
        m_MoveSpeed -= .5f;
    }

    D3DXVECTOR3 up, position, lookAt;
    float yaw, pitch, roll;
    D3DXMATRIX rotationMatrix;

    // Setup the vector that points upwards.
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;

    // Setup where the camera is looking by default.
    lookAt.x = 0.f;
    lookAt.y = 0.f;
    lookAt.z = 1.0f;

    // Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
    pitch = m_rotationX * 0.0174532925f;
    yaw = m_rotationY * 0.0174532925f;
    roll = m_rotationZ * 0.0174532925f;

    // Create the rotation matrix from the yaw, pitch, and roll values.
    D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

    // Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
    D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
    D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

    D3DXVECTOR3 moveZ(0.f, 0.f, 0.f), moveX(0.f, 0.f, 0.f);
    D3DXVECTOR3 dirZ(0.f, 0.f, 1.f);
    D3DXVECTOR3 dirX(1.f, 0.f, 0.f);

    D3DXVec3TransformCoord(&dirZ, &dirZ, &rotationMatrix);
    D3DXVec3TransformCoord(&dirX, &dirX, &rotationMatrix);

    if (g_InputManager->IsKeyPressed(InputManager::Key::W))
    {
        moveZ += dirZ * m_MoveSpeed;
    }
    if (g_InputManager->IsKeyPressed(InputManager::Key::A))
    {
        moveX -= dirX * m_MoveSpeed;
    }
    if (g_InputManager->IsKeyPressed(InputManager::Key::S))
    {
        moveZ -= dirZ * m_MoveSpeed;
    }
    if (g_InputManager->IsKeyPressed(InputManager::Key::D))
    {
        moveX += dirX * m_MoveSpeed;
    }

    // Setup the position of the camera in the world.
    position.x = m_positionX;
    position.y = m_positionY;
    position.z = m_positionZ;

    position += moveX;
    position += moveZ;

    m_positionX += moveX.x;
    m_positionX += moveZ.x;

    m_positionY += moveX.y;
    m_positionY += moveZ.y;

    m_positionZ += moveX.z;
    m_positionZ += moveZ.z;

    // Translate the rotated camera position to the location of the viewer.
    lookAt = position + lookAt;

    // Finally create the view matrix from the three updated vectors.
    D3DXMATRIX viewMatrix;
    D3DXMatrixLookAtLH(&viewMatrix, &position, &lookAt, &up);
    m_ViewMatrix = viewMatrix;
}

void Camera::GetViewMatrix(mat4x4& viewMatrix)
{
    viewMatrix = m_ViewMatrix;
}