﻿#include "pch.h"
#include "AddProteinPage.h"
#if __has_include("AddProteinPage.g.cpp")
#include "AddProteinPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Foundation;

namespace winrt::ProteinModeler::implementation
{
    int64_t AddProteinPage::ModelerMainPtr()
    {
        return reinterpret_cast<int64_t>(m_modelerMain);
    }
    void AddProteinPage::ModelerMainPtr(int64_t value)
    {
        m_modelerMain = reinterpret_cast<ModelerMain*>(value);
        WINRT_ASSERT(m_modelerMain != nullptr);
    }


    void AddProteinPage::OnNavigatedTo(NavigationEventArgs const& e)
    {
        // Unbox the parameter (and cast to ModelerMain*)
        ModelerMainPtr(winrt::unbox_value<int64_t>(e.Parameter()));
    }
}
