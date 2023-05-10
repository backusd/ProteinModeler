#include "pch.h"
#include "ElementTypeFormatter.h"
#include "ElementTypeFormatter.g.cpp"

using winrt::Windows::Foundation::IInspectable;
using winrt::Windows::UI::Xaml::Interop::TypeName;

using winrt::ProteinModeler::ElementType;

namespace winrt::ProteinModeler::implementation
{
    IInspectable ElementTypeFormatter::Convert(IInspectable const& value, TypeName const& targetType, IInspectable const& parameter, hstring const& language)
    {
        ElementType type = winrt::unbox_value_or<ElementType>(value, ElementType::None);

        switch (type)
        {
        case winrt::ProteinModeler::ElementType::None:      return winrt::box_value(L"None");
        case winrt::ProteinModeler::ElementType::Hydrogen:  return winrt::box_value(L"Hydrogen");
        case winrt::ProteinModeler::ElementType::Helium:    return winrt::box_value(L"Helium");
        case winrt::ProteinModeler::ElementType::Lithium:   return winrt::box_value(L"Lithium");
        case winrt::ProteinModeler::ElementType::Beryllium: return winrt::box_value(L"Beryllium");
        case winrt::ProteinModeler::ElementType::Boron:     return winrt::box_value(L"Boron");
        case winrt::ProteinModeler::ElementType::Carbon:    return winrt::box_value(L"Carbon");
        case winrt::ProteinModeler::ElementType::Nitrogen:  return winrt::box_value(L"Nitrogen");
        case winrt::ProteinModeler::ElementType::Oxygen:    return winrt::box_value(L"Oxygen");
        case winrt::ProteinModeler::ElementType::Flourine:  return winrt::box_value(L"Flourine");
        case winrt::ProteinModeler::ElementType::Neon:      return winrt::box_value(L"Neon");
        default:
            return winrt::box_value(L"Unrecognized");
        }
        return winrt::box_value(L"Unrecognized");
    }
    IInspectable ElementTypeFormatter::ConvertBack(IInspectable const& value, TypeName const& targetType, IInspectable const& parameter, hstring const& language)
    {
        // I don't think we need to implement ConvertBack when its just a one-way binding
        throw hresult_not_implemented();
    }
}
