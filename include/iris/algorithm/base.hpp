#pragma once

#include <iris/config.hpp>

#include <iris/type_traits.hpp>

namespace iris::ranges {

template <class I, class T>
struct in_value_result {
    [[no_unique_address]] I in;
    [[no_unique_address]] T value;
    template <class I2, class T2>
        requires std::convertible_to<const I&,
                                     I2> && std::convertible_to<const T&, T2>
    constexpr operator in_value_result<I2, T2>() const&
    {
        return { in, value };
    }
    template <class I2, class T2>
        requires std::convertible_to<I, I2> && std::convertible_to<T, T2>
    constexpr operator in_value_result<I2, T2>() &&
    {
        return { std::move(in), std::move(value) };
    }
};

template <class O, class T>
struct out_value_result {
    [[no_unique_address]] O out;
    [[no_unique_address]] T value;

    template <class O2, class T2>
        requires std::convertible_to<const O&,
                                     O2> && std::convertible_to<const T&, T2>
    constexpr operator out_value_result<O2, T2>() const&
    {
        return { out, value };
    }

    template <class O2, class T2>
        requires std::convertible_to<O, O2> && std::convertible_to<T, T2>
    constexpr operator out_value_result<O2, T2>() &&
    {
        return { std::move(out), std::move(value) };
    }
};

}
