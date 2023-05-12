#pragma once

#include "MainPage.g.h"

#include "ModelerUIControl.h"
#include "DeviceResources.h"
#include "ModelerMain.h"

#include <memory>

#include "AtomViewModel.h"
#include "ElementTypeFormatter.h" // Do NOT remove this! Even though we don't reference it directly, one of the auto generated
                                  // needs this header and does not include it for some reason. However, it does include MainPage.h
                                  // so just include it here.



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

    private:
        std::shared_ptr<DeviceResources>    m_deviceResources;
        std::unique_ptr<ModelerMain>        m_main;

        bool m_windowVisible;

        

    public:
        void ViewportGrid_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::SizeChangedEventArgs const& e);
    
    
    public:
        void BookSkuButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        winrt::ProteinModeler::AtomViewModel AtomsViewModel();
    private:
        winrt::ProteinModeler::AtomViewModel m_atomsViewModel;

    
        // Atom List View
    public:
        void AtomsListView_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void AtomsListView_ItemClick(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Controls::ItemClickEventArgs const& e);

        // Top-Right Panel
        void AddSelectViewContentFrame_NavigationFailed(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Navigation::NavigationFailedEventArgs const& e);
        void NavigationTabsAddSelectView_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void NavigationTabsAddSelectView_SelectionChanged(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);
        void NavigationTabsAddSelectView_ItemInvoked(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args);
    };
}

namespace winrt::ProteinModeler::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
