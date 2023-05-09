#pragma once
#include "AtomViewModel.g.h"
#include "Atom.h"

namespace winrt::ProteinModeler::implementation
{
    struct AtomViewModel : AtomViewModelT<AtomViewModel>
    {
        AtomViewModel();

        winrt::Windows::Foundation::Collections::IObservableVector<winrt::ProteinModeler::Atom> Atoms();

    private:
        Windows::Foundation::Collections::IObservableVector<ProteinModeler::Atom> m_atoms;
    };
}
namespace winrt::ProteinModeler::factory_implementation
{
    struct AtomViewModel : AtomViewModelT<AtomViewModel, implementation::AtomViewModel>
    {
    };
}
