#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "ModelerUIControl.h"
#include "Renderer.h"
#include "Simulation.h"


#include <ppl.h>

class ModelerMain : public IDeviceNotify
{
public:
    ModelerMain(const std::shared_ptr<DeviceResources>& deviceResources, IModelerUIControl* UIControl);
    ~ModelerMain();
    void CreateWindowSizeDependentResources();
    void StartRenderLoop();
    void StopRenderLoop();
    void Suspend();
    void Resume();

    concurrency::critical_section& GetCriticalSection() { return m_criticalSection; }

    void WindowActivationChanged(winrt::Windows::UI::Core::CoreWindowActivationState activationState);


    // IDeviceNotify
    void OnDeviceLost() override {}
    void OnDeviceRestored() override {}

    // Rendering Stuff
    inline void SetViewport(float top, float left, float height, float width) const { m_renderer->SetViewport(top, left, height, width); }

    // Modification Methods
    void AddAtom(Element element, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& velocity) noexcept 
    { 
        size_t index = m_simulation->Add(element, position, velocity); 
        m_renderer->AddInstance(element, &m_simulation->Positions()[index]);
    }

private:
    void UpdateLayoutState();


private:
    bool m_haveFocus;

    std::shared_ptr<DeviceResources> m_deviceResources;
    IModelerUIControl* m_uiControl;

    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Simulation> m_simulation;

    Concurrency::critical_section            m_criticalSection;
    winrt::Windows::Foundation::IAsyncAction m_renderLoopWorker;
};