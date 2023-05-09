#pragma once
#include "Atom.g.h"

namespace winrt::ProteinModeler::implementation
{
    struct Atom : AtomT<Atom>
    {
        Atom() = delete;

        Atom(hstring const& name);
        hstring Name();
        void Name(hstring const& value);
        winrt::event_token PropertyChanged(winrt::Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;

    private:
        winrt::hstring m_name;
        winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    };
}
namespace winrt::ProteinModeler::factory_implementation
{
    struct Atom : AtomT<Atom, implementation::Atom>
    {
    };
}
