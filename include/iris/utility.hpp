#pragma once

#include <iris/config.hpp>

#include <type_traits>

namespace iris {

template <typename Enum>
constexpr auto to_underlying(Enum e) noexcept
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}

#define IRIS_BITWISE_OP(TYPE)                                                  \
    inline TYPE operator&(const TYPE& lhs, const TYPE& rhs) noexcept           \
    {                                                                          \
        return static_cast<TYPE>(to_underlying(lhs) & to_underlying(rhs));     \
    }                                                                          \
    inline TYPE operator|(const TYPE& lhs, const TYPE& rhs) noexcept           \
    {                                                                          \
        return static_cast<TYPE>(to_underlying(lhs) | to_underlying(rhs));     \
    }                                                                          \
    inline TYPE operator^(const TYPE& lhs, const TYPE& rhs) noexcept           \
    {                                                                          \
        return static_cast<TYPE>(to_underlying(lhs) ^ to_underlying(rhs));     \
    }                                                                          \
    inline TYPE operator&=(TYPE& lhs, const TYPE& rhs) noexcept                \
    {                                                                          \
        return lhs = lhs & rhs;                                                \
    }                                                                          \
    inline TYPE operator|=(TYPE& lhs, const TYPE& rhs) noexcept                \
    {                                                                          \
        return lhs = lhs | rhs;                                                \
    }                                                                          \
    inline TYPE operator^=(TYPE& lhs, const TYPE& rhs) noexcept                \
    {                                                                          \
        return lhs = lhs ^ rhs;                                                \
    }

}
