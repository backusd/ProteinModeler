#pragma once
#include "pch.h"
#include "DeviceResources.h"

class RasterizerState
{
public:
	RasterizerState(std::shared_ptr<DeviceResources> deviceResources, const D3D11_RASTERIZER_DESC& desc) :
		m_deviceResources(deviceResources),
		m_rasterizerState(nullptr)
	{
		WINRT_ASSERT(m_deviceResources != nullptr);

		winrt::check_hresult(
			m_deviceResources->GetD3DDevice()->CreateRasterizerState(&desc, m_rasterizerState.put())
		);
	}
	RasterizerState(const RasterizerState&) noexcept = delete;
	RasterizerState& operator=(const RasterizerState&) noexcept = delete;
	virtual ~RasterizerState() noexcept {}

	ND inline ID3D11RasterizerState* Get() { return m_rasterizerState.get(); }

protected:
	std::shared_ptr<DeviceResources> m_deviceResources;
	winrt::com_ptr<ID3D11RasterizerState> m_rasterizerState;
};