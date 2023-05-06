#pragma once
#include "pch.h"
#include "Shaders.h"

class InputLayout
{
public:
	InputLayout(std::shared_ptr<DeviceResources> deviceResources, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements, VertexShader* vs) :
		m_deviceResources(deviceResources),
		m_inputLayout(nullptr)
	{
		WINRT_ASSERT(m_deviceResources != nullptr);

		winrt::check_hresult(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				inputElements.data(),
				static_cast<UINT>(inputElements.size()),
				vs->GetBufferPointer(),
				vs->GetBufferSize(),
				m_inputLayout.put()
			)
		);
	}
	InputLayout(const InputLayout&) noexcept = delete;
	InputLayout& operator=(const InputLayout&) noexcept = delete;
	virtual ~InputLayout() noexcept {}

	ND inline ID3D11InputLayout* Get() { return m_inputLayout.get(); }

protected:
	std::shared_ptr<DeviceResources> m_deviceResources;
	winrt::com_ptr<ID3D11InputLayout> m_inputLayout;
};

