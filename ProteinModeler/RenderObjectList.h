#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "MeshSet.h"
#include "Timer.h"

#define MAX_INSTANCES 1024 // TODO: Why is this defined at 1024 and why is it used in the RenderObjectList constructor

class RenderObject
{
public:
	RenderObject(const DirectX::XMFLOAT3& scaling, const DirectX::XMFLOAT3* translation, unsigned int materialIndex) :
		m_scaling(scaling),
		m_translation(translation),
		m_materialIndex(materialIndex)
	{
		WINRT_ASSERT(translation != nullptr);
	}
	// Must implement copy constructor because it is required when stored in std::vector. See https://stackoverflow.com/questions/40457302/c-vector-emplace-back-calls-copy-constructor
	RenderObject(const RenderObject&) noexcept = default;
	~RenderObject() noexcept {};

	ND inline DirectX::XMMATRIX WorldMatrix() const noexcept
	{
		return DirectX::XMMatrixTranspose(
					DirectX::XMMatrixScaling(m_scaling.x, m_scaling.y, m_scaling.z) *
					DirectX::XMMatrixTranslation(m_translation->x, m_translation->y, m_translation->z)
			   );
	}
	ND inline DirectX::XMFLOAT4X4 WorldMatrix4X4() const noexcept
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world, WorldMatrix());
		return world;
	}
	ND inline unsigned int MaterialIndex() const noexcept
	{
		return m_materialIndex;
	}

private:
	DirectX::XMFLOAT3			m_scaling;
	const DirectX::XMFLOAT3*	m_translation; // Hold a pointer to the translation data which should be managed elsewhere
	unsigned int				m_materialIndex;
};

// -----------------------------------------------------------------------------------

class RenderObjectList
{
public:
	RenderObjectList(std::shared_ptr<DeviceResources> deviceResources, const MeshInstance& mesh) :
		m_deviceResources(deviceResources),
		m_mesh(mesh),
		m_instanceBuffer(nullptr)
	{
		WINRT_ASSERT(deviceResources != nullptr);
	}
	// Must implement copy constructor because it is required when stored in std::vector. See https://stackoverflow.com/questions/40457302/c-vector-emplace-back-calls-copy-constructor
	RenderObjectList(const RenderObjectList&) noexcept = default;

	//void Update(const Evergreen::Timer& timer) noexcept;
	void Render() const
	{
		WINRT_ASSERT(m_deviceResources != nullptr);
		WINRT_ASSERT(m_renderObjects.size() > 0); // Must have a non-zero number of objects to render
		WINRT_ASSERT(m_worldMatrices.size() == m_renderObjects.size());

		auto context = m_deviceResources->GetD3DDeviceContext();

		// If there is only a single render object, we don't need to call DrawIndexedInstanced, instead, just call DrawIndexed
		if (m_renderObjects.size() == 1)
		{
			m_BufferUpdateFn(this, 0, 0);
			context->DrawIndexed(m_mesh.IndexCount, m_mesh.StartIndexLocation, m_mesh.BaseVertexLocation);
		}
		else
		{
			// There are multiple objects that want to be rendered with the same mesh instance, so use DrawIndexedInstanced
			WINRT_ASSERT(m_instanceBuffer != nullptr);

			// Loop over the world matrices and draw up to MAX_INSTANCES at a time
			size_t endIndex = 0;
			for (size_t startIndex = 0; startIndex < m_worldMatrices.size(); startIndex += MAX_INSTANCES)
			{
				endIndex = std::min(startIndex + MAX_INSTANCES, m_worldMatrices.size()) - 1;
			
				// Need to assign lambda that will update pipeline constant buffers 
				m_BufferUpdateFn(this, startIndex, endIndex);
			
				// TODO: Wrap this in THROW_INFO_ONLY macro
				context->DrawIndexedInstanced(m_mesh.IndexCount, static_cast<UINT>(endIndex - startIndex + 1), m_mesh.StartIndexLocation, m_mesh.BaseVertexLocation, 0u);
			}
		}
	}

	inline void AddRenderObject(const DirectX::XMFLOAT3& scaling, const DirectX::XMFLOAT3* translation, unsigned int materialIndex)
	{
		m_renderObjects.emplace_back(scaling, translation, materialIndex);
		m_worldMatrices.push_back(m_renderObjects.back().WorldMatrix4X4());
		m_materialIndices.push_back(materialIndex);

		// If we are rendering more than one render object, then we are going to be doing instanced rendering, so create the instance buffer
		if (m_renderObjects.size() > 1)
			CreateInstanceBuffer();
	}

	inline void Update(const Timer&) noexcept
	{
		WINRT_ASSERT(m_worldMatrices.size() == m_renderObjects.size()); // Number of world matrices and render objects should match

		// Re-compute all world matrices every frame because their positions will be changing
		for (unsigned int iii = 0; iii < m_renderObjects.size(); ++iii)
			m_worldMatrices[iii] = m_renderObjects[iii].WorldMatrix4X4();
	}

	ND inline std::shared_ptr<DeviceResources> GetDeviceResources() const noexcept { return m_deviceResources; }
	ND inline const std::vector<DirectX::XMFLOAT4X4>& GetWorldMatrices() const noexcept { return m_worldMatrices; }
	ND inline const std::vector<unsigned int>& GetMaterialIndices() const noexcept { return m_materialIndices; }
	ND inline winrt::com_ptr<ID3D11Buffer> GetInstanceBuffer() const noexcept { return m_instanceBuffer; }
	ND inline size_t RenderObjectCount() const noexcept { return m_renderObjects.size(); }

public:
	std::function<void(const RenderObjectList*, size_t, size_t)> m_BufferUpdateFn = [](const RenderObjectList*, size_t, size_t) {};

private:
	void CreateInstanceBuffer()
	{
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0u;
		bd.ByteWidth = static_cast<UINT>(MAX_INSTANCES * sizeof(unsigned int)); // Size of buffer in bytes
		bd.StructureByteStride = sizeof(unsigned int);
		
		m_instanceBuffer = nullptr; // Release
		winrt::check_hresult(
			m_deviceResources->GetD3DDevice()->CreateBuffer(&bd, nullptr, m_instanceBuffer.put())
		);
	}


	std::shared_ptr<DeviceResources> m_deviceResources;
	MeshInstance					 m_mesh;

	// Right now, each instance just requires an index into the materials array
	winrt::com_ptr<ID3D11Buffer> m_instanceBuffer;

	std::vector<RenderObject>		 m_renderObjects;
	std::vector<DirectX::XMFLOAT4X4> m_worldMatrices;
	std::vector<unsigned int>		 m_materialIndices;
};