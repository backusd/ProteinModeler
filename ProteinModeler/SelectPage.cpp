#include "pch.h"
#include "SelectPage.h"
#if __has_include("SelectPage.g.cpp")
#include "SelectPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Foundation;

namespace winrt::ProteinModeler::implementation
{
    int64_t SelectPage::ModelerMainPtr()
    {
        return reinterpret_cast<int64_t>(m_modelerMain);
    }
    void SelectPage::ModelerMainPtr(int64_t value)
    {
        m_modelerMain = reinterpret_cast<ModelerMain*>(value);
        WINRT_ASSERT(m_modelerMain != nullptr);
    }


    void SelectPage::OnNavigatedTo(NavigationEventArgs const& e)
    {
        // Unbox the parameter (and cast to ModelerMain*)
        ModelerMainPtr(winrt::unbox_value<int64_t>(e.Parameter()));
    }
}
