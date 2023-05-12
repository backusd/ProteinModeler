#pragma once

#include "ViewPage.g.h"

namespace winrt::ProteinModeler::implementation
{
    struct ViewPage : ViewPageT<ViewPage>
    {
        ViewPage() 
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }
    };
}

namespace winrt::ProteinModeler::factory_implementation
{
    struct ViewPage : ViewPageT<ViewPage, implementation::ViewPage>
    {
    };
}
