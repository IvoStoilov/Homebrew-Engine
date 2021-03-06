#include <engine/precompile.h>

#include <engine/camera.h>

#include <system/inputmanager.h>
#include <system/math/mathutil.h>

Camera::Camera()
{
    m_positionX = 0.0f;
    m_positionY = 0.0f;
    m_positionZ = 0.0f;

    m_rotationX = 0.0f;
    m_rotationY = 0.0f;
    m_rotationZ = 0.0f;

    m_MoveSpeed = 15.f;
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
    return vec4(m_ViewMatrix.Up());
}

mat4x4 Camera::ComputeReflectionMatrix(f32 planeHeight) const
{
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);
    f32 distance = 2.f * (m_positionY - planeHeight);
    f32 cameraPosX = m_positionX;
    f32 cameraPosY = m_positionY - distance;
    f32 cameraPosZ = m_positionZ;
    DirectX::XMVECTOR cameraPosition = DirectX::XMVectorSet(cameraPosX, cameraPosY, cameraPosZ, 1.f);
    
    // Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
    f32 pitch = -1.f * MathUtil::ToRads(m_rotationX);
    f32 yaw   =        MathUtil::ToRads(m_rotationY);
    f32 roll  =        MathUtil::ToRads(m_rotationZ);

    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

    DirectX::XMVECTOR lookAt = DirectX::XMVectorSet(0.f, 0.f, 1.f, 0.f);
    lookAt = DirectX::XMVector3TransformCoord(lookAt, rotationMatrix);
    up = DirectX::XMVector3TransformCoord(up, rotationMatrix);

    lookAt = DirectX::XMVectorAdd(lookAt, cameraPosition);

    return DirectX::XMMatrixLookAtRH(cameraPosition, lookAt, up);
}

void Camera::UpdateMovement()
{
    s32 mouseDeltaX;
    s32 mouseDeltaY;

    if (g_InputManager.IsMouseButtonPressed(InputManager::MouseButton::LEFT))
    {
        g_InputManager.GetMouseDelta(mouseDeltaX, mouseDeltaY);
        m_rotationY += mouseDeltaX * -0.1f;
        m_rotationX += mouseDeltaY * +0.1f;
    }
}

void Camera::Update(float dt)
{
    popProfile(Camera::Update);

    UpdateMovement();

    if (g_InputManager.IsKeyJustPressed(InputManager::Key::Q))
    {
        m_MoveSpeed += .5f;
    }

    if (g_InputManager.IsKeyJustPressed(InputManager::Key::E) && (m_MoveSpeed - .5f) > 0)
    {
        m_MoveSpeed -= .5f;
    }

    vec3 up, position, lookAt;
    f32 yaw, pitch, roll;
    mat4x4 rotationMatrix;

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
    rotationMatrix = mat4x4::CreateFromYawPitchRoll(yaw, pitch, roll);

    // Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
    lookAt = vec3::Transform(lookAt, rotationMatrix);
    up = vec3::Transform(up, rotationMatrix);
    
    vec3 moveZ(0.f, 0.f, 0.f), moveX(0.f, 0.f, 0.f);
    vec3 dirZ(0.f, 0.f, 1.f);
    vec3 dirX(1.f, 0.f, 0.f);

    dirZ = vec3::Transform(dirZ, rotationMatrix);
    dirX = vec3::Transform(dirX, rotationMatrix);

    if (g_InputManager.IsKeyPressed(InputManager::Key::W))
    {
        moveZ += dirZ * m_MoveSpeed * dt;
    }
    if (g_InputManager.IsKeyPressed(InputManager::Key::A))
    {
        moveX += dirX * m_MoveSpeed * dt;
    }
    if (g_InputManager.IsKeyPressed(InputManager::Key::S))
    {
        moveZ -= dirZ * m_MoveSpeed * dt;
    }
    if (g_InputManager.IsKeyPressed(InputManager::Key::D))
    {
        moveX -= dirX * m_MoveSpeed * dt;
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
    m_ViewMatrix = mat4x4::CreateLookAt(position, lookAt, up);
}