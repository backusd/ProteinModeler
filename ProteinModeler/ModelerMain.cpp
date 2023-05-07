#include "pch.h"
#include "ModelerMain.h"
#include "Timer.h"

using winrt::Windows::Foundation::AsyncStatus;
using winrt::Windows::Foundation::IAsyncAction;
using winrt::Windows::System::Threading::ThreadPool;
using winrt::Windows::System::Threading::WorkItemHandler;
using winrt::Windows::System::Threading::WorkItemPriority;
using winrt::Windows::System::Threading::WorkItemOptions;
using winrt::Windows::UI::Core::CoreWindowActivationState;


ModelerMain::ModelerMain(const std::shared_ptr<DeviceResources>& deviceResources, IModelerUIControl* UIControl) :
    m_deviceResources(deviceResources),
    m_uiControl(UIControl),
    m_haveFocus(false)
{
    m_deviceResources->RegisterDeviceNotify(this);

    m_simulation = std::make_unique<Simulation>();

    m_simulation->Add(Element::Helium, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f });
    m_simulation->Add(Element::Helium, { 0.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f });
    //for (unsigned int iii = 0; iii < 20; ++iii)
    //    m_simulation->Add(Element::Hydrogen, { 0.0f, 0.0f, 0.0f }, { iii / 57.0f,  iii / 48.0f,  iii / 55.0f });

    m_renderer = std::make_unique<Renderer>(m_deviceResources, m_simulation.get());
}

ModelerMain::~ModelerMain()
{
    // Deregister device notification
    m_deviceResources->RegisterDeviceNotify(nullptr);
}

void ModelerMain::CreateWindowSizeDependentResources()
{
    UpdateLayoutState();
    m_renderer->CreateWindowSizeDependentResources();

    if (m_renderLoopWorker == nullptr || m_renderLoopWorker.Status() != AsyncStatus::Started)
    {
        StartRenderLoop();
    }
}

void ModelerMain::WindowActivationChanged(CoreWindowActivationState activationState)
{
    // TODO: Write this method based on Simple3DGameXaml, but it will differ significantly because ModelerMain will have different states
    //          Also, understand this code and are there other CoreWindowActivationStates?

    if (activationState == CoreWindowActivationState::Deactivated)
    {
        m_haveFocus = false;
    }
    else if (activationState == CoreWindowActivationState::CodeActivated ||
             activationState == CoreWindowActivationState::PointerActivated)
    {
        m_haveFocus = true;
    }
}

void ModelerMain::Suspend() 
{

}

void ModelerMain::Resume() 
{

}

void ModelerMain::UpdateLayoutState()
{
    // TODO: Write this method based on Simple3DGameXaml, but it will differ significantly because ModelerMain will have different states
}

void ModelerMain::StartRenderLoop() 
{
    // If the animation render loop is already running then do not start another thread.
    if (m_renderLoopWorker != nullptr && m_renderLoopWorker.Status() == AsyncStatus::Started)
    {
        return;
    }

    // Create a task that will be run on a background thread.
    auto workItemHandler = WorkItemHandler([this](IAsyncAction action)
        {
            Timer timer;

            // Start the simulation
            m_simulation->Play();

            // Calculate the updated frame and render once per vertical blanking interval.
            while (action.Status() == AsyncStatus::Started)
            {
                concurrency::critical_section::scoped_lock lock(m_criticalSection);

                // Update =========================================================================
                timer.Tick([&]()
                    {
                        m_simulation->Update(timer);
                        m_renderer->Update(timer);
                    }
                );

                // Render =========================================================================
                m_renderer->Render();

                // Present ========================================================================
                m_deviceResources->Present();

                if (!m_haveFocus)
                {
                    // The app is in an inactive state so stop rendering
                    // This optimizes for power and allows the framework to become more queiecent
                    break;
                }
            }

            m_simulation->Pause();
        });

    // Run task on a dedicated high priority background thread.
    m_renderLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);
}
void ModelerMain::StopRenderLoop() 
{
    m_renderLoopWorker.Cancel();
    m_simulation->Pause();
}
