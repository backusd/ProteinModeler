#pragma once

#include "AddAtomPage.g.h"
#include "ModelerMain.h"

namespace winrt::ProteinModeler::implementation
{
    struct AddAtomPage : AddAtomPageT<AddAtomPage>
    {
        AddAtomPage() 
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        void OnNavigatedTo(winrt::Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        int64_t ModelerMainPtr();
        void ModelerMainPtr(int64_t value);

    private:
        ModelerMain* m_modelerMain;
    public:
        void AddAtomButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::ProteinModeler::factory_implementation
{
    struct AddAtomPage : AddAtomPageT<AddAtomPage, implementation::AddAtomPage>
    {
    };
}
