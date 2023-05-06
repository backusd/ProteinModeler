#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "ModelerUIControl.h"
#include "Renderer.h"

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

private:
    void UpdateLayoutState();
    void Update();


private:
    bool m_haveFocus;

    std::shared_ptr<DeviceResources> m_deviceResources;
    IModelerUIControl* m_uiControl;

    std::unique_ptr<Renderer> m_renderer;

    Concurrency::critical_section            m_criticalSection;
    winrt::Windows::Foundation::IAsyncAction m_renderLoopWorker;
};