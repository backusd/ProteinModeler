#pragma once

#include "AddProteinPage.g.h"
#include "ModelerMain.h"

namespace winrt::ProteinModeler::implementation
{
    struct AddProteinPage : AddProteinPageT<AddProteinPage>
    {
        AddProteinPage() 
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }
        void OnNavigatedTo(winrt::Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        int64_t ModelerMainPtr();
        void ModelerMainPtr(int64_t value);

    private:
        ModelerMain* m_modelerMain;
    };
}

namespace winrt::ProteinModeler::factory_implementation
{
    struct AddProteinPage : AddProteinPageT<AddProteinPage, implementation::AddProteinPage>
    {
    };
}
