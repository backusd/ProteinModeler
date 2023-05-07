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
#include "Simulation.h"
#include "Structs.h"
#include "Timer.h"

class Renderer 
{
	using PipelineConfigAndObjectList = std::tuple<std::unique_ptr<PipelineConfig>, std::unique_ptr<MeshSetBase>, std::vector<RenderObjectList>>;

public:
	Renderer(std::shared_ptr<DeviceResources> deviceResources, Simulation* simulation);

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();
	void ReleaseDeviceDependentResources();

	void Update(const Timer& timer);
	void Render();

	void SetViewport(float top, float left, float height, float width) noexcept;


private:
	void CreateMainPipelineConfig();
	void CreateBoxPipelineConfig();
	void CreateMaterials();


	std::shared_ptr<DeviceResources> m_deviceResources;
	bool m_initialized;
	bool m_gameResourcesLoaded;

	D3D11_VIEWPORT m_viewport;

	std::vector<PipelineConfigAndObjectList> m_configsAndObjectLists;

	std::unique_ptr<Camera> m_camera;
	Simulation* m_simulation;

	// Pass Constants that will be updated/bound only once per pass
	// NOTE: the ConstantBuffer is a shared_ptr so that it can be shared with EVERY PipelineConfig
	PassConstants m_passConstants;
	std::vector<std::shared_ptr<ConstantBufferBase>> m_vsPerPassConstantsBuffers;
	std::vector<std::shared_ptr<ConstantBufferBase>> m_psPerPassConstantsBuffers;

	// Materials
	std::shared_ptr<ConstantBuffer<MaterialsArray>> m_materialsBuffer;
	std::unique_ptr<MaterialsArray> m_materials;
};