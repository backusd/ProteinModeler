#pragma once
#include "pch.h"
#include "DeviceResources.h"

class Shader
{
public:
	Shader(std::shared_ptr<DeviceResources> deviceResources, const std::wstring& filename) noexcept :
		m_deviceResources(deviceResources),
		m_filename(filename)
	{
		WINRT_ASSERT(m_deviceResources != nullptr);
		WINRT_ASSERT(m_filename.size() > 0);
	}
	Shader(const Shader&) noexcept = delete;
	Shader& operator=(const Shader&) noexcept = delete;
	virtual ~Shader() noexcept {}

protected:
	std::shared_ptr<DeviceResources> m_deviceResources;
	std::wstring m_filename;
};

// -------------------------------------------------------------------------

class VertexShader : public Shader
{
public:
	VertexShader(std::shared_ptr<DeviceResources> deviceResources, const std::wstring& filename) :
		Shader(deviceResources, filename),
		m_vertexShader(nullptr),
		m_blob(nullptr)
	{
		// Create Pixel Shader
		D3DReadFileToBlob(filename.c_str(), m_blob.put());
		
		winrt::check_hresult(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), nullptr, m_vertexShader.put())
		);
	}
	VertexShader(const VertexShader&) noexcept = delete;
	VertexShader& operator=(const VertexShader&) noexcept = delete;
	virtual ~VertexShader() noexcept override {}

	ND inline ID3D11VertexShader* Get() noexcept { return m_vertexShader.get(); }
	ND inline void* GetBufferPointer() { return m_blob->GetBufferPointer(); }
	ND inline SIZE_T GetBufferSize() { return m_blob->GetBufferSize(); }

private:
	winrt::com_ptr<ID3D11VertexShader>	m_vertexShader;
	winrt::com_ptr<ID3DBlob>			m_blob;
};

// -------------------------------------------------------------------------

class PixelShader : public Shader
{
public:
	PixelShader(std::shared_ptr<DeviceResources> deviceResources, const std::wstring& filename) :
		Shader(deviceResources, filename),
		m_pixelShader(nullptr)
	{
		// Create Pixel Shader
		winrt::com_ptr<ID3DBlob> pBlob = nullptr;
		D3DReadFileToBlob(filename.c_str(), pBlob.put());
		winrt::check_hresult(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, m_pixelShader.put())
		);
	}
	PixelShader(const PixelShader&) noexcept = delete;
	PixelShader& operator=(const PixelShader&) noexcept = delete;
	virtual ~PixelShader() noexcept override {}

	ND inline ID3D11PixelShader* Get() noexcept { return m_pixelShader.get(); }

private:
	winrt::com_ptr<ID3D11PixelShader> m_pixelShader;
};