#include "pch.h"
#include "Atom.h"
#include "Atom.g.cpp"

namespace winrt::ProteinModeler::implementation
{
    Atom::Atom(winrt::hstring const& name) : m_name{ name }
    {
    }

    winrt::hstring Atom::Name()
    {
        return m_name;
    }

    void Atom::Name(winrt::hstring const& value)
    {
        if (m_name != value)
        {
            m_name = value;
            m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Name" });
        }
    }

    winrt::event_token Atom::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
        return m_propertyChanged.add(handler);
    }

    void Atom::PropertyChanged(winrt::event_token const& token) noexcept
    {
        m_propertyChanged.remove(token);
    }
}
