#pragma once
#include "Atom.g.h"

namespace winrt::ProteinModeler::implementation
{
    struct Atom : AtomT<Atom>
    {
        Atom() = delete;
        Atom(winrt::ProteinModeler::ElementType const& type, winrt::ProteinModeler::Float3 const& position, winrt::ProteinModeler::Float3 const& velocity);
        
        winrt::ProteinModeler::ElementType Type();
        void Type(winrt::ProteinModeler::ElementType const& value);
        winrt::ProteinModeler::Float3 Position();
        void Position(winrt::ProteinModeler::Float3 const& value);
        winrt::ProteinModeler::Float3 Velocity();
        void Velocity(winrt::ProteinModeler::Float3 const& value);

        winrt::event_token PropertyChanged(winrt::Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;

    private:
        winrt::ProteinModeler::ElementType m_type;
        winrt::ProteinModeler::Float3 m_position;
        winrt::ProteinModeler::Float3 m_velocity;

        winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    };
}
namespace winrt::ProteinModeler::factory_implementation
{
    struct Atom : AtomT<Atom, implementation::Atom>
    {
    };
}
