#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "ConstantBufferArray.h"
#include "Shaders.h"
#include "InputLayout.h"
#include "RasterizerState.h"
#include "BlendState.h"
#include "DepthStencilState.h"

class PipelineConfig
{
public:
	PipelineConfig(std::shared_ptr<DeviceResources> deviceResources,
					std::unique_ptr<VertexShader> vertexShader,
					std::unique_ptr<PixelShader> pixelShader,
					std::unique_ptr<InputLayout> inputLayout,
					std::unique_ptr<RasterizerState> rasterizerState,
					std::unique_ptr<BlendState> blendState,
					std::unique_ptr<DepthStencilState> depthStencilState,
					std::unique_ptr<ConstantBufferArray> vertexShaderConstantBufferArray,
					std::unique_ptr<ConstantBufferArray> pixelShaderConstantBufferArray) :
		m_deviceResources(deviceResources),
		m_topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
		m_blendFactor{ 1.0f, 1.0f, 1.0f, 1.0f },
		m_blendSampleMask(0xffffffff),
		m_stencilRef(0u),
		m_vertexShader(std::move(vertexShader)),
		m_pixelShader(std::move(pixelShader)),
		m_inputLayout(std::move(inputLayout)),
		m_rasterizerState(std::move(rasterizerState)),
		m_blendState(std::move(blendState)),
		m_depthStencilState(std::move(depthStencilState)),
		m_vertexShaderConstantBufferArray(std::move(vertexShaderConstantBufferArray)),
		m_pixelShaderConstantBufferArray(std::move(pixelShaderConstantBufferArray))
	{
		WINRT_ASSERT(m_deviceResources != nullptr);
		WINRT_ASSERT(m_vertexShader != nullptr);
		WINRT_ASSERT(m_pixelShader != nullptr);
		WINRT_ASSERT(m_inputLayout != nullptr);
		WINRT_ASSERT(m_rasterizerState != nullptr);
		WINRT_ASSERT(m_blendState != nullptr);
		WINRT_ASSERT(m_depthStencilState != nullptr);
	}
	PipelineConfig(const PipelineConfig&) noexcept = delete;
	PipelineConfig& operator=(const PipelineConfig&) noexcept = delete;
	~PipelineConfig() noexcept {}

	void ApplyConfig() const
	{
		WINRT_ASSERT(m_deviceResources != nullptr);

		auto context = m_deviceResources->GetD3DDeviceContext();

		// bind pixel shader
		context->PSSetShader(m_pixelShader->Get(), nullptr, 0u);

		// bind vertex shader
		context->VSSetShader(m_vertexShader->Get(), nullptr, 0u);

		// bind vertex layout
		context->IASetInputLayout(m_inputLayout->Get());

		// Set primitive topology to triangle list (groups of 3 vertices)
		context->IASetPrimitiveTopology(m_topology);

		// Set Rasterizer State
		context->RSSetState(m_rasterizerState->Get());

		// Set Blend State
		context->OMSetBlendState(m_blendState->Get(), m_blendFactor, m_blendSampleMask);

		// Set Depth Stencil State
		context->OMSetDepthStencilState(m_depthStencilState->Get(), m_stencilRef);

		// Set the VS constant buffers
		if (m_vertexShaderConstantBufferArray != nullptr)
			m_vertexShaderConstantBufferArray->BindVS();

		// Set the PS constant buffers
		if (m_pixelShaderConstantBufferArray != nullptr)
			m_pixelShaderConstantBufferArray->BindPS();
	}

	inline void SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology) noexcept { m_topology = topology; }
	inline void SetBlendFactor(float blendFactors[4]) noexcept { memcpy(m_blendFactor, blendFactors, sizeof(float) * 4); }
	inline void SetBlendSampleMask(unsigned int mask) noexcept { m_blendSampleMask = mask; }
	inline void SetStencilRef(unsigned int ref) noexcept { m_stencilRef = ref; }

private:
	std::shared_ptr<DeviceResources> m_deviceResources;

	std::unique_ptr<VertexShader>		 m_vertexShader;
	std::unique_ptr<PixelShader>		 m_pixelShader;
	std::unique_ptr<InputLayout>		 m_inputLayout;
	std::unique_ptr<RasterizerState>	 m_rasterizerState;
	std::unique_ptr<BlendState>			 m_blendState;
	std::unique_ptr<DepthStencilState>	 m_depthStencilState;
	std::unique_ptr<ConstantBufferArray> m_vertexShaderConstantBufferArray;
	std::unique_ptr<ConstantBufferArray> m_pixelShaderConstantBufferArray;

	D3D11_PRIMITIVE_TOPOLOGY m_topology;
	float                    m_blendFactor[4];
	unsigned int             m_blendSampleMask;
	unsigned int			 m_stencilRef;
};