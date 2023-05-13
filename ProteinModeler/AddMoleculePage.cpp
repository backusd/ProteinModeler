#include "pch.h"
#include "AddMoleculePage.h"
#if __has_include("AddMoleculePage.g.cpp")
#include "AddMoleculePage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Foundation;

namespace winrt::ProteinModeler::implementation
{
    int64_t AddMoleculePage::ModelerMainPtr()
    {
        return reinterpret_cast<int64_t>(m_modelerMain);
    }
    void AddMoleculePage::ModelerMainPtr(int64_t value)
    {
        m_modelerMain = reinterpret_cast<ModelerMain*>(value);
        WINRT_ASSERT(m_modelerMain != nullptr);
    }


    void AddMoleculePage::OnNavigatedTo(NavigationEventArgs const& e)
    {
        // Unbox the parameter (and cast to ModelerMain*)
        ModelerMainPtr(winrt::unbox_value<int64_t>(e.Parameter()));
    }
}
