#pragma once
#include "ElementTypeFormatter.g.h"


namespace winrt::ProteinModeler::implementation
{
    struct ElementTypeFormatter : ElementTypeFormatterT<ElementTypeFormatter>
    {
        ElementTypeFormatter() = default;

        winrt::Windows::Foundation::IInspectable Convert(winrt::Windows::Foundation::IInspectable const& value, winrt::Windows::UI::Xaml::Interop::TypeName const& targetType, winrt::Windows::Foundation::IInspectable const& parameter, hstring const& language);
        winrt::Windows::Foundation::IInspectable ConvertBack(winrt::Windows::Foundation::IInspectable const& value, winrt::Windows::UI::Xaml::Interop::TypeName const& targetType, winrt::Windows::Foundation::IInspectable const& parameter, hstring const& language);
    };
}
namespace winrt::ProteinModeler::factory_implementation
{
    struct ElementTypeFormatter : ElementTypeFormatterT<ElementTypeFormatter, implementation::ElementTypeFormatter>
    {
    };
}
