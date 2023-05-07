#pragma once
#include "pch.h"
#include "DeviceResources.h"


class ConstantBufferBase
{
public:
	ConstantBufferBase(std::shared_ptr<DeviceResources> deviceResources) noexcept :
		m_deviceResources(deviceResources),
		m_buffer(nullptr)
	{
		WINRT_ASSERT(m_deviceResources != nullptr);
	}
	ConstantBufferBase(const ConstantBufferBase&) = delete;
	ConstantBufferBase& operator=(const ConstantBufferBase&) = delete;
	virtual ~ConstantBufferBase() noexcept {}

	ND inline ID3D11Buffer* GetRawBufferPointer() const noexcept { return m_buffer.get(); }

	inline void UpdateData(void* data) noexcept
	{
		WINRT_ASSERT(m_deviceResources != nullptr);

		// TODO: Wrap this in a THROW_INFO_ONLY macro
		m_deviceResources->GetD3DDeviceContext()->UpdateSubresource(
			m_buffer.get(), // Resource to be updated
			0u,				// 0-based index that identified the destination subresource. Will always be 0 when updating a single constant buffer
			nullptr,		// D3D11_BOX that defines the portion of the subresource to copy. "For a shader-constant buffer, set pDstBox to NULL"
			data,			// Pointer to data to copy to the subresource
			0u,				// "SourceRowPitch" - Not really sure what this is for, but it can be 0 for constant buffers
			0u				// "SourceDepthPitch" - Not really sure what this is for, but it can be 0 for constant buffers
		);
	}

protected:
	std::shared_ptr<DeviceResources> m_deviceResources;
	winrt::com_ptr<ID3D11Buffer>     m_buffer;
};

template<typename T>
class ConstantBuffer : public ConstantBufferBase
{
public:
	ConstantBuffer(std::shared_ptr<DeviceResources> deviceResources, D3D11_USAGE usage, unsigned int cpuAccessFlags, unsigned int miscFlags, unsigned int structuredByteStride, void* initialData = nullptr) noexcept :
		ConstantBufferBase(deviceResources)
	{
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = sizeof(T);
		desc.Usage = usage;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = cpuAccessFlags;
		desc.MiscFlags = miscFlags;
		desc.StructureByteStride = structuredByteStride;

		// MUST make sure we release the data in the buffer. Note that unlike ComPtr::ReleaseAndGetAddressOf(), I think that calling m_buffer.put() 
		// doesn't actually release the underlying contents - I think we need to intentionally need to assign as nullptr first
		m_buffer = nullptr;

		D3D11_SUBRESOURCE_DATA* pData = nullptr;
		D3D11_SUBRESOURCE_DATA data{};

		if (initialData != nullptr)
		{
			data.pSysMem = initialData;
			data.SysMemPitch = 0;			// Only relevant for 2D/3D textures
			data.SysMemSlicePitch = 0;		// Only relevant for 2D/3D textures

			pData = &data;
		}

		winrt::check_hresult(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&desc,			// Use the description we just created
				pData,			// Fill the buffer with the passed in data (if nullptr, will not fill with any data)
				m_buffer.put()	// Assign result to buffer - See note above about using put()
			)
		);
	}
	ConstantBuffer(const ConstantBuffer&) = delete;
	ConstantBuffer& operator=(const ConstantBuffer&) = delete;
	virtual ~ConstantBuffer() noexcept override {}
};

// ========================================================================================================================================
// Constant Buffer Array

enum class ShaderStage
{
	COMPUTE_SHADER = 0,
	VERTEX_SHADER = 1,
	HULL_SHADER = 2,
	DOMAIN_SHADER = 3,
	GEOMETRY_SHADER = 4,
	PIXEL_SHADER = 5
};

class ConstantBufferArray
{
public:
	ConstantBufferArray(std::shared_ptr<DeviceResources> deviceResources) noexcept :
		m_deviceResources(deviceResources)
	{
		WINRT_ASSERT(m_deviceResources != nullptr);
	}
	ConstantBufferArray(const ConstantBufferArray&) = delete;
	ConstantBufferArray& operator=(const ConstantBufferArray&) = delete;
	~ConstantBufferArray() noexcept {}

	inline void AddBuffer(std::shared_ptr<ConstantBufferBase> buffer) noexcept
	{
		WINRT_ASSERT(buffer != nullptr);
		m_buffers.push_back(buffer);
		m_rawBufferPointers.push_back(m_buffers.back()->GetRawBufferPointer());
	}
	inline void ClearBuffers() noexcept
	{
		m_buffers.clear();
		m_rawBufferPointers.clear();
	}

	// TODO: Add THROW_INFO_ONLY macro to each of the below *SetConstantBuffer calls

	inline void BindCS() const
	{
		WINRT_ASSERT(m_deviceResources != nullptr);
		m_deviceResources->GetD3DDeviceContext()->CSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data());
	}
	inline void BindVS() const
	{
		WINRT_ASSERT(m_deviceResources != nullptr);
		m_deviceResources->GetD3DDeviceContext()->VSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data());
	}
	inline void BindHS() const
	{
		WINRT_ASSERT(m_deviceResources != nullptr);
		m_deviceResources->GetD3DDeviceContext()->HSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data());
	}
	inline void BindDS() const
	{
		WINRT_ASSERT(m_deviceResources != nullptr);
		m_deviceResources->GetD3DDeviceContext()->DSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data());
	}
	inline void BindGS() const
	{
		WINRT_ASSERT(m_deviceResources != nullptr);
		m_deviceResources->GetD3DDeviceContext()->GSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data());
	}
	inline void BindPS() const
	{
		WINRT_ASSERT(m_deviceResources != nullptr);
		m_deviceResources->GetD3DDeviceContext()->PSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data());
	}

protected:
	std::shared_ptr<DeviceResources> m_deviceResources;
	std::vector<ID3D11Buffer*> m_rawBufferPointers;
	std::vector<std::shared_ptr<ConstantBufferBase>> m_buffers;
};