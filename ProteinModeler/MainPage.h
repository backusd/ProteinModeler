#pragma once

#include "MainPage.g.h"

#include "ModelerUIControl.h"
#include "DeviceResources.h"
#include "ModelerMain.h"

#include <memory>



namespace winrt::ProteinModeler::implementation
{
    struct MainPage : MainPageT<MainPage>, IModelerUIControl
    {
        MainPage();
        void InitializeComponent();

        // Forwarded from App
        void OnSuspending(const Windows::Foundation::IInspectable&, const Windows::ApplicationModel::SuspendingEventArgs&);
        void OnResuming(const Windows::Foundation::IInspectable&, const Windows::Foundation::IInspectable&);

        // Core Window
        void OnVisibilityChanged(const Windows::UI::Core::CoreWindow& sender, const Windows::UI::Core::VisibilityChangedEventArgs& args);
        void OnWindowSizeChanged(const Windows::UI::Core::CoreWindow& sender, const Windows::UI::Core::WindowSizeChangedEventArgs& args);
        
        // Window
        void OnWindowActivationChanged(const Windows::Foundation::IInspectable& sender, const Windows::UI::Core::WindowActivatedEventArgs& args);

        // Display Information
        void OnDpiChanged(const Windows::Graphics::Display::DisplayInformation& displayInfo, const Windows::Foundation::IInspectable& args);
        void OnOrientationChanged(const Windows::Graphics::Display::DisplayInformation& displayInfo, const Windows::Foundation::IInspectable& args);
        void OnStereoEnabledChanged(const Windows::Graphics::Display::DisplayInformation& displayInfo, const Windows::Foundation::IInspectable& args);
        void OnDisplayContentsInvalidated(const Windows::Graphics::Display::DisplayInformation& displayInfo, const Windows::Foundation::IInspectable& args);

        // SwapChainPanel
        void DXSwapChainPanel_SizeChanged(const winrt::Windows::Foundation::IInspectable& sender, const winrt::Windows::UI::Xaml::SizeChangedEventArgs& e);
        void DXSwapChainPanel_CompositionScaleChanged(const winrt::Windows::UI::Xaml::Controls::SwapChainPanel& sender, const winrt::Windows::Foundation::IInspectable& args);

        // IModelerUIControl
        virtual void SetModelLoading() override
        {

        }




        int32_t MyProperty();
        void MyProperty(int32_t value);

        void ClickHandler(const Windows::Foundation::IInspectable& sender, const Windows::UI::Xaml::RoutedEventArgs& args);

    private:
        std::shared_ptr<DeviceResources>    m_deviceResources;
        std::unique_ptr<ModelerMain>        m_main;

        bool m_windowVisible;

    };
}

namespace winrt::ProteinModeler::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
