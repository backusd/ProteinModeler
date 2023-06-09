﻿#pragma once

#include "AddPage.g.h"
#include "AtomViewModel.h"
#include "ModelerMain.h"

namespace winrt::ProteinModeler::implementation
{
    struct AddPage : AddPageT<AddPage>
    {
        AddPage() 
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        void OnNavigatedTo(winrt::Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        void NavigationAdd_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void NavigationAdd_ItemInvoked(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args);
    
        
        int64_t ModelerMainPtr();
        void ModelerMainPtr(int64_t value);

    private:
        ModelerMain* m_modelerMain;
    };
}

namespace winrt::ProteinModeler::factory_implementation
{
    struct AddPage : AddPageT<AddPage, implementation::AddPage>
    {
    };
}
