#pragma once
#include "pch.h"
#include "DeviceResources.h"



class Renderer 
{
public:
	Renderer(std::shared_ptr<DeviceResources> deviceResources);

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();
	void ReleaseDeviceDependentResources();
	void Render();


private:
	std::shared_ptr<DeviceResources> m_deviceResources;
	bool m_initialized;
	bool m_gameResourcesLoaded;
};