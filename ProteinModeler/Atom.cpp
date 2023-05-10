#include "pch.h"
#include "Atom.h"
#include "Atom.g.cpp"

namespace winrt::ProteinModeler::implementation
{
    Atom::Atom(winrt::ProteinModeler::ElementType const& type, winrt::ProteinModeler::Float3 const& position, winrt::ProteinModeler::Float3 const& velocity) :
        m_type(type),
        m_position(position),
        m_velocity(velocity)
    {
    }

    winrt::ProteinModeler::ElementType Atom::Type()
    {
        return m_type;
    }
    void Atom::Type(winrt::ProteinModeler::ElementType const& value)
    {
        if (m_type != value)
        {
            m_type = value;
            m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Type" });
        }
    }

    winrt::ProteinModeler::Float3 Atom::Position()
    {
        return m_position;
    }
    void Atom::Position(winrt::ProteinModeler::Float3 const& value)
    {
        if (m_position != value)
        {
            m_position = value;
            m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Position" });
        }
    }
    winrt::ProteinModeler::Float3 Atom::Velocity()
    {
        return m_velocity;
    }
    void Atom::Velocity(winrt::ProteinModeler::Float3 const& value)
    {
        if (m_velocity != value)
        {
            m_velocity = value;
            m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Velocity" });
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
