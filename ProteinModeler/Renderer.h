#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "InputLayout.h"
#include "Shaders.h"
#include "RasterizerState.h"
#include "BlendState.h"
#include "DepthStencilState.h"
#include "ConstantBufferArray.h"
#include "PipelineConfig.h"
#include "MeshSet.h"
#include "RenderObjectList.h"
#include "Camera.h"

struct WorldViewProjectionMatrix
{
	DirectX::XMFLOAT4X4 worldViewProjection;
};
struct BoxVertex
{
	DirectX::XMFLOAT3 Position;
};

class Renderer 
{
	using PipelineConfigAndObjectList = std::tuple<std::unique_ptr<PipelineConfig>, std::unique_ptr<MeshSetBase>, std::vector<RenderObjectList>>;

public:
	Renderer(std::shared_ptr<DeviceResources> deviceResources);

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();
	void ReleaseDeviceDependentResources();
	void Render();

	void SetViewport(float top, float left, float height, float width) noexcept;

	void CreateBoxPipelineConfig();

private:
	std::shared_ptr<DeviceResources> m_deviceResources;
	bool m_initialized;
	bool m_gameResourcesLoaded;

	D3D11_VIEWPORT m_viewport;

	std::vector<PipelineConfigAndObjectList> m_configsAndObjectLists;

	std::unique_ptr<Camera> m_camera;

	DirectX::XMFLOAT3 m_translation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
};