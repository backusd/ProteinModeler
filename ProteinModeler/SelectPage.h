#pragma once

#include "SelectPage.g.h"

namespace winrt::ProteinModeler::implementation
{
    struct SelectPage : SelectPageT<SelectPage>
    {
        SelectPage() 
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }
    };
}

namespace winrt::ProteinModeler::factory_implementation
{
    struct SelectPage : SelectPageT<SelectPage, implementation::SelectPage>
    {
    };
}
