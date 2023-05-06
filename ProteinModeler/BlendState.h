#pragma once
#include "pch.h"
#include "DeviceResources.h"

class BlendState
{
public:
	BlendState(std::shared_ptr<DeviceResources> deviceResources, const D3D11_BLEND_DESC& desc) :
		m_deviceResources(deviceResources),
		m_blendState(nullptr)
	{
		WINRT_ASSERT(m_deviceResources != nullptr);

		winrt::check_hresult(
			m_deviceResources->GetD3DDevice()->CreateBlendState(&desc, m_blendState.put())
		);
	}
	BlendState(const BlendState&) noexcept = delete;
	BlendState& operator=(const BlendState&) noexcept = delete;
	virtual ~BlendState() noexcept {}

	ND inline ID3D11BlendState* Get() { return m_blendState.get(); }

protected:
	std::shared_ptr<DeviceResources> m_deviceResources;
	winrt::com_ptr<ID3D11BlendState> m_blendState;
};