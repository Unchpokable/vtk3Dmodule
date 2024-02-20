#pragma once

#include <type_traits>

enum class PropertyMode
{
    Get,
    GetSet
};

/**
 * \brief Implements an a High-Level Properties behaviour with value setting validation. Allows to avoid using default get-set methods for classes
 * \tparam T Type of target field
 * \tparam Mode Property mode - Getter-Setter or Getter-only. When set to `PropertyMode::Get`, `=` operator will not be compiled for the instance
 */
template<typename T, PropertyMode Mode = PropertyMode::GetSet>
class Property
{
public:
    using Validator = std::function<bool(const T&)>;

    Property(T& field, Validator filter) noexcept
        : _fieldProxy(field), _filter(std::move(filter))
    { }

    template<PropertyMode _Mode = Mode, std::enable_if_t<_Mode == PropertyMode::Get, bool> = true>
    explicit Property(T& field) : _fieldProxy(field) { }

    Property(const Property&) = delete;
    Property(Property&& rv) noexcept
        : _fieldProxy(rv._fieldProxy), _filter(rv._filter)
    { }

    ~Property() = default;

    Property& operator=(const Property&) = delete;
    Property& operator=(Property&& rv) noexcept
    {
        _fieldProxy = rv._fieldProxy;
        _filter = rv._filter;
        return *this;
    }

    template<PropertyMode _Mode = Mode>
    std::enable_if_t<_Mode == PropertyMode::GetSet, Property&> operator=(const T& value)
    {
        if(_filter(value))
            _fieldProxy = value;
        else
            throw std::exception("Invalid field assignment");
        return *this;
    }

    operator T() const {
        return _fieldProxy;
    }

private:
    T& _fieldProxy;
    Validator _filter;
};