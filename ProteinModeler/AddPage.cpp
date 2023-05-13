#include "pch.h"
#include "AddPage.h"
#if __has_include("AddPage.g.cpp")
#include "AddPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Foundation;

namespace winrt::ProteinModeler::implementation
{
    int64_t AddPage::ModelerMainPtr()
    {
        return reinterpret_cast<int64_t>(m_modelerMain);
    }
    void AddPage::ModelerMainPtr(int64_t value)
    {
        m_modelerMain = reinterpret_cast<ModelerMain*>(value);
        WINRT_ASSERT(m_modelerMain != nullptr);
    }
    void AddPage::OnNavigatedTo(NavigationEventArgs const& e)
    {
        // Unbox the parameter (and cast to ModelerMain*)
        ModelerMainPtr(winrt::unbox_value<int64_t>(e.Parameter()));
    }

	void AddPage::NavigationAdd_Loaded(IInspectable const& sender, RoutedEventArgs const& e)
	{
		// NavigationView doesn't load any page by default, so load AddAtom page.
		// This requires 2 steps: 1) Select the first Tab, and 2) Load the content frame to the Add Atom page
		NavigationAdd().SelectedItem(NavigationAdd().MenuItems().GetAt(0));
		AddContentFrame().Navigate(winrt::xaml_typename<ProteinModeler::AddAtomPage>(), winrt::box_value<int64_t>(reinterpret_cast<int64_t>(m_modelerMain)));
	}

	void AddPage::NavigationAdd_ItemInvoked(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args)
	{
        if (args.InvokedItemContainer())
        {
            hstring tag = winrt::unbox_value<hstring>(args.InvokedItemContainer().Tag());

            Windows::UI::Xaml::Interop::TypeName previousPage = AddContentFrame().CurrentSourcePageType();

            if (tag == L"AddAtom")
            {
                if (previousPage.Name != L"ProteinModeler.AddAtomPage")
                    AddContentFrame().Navigate(winrt::xaml_typename<ProteinModeler::AddAtomPage>(), winrt::box_value<int64_t>(reinterpret_cast<int64_t>(m_modelerMain)));
            }
            else if (tag == L"AddMolecule")
            {
                if (previousPage.Name != L"ProteinModeler.AddMoleculePage")
                    AddContentFrame().Navigate(winrt::xaml_typename<ProteinModeler::AddMoleculePage>(), winrt::box_value<int64_t>(reinterpret_cast<int64_t>(m_modelerMain)));
            }
            else if (tag == L"AddProtein")
            {
                if (previousPage.Name != L"ProteinModeler.AddProteinPage")
                    AddContentFrame().Navigate(winrt::xaml_typename<ProteinModeler::AddProteinPage>(), winrt::box_value<int64_t>(reinterpret_cast<int64_t>(m_modelerMain)));
            }
        }
	}
}

