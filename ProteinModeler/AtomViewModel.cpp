#include "pch.h"
#include "AtomViewModel.h"
#include "AtomViewModel.g.cpp"

namespace winrt::ProteinModeler::implementation
{
    AtomViewModel::AtomViewModel()
    {
        m_atoms = winrt::single_threaded_observable_vector<ProteinModeler::Atom>();
    }

    winrt::Windows::Foundation::Collections::IObservableVector<winrt::ProteinModeler::Atom> AtomViewModel::Atoms()
    {
        return m_atoms;
    }
}
