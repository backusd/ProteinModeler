#pragma once

#include "AddMoleculePage.g.h"

namespace winrt::ProteinModeler::implementation
{
    struct AddMoleculePage : AddMoleculePageT<AddMoleculePage>
    {
        AddMoleculePage() 
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }


    };
}

namespace winrt::ProteinModeler::factory_implementation
{
    struct AddMoleculePage : AddMoleculePageT<AddMoleculePage, implementation::AddMoleculePage>
    {
    };
}
