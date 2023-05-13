#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"

#include "winrt/Windows.UI.Input.h"

using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Core;
using namespace Windows::Graphics::Display;

namespace winrt::ProteinModeler::implementation
{
    // NOTE: Using uniform construction, so no need to call winrt::make to instantiate runtime types
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

        m_main = std::make_shared<ModelerMain>(m_deviceResources, this);
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


    // Viewport
    void MainPage::ViewportGrid_SizeChanged(IInspectable const& sender, SizeChangedEventArgs const& e)
    {
        concurrency::critical_section::scoped_lock lock(m_main->GetCriticalSection());

        auto viewportGrid = sender.as<Controls::Grid>();
        
        float top = viewportGrid.ActualOffset().y;
        float left = viewportGrid.ActualOffset().x;

        float height = e.NewSize().Height;
        float width = e.NewSize().Width;

        m_main->SetViewport(top, left, height, width);
    }







    void MainPage::BookSkuButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ProteinModeler::Atom atom(ElementType::Hydrogen, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f });
        AtomsViewModel().Atoms().Append(atom);
    }
    ProteinModeler::AtomViewModel MainPage::AtomsViewModel()
    {
        return m_atomsViewModel;
    }





    void MainPage::AtomsListView_SelectionChanged(IInspectable const& sender, SelectionChangedEventArgs const& e)
    {
        for (auto item : e.AddedItems())
        {
            auto lvi = sender.as<ListView>().ContainerFromItem(item).as<ListViewItem>();
            lvi.ContentTemplate(this->Resources().Lookup(box_value(L"AtomListViewItemExpanded")).as<DataTemplate>()); 
        }

        for (auto item : e.RemovedItems())
        {
            auto lvi = sender.as<ListView>().ContainerFromItem(item).as<ListViewItem>();
            lvi.ContentTemplate(this->Resources().Lookup(box_value(L"AtomListViewItemCollapsed")).as<DataTemplate>());
        }
    }
    void MainPage::AtomsListView_ItemClick(IInspectable const& sender, ItemClickEventArgs const& e)
    {
        DataTemplate collapsed = this->Resources().Lookup(box_value(L"AtomListViewItemCollapsed")).as<DataTemplate>();
        DataTemplate expanded  = this->Resources().Lookup(box_value(L"AtomListViewItemExpanded")).as<DataTemplate>();

        auto lvi = sender.as<ListView>().ContainerFromItem(e.ClickedItem()).as<ListViewItem>();

        if (collapsed == lvi.ContentTemplate())
        {
            lvi.ContentTemplate(expanded);
        }
        else
        {
            lvi.ContentTemplate(collapsed);
        }
    }




    void MainPage::AddSelectViewContentFrame_NavigationFailed(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Navigation::NavigationFailedEventArgs const& e)
    {
        throw winrt::hresult_error(E_FAIL, winrt::hstring(L"Failed to load Page ") + e.SourcePageType().Name);
    }
    void MainPage::NavigationTabsAddSelectView_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
        // NavigationView doesn't load any page by default, so load Add page.
        // This requires 2 steps: 1) Select the first Tab, and 2) Load the content frame to the Add page
        NavigationTabsAddSelectView().SelectedItem(NavigationTabsAddSelectView().MenuItems().GetAt(0));
        AddSelectViewContentFrame().Navigate(winrt::xaml_typename<ProteinModeler::AddPage>(), winrt::box_value<int64_t>(reinterpret_cast<int64_t>(m_main.get())));
    }
    void MainPage::NavigationTabsAddSelectView_ItemInvoked(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args)
    {
        if (args.InvokedItemContainer())
        {
            hstring tag = winrt::unbox_value<hstring>(args.InvokedItemContainer().Tag());

            Windows::UI::Xaml::Interop::TypeName previousPage = AddSelectViewContentFrame().CurrentSourcePageType(); 

            if (tag == L"AddPage")
            {
                if (previousPage.Name != L"ProteinModeler.AddPage")
                    AddSelectViewContentFrame().Navigate(winrt::xaml_typename<ProteinModeler::AddPage>(), winrt::box_value<int64_t>(reinterpret_cast<int64_t>(m_main.get())));
            }
            else if (tag == L"SelectPage")
            {
                if (previousPage.Name != L"ProteinModeler.SelectPage")
                    AddSelectViewContentFrame().Navigate(winrt::xaml_typename<ProteinModeler::SelectPage>(), winrt::box_value<int64_t>(reinterpret_cast<int64_t>(m_main.get())));
            }
            else if (tag == L"ViewPage")
            {
                if (previousPage.Name != L"ProteinModeler.ViewPage")
                    AddSelectViewContentFrame().Navigate(winrt::xaml_typename<ProteinModeler::ViewPage>(), winrt::box_value<int64_t>(reinterpret_cast<int64_t>(m_main.get())));
            }
        }
    }
   
}






