#pragma once

#include "AddPage.g.h"

namespace winrt::ProteinModeler::implementation
{
    struct AddPage : AddPageT<AddPage>
    {
        AddPage() 
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }
    };
}

namespace winrt::ProteinModeler::factory_implementation
{
    struct AddPage : AddPageT<AddPage, implementation::AddPage>
    {
    };
}
