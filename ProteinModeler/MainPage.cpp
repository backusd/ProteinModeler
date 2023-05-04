#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"

#include "winrt/Windows.UI.Input.h"

using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Core;
using namespace Windows::Graphics::Display;

namespace winrt::ProteinModeler::implementation
{
    MainPage::MainPage() :
        m_windowVisible(false)
    {
        // Xaml objects should not call InitializeComponent during construction.
        // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent

        CoreWindow coreWindow = Window::Current().CoreWindow();
        coreWindow.VisibilityChanged({ this, &MainPage::OnVisibilityChanged });
        coreWindow.SizeChanged({ this, &MainPage::OnWindowSizeChanged });

        Window window = Window::Current();
        window.Activated({ this, &MainPage::OnWindowActivationChanged });

        DisplayInformation displayInformation = DisplayInformation::GetForCurrentView();
        displayInformation.DpiChanged({ this, &MainPage::OnDpiChanged });
        displayInformation.OrientationChanged({ this, &MainPage::OnOrientationChanged });
        displayInformation.StereoEnabledChanged({ this, &MainPage::OnStereoEnabledChanged });
        DisplayInformation::DisplayContentsInvalidated({ this, &MainPage::OnDisplayContentsInvalidated });

        // Disable all pointer visual feedback for better performance when touching.
        // TODO: Understand the next 3 lines
        auto pointerVisualizationSettings = Windows::UI::Input::PointerVisualizationSettings::GetForCurrentView();
        pointerVisualizationSettings.IsContactFeedbackEnabled(false);
        pointerVisualizationSettings.IsBarrelButtonFeedbackEnabled(false);

        m_deviceResources = std::make_shared<DeviceResources>();

        m_main = std::make_unique<ModelerMain>(m_deviceResources, this);
    }
    void MainPage::InitializeComponent()
    {
        // From: https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        // "In older versions of C++/WinRT, Xaml objects called InitializeComponent from constructors. This can lead to memory corruption if 
        //  InitializeComponent throws an exception. C++/WinRT now calls InitializeComponent automatically and safely, after object construction. 
        //  Explicit calls to InitializeComponent from constructors in existing code should now be removed. Multiple calls to InitializeComponent 
        //  are idempotent. If a Xaml object needs to access a Xaml property during initialization, it should override InitializeComponent."
        // 
        // Call base InitializeComponent() to register with the Xaml runtime
        MainPageT::InitializeComponent();

        m_deviceResources->SetSwapChainPanel(DXSwapChainPanel()); 

        m_main->StartRenderLoop();
    }

    // Forwarded From App
    void MainPage::OnSuspending(const IInspectable&, const SuspendingEventArgs&)
    {
        concurrency::critical_section::scoped_lock lock(m_main->GetCriticalSection());
        m_main->Suspend();
        m_main->StopRenderLoop();

        m_deviceResources->Trim();
    }
    void MainPage::OnResuming(const IInspectable&, const IInspectable&)
    {
        m_main->Resume();
        m_main->StartRenderLoop();
    }

    // CoreWindow
    void MainPage::OnVisibilityChanged(const CoreWindow&, const VisibilityChangedEventArgs& args)
    {
        m_windowVisible = args.Visible();
        if (m_windowVisible)
        {
            m_main->StartRenderLoop();
        }
        else
        {
            m_main->StopRenderLoop();
        }
    }
    void MainPage::OnWindowSizeChanged(const CoreWindow&, const WindowSizeChangedEventArgs&)
    {
        // This would be for updates for UI elements other than the SwapChainPanel because we handle DXSwapChainPanel_SizeChanged elsewhere
    }

    // Window
    void MainPage::OnWindowActivationChanged(const IInspectable&, const WindowActivatedEventArgs& args)
    {
        // TODO: Understand why this scoped lock is necessary
        concurrency::critical_section::scoped_lock lock(m_main->GetCriticalSection());
        m_main->WindowActivationChanged(args.WindowActivationState());
    }

    // DisplayInformation
    void MainPage::OnDpiChanged(const DisplayInformation& displayInfo, const IInspectable&)
    {
        concurrency::critical_section::scoped_lock lock(m_main->GetCriticalSection());
        m_deviceResources->SetDpi(displayInfo.LogicalDpi());
        m_main->CreateWindowSizeDependentResources();
    }
    void MainPage::OnOrientationChanged(const DisplayInformation& displayInfo, const IInspectable&)
    {
        concurrency::critical_section::scoped_lock lock(m_main->GetCriticalSection());
        m_deviceResources->SetCurrentOrientation(displayInfo.CurrentOrientation());
        m_main->CreateWindowSizeDependentResources();
    }
    void MainPage::OnStereoEnabledChanged(const DisplayInformation&, const IInspectable&)
    {
        concurrency::critical_section::scoped_lock lock(m_main->GetCriticalSection());
        m_deviceResources->UpdateStereoState();
        m_main->CreateWindowSizeDependentResources();
    }
    void MainPage::OnDisplayContentsInvalidated(const DisplayInformation&, const IInspectable&)
    {
        concurrency::critical_section::scoped_lock lock(m_main->GetCriticalSection());
        m_deviceResources->ValidateDevice();
    }

    // SwapChainPanel
    void MainPage::DXSwapChainPanel_SizeChanged(const IInspectable&, const SizeChangedEventArgs& args)
    {
        concurrency::critical_section::scoped_lock lock(m_main->GetCriticalSection());
        m_deviceResources->SetLogicalSize(args.NewSize());
        m_main->CreateWindowSizeDependentResources();
    }
    void MainPage::DXSwapChainPanel_CompositionScaleChanged(const Controls::SwapChainPanel& sender, const IInspectable&)
    {
        concurrency::critical_section::scoped_lock lock(m_main->GetCriticalSection());
        m_deviceResources->SetCompositionScale(sender.CompositionScaleX(), sender.CompositionScaleY());
        m_main->CreateWindowSizeDependentResources();
    }





    int32_t MainPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void MainPage::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        myButton().Content(box_value(L"Clicked"));
    }
}






