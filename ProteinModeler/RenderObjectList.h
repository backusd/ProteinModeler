#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "MeshSet.h"
#include "Timer.h"

#define MAX_INSTANCES 1024 // TODO: Why is this defined at 1024 and why is it used in the RenderObjectList constructor

class RenderableBase
{
public:
	RenderableBase(std::shared_ptr<DeviceResources> deviceResources, const MeshInstance& mesh) noexcept :
		m_deviceResources(deviceResources),
		m_mesh(mesh)
	{
		WINRT_ASSERT(deviceResources != nullptr);
	}
	// Must implement copy constructor because it is required when stored in std::vector. See https://stackoverflow.com/questions/40457302/c-vector-emplace-back-calls-copy-constructor
	RenderableBase(const RenderableBase& rhs) noexcept :
		m_deviceResources(rhs.m_deviceResources),
		m_mesh(rhs.m_mesh)
	{}
	RenderableBase& operator=(const RenderableBase& rhs) noexcept
	{
		m_deviceResources = rhs.m_deviceResources;
		m_mesh = rhs.m_mesh;
		return *this;
	}
	virtual ~RenderableBase() noexcept {};

	virtual void Render() const = 0;
	virtual void Update(const Timer&) {}

protected:
	std::shared_ptr<DeviceResources> m_deviceResources;
	MeshInstance					 m_mesh;
};

// =================================================================================================================================================

class RenderObject : public RenderableBase
{
public:
	RenderObject(std::shared_ptr<DeviceResources> deviceResources, const MeshInstance& mesh, const DirectX::XMFLOAT3& scaling, const DirectX::XMFLOAT3* translation, unsigned int materialIndex) noexcept :
		RenderableBase(deviceResources, mesh),
		m_scaling(scaling),
		m_translation(translation),
		m_materialIndex(materialIndex)
	{
		WINRT_ASSERT(translation != nullptr);
	}
	// Must implement copy constructor because it is required when stored in std::vector. 
	// See https://stackoverflow.com/questions/40457302/c-vector-emplace-back-calls-copy-constructor
	RenderObject(const RenderObject& rhs) noexcept :
		RenderableBase(rhs),
		m_scaling(rhs.m_scaling),
		m_translation(rhs.m_translation),
		m_materialIndex(rhs.m_materialIndex),
		m_BufferUpdateFn(rhs.m_BufferUpdateFn)
	{}
	RenderObject& operator=(RenderObject& rhs) noexcept
	{
		RenderableBase::operator=(rhs);
		m_scaling = rhs.m_scaling;
		m_translation = rhs.m_translation;
		m_materialIndex = rhs.m_materialIndex;
		m_BufferUpdateFn = rhs.m_BufferUpdateFn;
	}
	virtual ~RenderObject() noexcept override {};

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

	virtual void Render() const override
	{
		WINRT_ASSERT(m_deviceResources != nullptr);
		m_BufferUpdateFn(this);
		m_deviceResources->GetD3DDeviceContext()->DrawIndexed(m_mesh.IndexCount, m_mesh.StartIndexLocation, m_mesh.BaseVertexLocation);
	}

	std::function<void(const RenderObject*)> m_BufferUpdateFn = [](const RenderObject*) {};

private:
	DirectX::XMFLOAT3		 m_scaling;
	const DirectX::XMFLOAT3* m_translation; // Hold a pointer to the translation data which should be managed elsewhere
	unsigned int			 m_materialIndex;
};

// =================================================================================================================================================

template<typename T>
class RenderObjectInstanced : public RenderableBase
{
public:
	RenderObjectInstanced(std::shared_ptr<DeviceResources> deviceResources, const MeshInstance& mesh) noexcept :
		RenderableBase(deviceResources, mesh)
	{
		CreateInstanceBuffer();
	}
	// Must implement copy constructor because it is required when stored in std::vector. 
	// See https://stackoverflow.com/questions/40457302/c-vector-emplace-back-calls-copy-constructor
	RenderObjectInstanced(const RenderObjectInstanced& rhs) noexcept :
		RenderableBase(rhs),
		m_renderObjects(rhs.m_renderObjects),
		m_materialIndices(rhs.m_materialIndices),
		m_worldMatrices(rhs.m_worldMatrices)
	{
		CreateInstanceBuffer();
	}
	RenderObjectInstanced& operator=(RenderObjectInstanced& rhs) noexcept
	{
		RenderableBase::operator=(rhs);

		m_renderObjects.assign(rhs.m_renderObjects.begin(), rhs.m_renderObjects.end());
		m_materialIndices.assign(rhs.m_materialIndices.begin(), rhs.m_materialIndices.end());
		m_worldMatrices.assign(rhs.m_worldMatrices.begin(), rhs.m_worldMatrices.end());

		CreateInstanceBuffer();		
	}
	virtual ~RenderObjectInstanced() noexcept override {};

	virtual void Render() const override
	{
		WINRT_ASSERT(m_deviceResources != nullptr); 
		WINRT_ASSERT(m_renderObjects.size() > 0); // Must have a non-zero number of objects to render 
		WINRT_ASSERT(m_worldMatrices.size() == m_renderObjects.size()); 
		WINRT_ASSERT(m_instanceBuffer != nullptr);

		auto context = m_deviceResources->GetD3DDeviceContext();

		UINT strides[1] = { sizeof(T) }; 
		UINT offsets[1] = { 0u };
		ID3D11Buffer* vertInstBuffers[1] = { m_instanceBuffer.get() };
		// TODO: Wrap this in a THROW_INFO_ONLY macro
		context->IASetVertexBuffers(1u, 1u, vertInstBuffers, strides, offsets); 

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
	inline void AddInstance(const DirectX::XMFLOAT3& scaling, const DirectX::XMFLOAT3* translation, unsigned int materialIndex)
	{
		m_renderObjects.emplace_back(m_deviceResources, m_mesh, scaling, translation, materialIndex);
		m_worldMatrices.push_back(m_renderObjects.back().WorldMatrix4X4());
		m_materialIndices.push_back(materialIndex);
	}

	inline virtual void Update(const Timer&) noexcept override
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
	ND inline size_t InstanceCount() const noexcept { return m_renderObjects.size(); }

	std::function<void(const RenderObjectInstanced*, size_t, size_t)> m_BufferUpdateFn = [](const RenderObjectInstanced*, size_t, size_t) {};

private:
	void CreateInstanceBuffer()
	{
		WINRT_ASSERT(m_deviceResources != nullptr);

		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0u;
		bd.ByteWidth = static_cast<UINT>(MAX_INSTANCES * sizeof(T)); // Size of buffer in bytes
		bd.StructureByteStride = sizeof(T);

		m_instanceBuffer = nullptr; // Release
		winrt::check_hresult(
			m_deviceResources->GetD3DDevice()->CreateBuffer(&bd, nullptr, m_instanceBuffer.put())
		);
	}

	std::vector<RenderObject>		 m_renderObjects;
	std::vector<DirectX::XMFLOAT4X4> m_worldMatrices;
	std::vector<unsigned int>		 m_materialIndices;

	// Right now, each instance just requires an index into the materials array
	winrt::com_ptr<ID3D11Buffer> m_instanceBuffer;
};
