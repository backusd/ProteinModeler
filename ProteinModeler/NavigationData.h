#pragma once
#include "NavigationData.g.h"
#include "ModelerMain.h"

namespace winrt::ProteinModeler::implementation
{
    struct NavigationData : NavigationDataT<NavigationData>
    {
        NavigationData() = default;

        void DummyMethod();

        std::shared_ptr<ModelerMain> m_main;
    };
}
namespace winrt::ProteinModeler::factory_implementation
{
    struct NavigationData : NavigationDataT<NavigationData, implementation::NavigationData>
    {
    };
}
