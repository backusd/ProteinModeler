#include "pch.h"
#include "AddAtomPage.h"
#if __has_include("AddAtomPage.g.cpp")
#include "AddAtomPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Foundation;

namespace winrt::ProteinModeler::implementation
{
    int64_t AddAtomPage::ModelerMainPtr()
    {
        return reinterpret_cast<int64_t>(m_modelerMain);
    }
    void AddAtomPage::ModelerMainPtr(int64_t value)
    {
        m_modelerMain = reinterpret_cast<ModelerMain*>(value);
    }

    void AddAtomPage::OnNavigatedTo(NavigationEventArgs const& e)
    {
        // Unbox the parameter (and cast to ModelerMain*)
        ModelerMainPtr(winrt::unbox_value<int64_t>(e.Parameter()));
    }

    void AddAtomPage::AddAtomButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
        WINRT_ASSERT(m_modelerMain != nullptr);
        concurrency::critical_section::scoped_lock lock(m_modelerMain->GetCriticalSection());
        m_modelerMain->AddAtom(Element::Hydrogen, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
    }

}


