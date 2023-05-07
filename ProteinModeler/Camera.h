#pragma once
#include "pch.h"
#include "Timer.h"

class Camera
{
public:
	Camera(const D3D11_VIEWPORT& viewport) noexcept;

	ND inline DirectX::XMMATRIX ViewMatrix() const noexcept { return DirectX::XMMatrixLookAtLH(m_eye, m_at, m_up); }
	ND inline DirectX::XMMATRIX ProjectionMatrix() const noexcept { return m_projectionMatrix; }
	ND DirectX::XMFLOAT3 Position() const noexcept;

	void SetViewport(const D3D11_VIEWPORT& viewport) noexcept;

	void Update(const Timer& timer);

private:
	ND inline float AspectRatio() const noexcept { return m_viewport.Width / m_viewport.Height; }

	void CreateProjectionMatrix() noexcept;

	// Eye/at/up vectors
	DirectX::XMVECTOR m_eye;
	DirectX::XMVECTOR m_at;
	DirectX::XMVECTOR m_up;

	DirectX::XMMATRIX m_projectionMatrix;

	D3D11_VIEWPORT m_viewport;
};