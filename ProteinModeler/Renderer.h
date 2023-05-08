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
	// The idea here is as follows: We want to be able to render as much as possible using a single pipeline configuration.
	// Therefore, we use a tuple to associate a PipelineConfig with all of the objects/data that will be rendered using that
	// config. The data largely consists of two things: 1) A MeshSet that holds 1+ meshes to be used during the rendering, and
	// 2) a vector of Renderable objects which hold transformation data that reference into the MeshSet. However, because a scene
	// will consist of many objects, it may not be feasible to put all necessary mesh data into a single MeshSet. Therefore, we
	// allow for a vector of MeshSet-RenderObjects tuples so that we can iterate over multiple MeshSets if necessary.
	using MeshSetAndObjectList = std::tuple<std::unique_ptr<MeshSetBase>, std::vector<std::unique_ptr<RenderableBase>>>;
	using PipelineConfigAndObjectList = std::tuple<std::unique_ptr<PipelineConfig>, std::vector<MeshSetAndObjectList>>;

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