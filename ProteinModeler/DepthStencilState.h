#pragma once
#include "pch.h"
#include "DeviceResources.h"

class DepthStencilState
{
public:
	DepthStencilState(std::shared_ptr<DeviceResources> deviceResources, const D3D11_DEPTH_STENCIL_DESC& desc) :
		m_deviceResources(deviceResources),
		m_depthStencilState(nullptr)
	{
		WINRT_ASSERT(m_deviceResources != nullptr);

		winrt::check_hresult(
			m_deviceResources->GetD3DDevice()->CreateDepthStencilState(&desc, m_depthStencilState.put())
		);
	}
	DepthStencilState(const DepthStencilState&) noexcept = delete;
	DepthStencilState& operator=(const DepthStencilState&) noexcept = delete;
	virtual ~DepthStencilState() noexcept {}

	ND inline ID3D11DepthStencilState* Get() { return m_depthStencilState.get(); }

protected:
	std::shared_ptr<DeviceResources> m_deviceResources;
	winrt::com_ptr<ID3D11DepthStencilState> m_depthStencilState;
};