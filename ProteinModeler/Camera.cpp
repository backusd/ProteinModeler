#include "pch.h"
#include "Camera.h"

using namespace DirectX;

Camera::Camera(const D3D11_VIEWPORT& viewport) noexcept :
	m_viewport(viewport),
    m_eye{ 0.0f, 0.0f, -10.0f },
    m_at{ 0.0f, 0.0f, 0.0f },
    m_up{ 0.0f, 1.0f, 0.0f }
{
	CreateProjectionMatrix();
}

void Camera::CreateProjectionMatrix() noexcept
{
    float fovAngleY = DirectX::XM_PI / 4;
    float aspectRatio = AspectRatio();

    // This is a simple example of a change that can be made when the app is in portrait or snapped view
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    XMMATRIX perspectiveMatrix = DirectX::XMMatrixPerspectiveFovLH(
        fovAngleY,
        aspectRatio,
        0.01f,
        1000.0f
    );

    //XMFLOAT4X4 orientation = m_deviceResources->OrientationTransform3D();
    //XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);
    XMMATRIX orientationMatrix = DirectX::XMMatrixIdentity();

    // Projection Matrix
    m_projectionMatrix = perspectiveMatrix * orientationMatrix;
}

XMFLOAT3 Camera::Position() const noexcept
{
    XMFLOAT3 position;
    DirectX::XMStoreFloat3(&position, m_eye);
    return position;
}

void Camera::SetViewport(const D3D11_VIEWPORT& viewport) noexcept
{
    m_viewport = viewport;
    CreateProjectionMatrix();
}

void Camera::Update(const Timer&)
{

}