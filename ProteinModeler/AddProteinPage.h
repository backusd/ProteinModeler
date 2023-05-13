#pragma once

#include "AddProteinPage.g.h"

namespace winrt::ProteinModeler::implementation
{
    struct AddProteinPage : AddProteinPageT<AddProteinPage>
    {
        AddProteinPage() 
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

    };
}

namespace winrt::ProteinModeler::factory_implementation
{
    struct AddProteinPage : AddProteinPageT<AddProteinPage, implementation::AddProteinPage>
    {
    };
}
