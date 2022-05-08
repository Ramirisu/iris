#pragma once

#include <iris/config.hpp>

#include <iris/__detail/type_traits.hpp>

namespace iris {

template <typename T, template <typename...> class U>
struct is_specialization_of : std::false_type {
};

template <template <typename...> class U, typename... Args>
struct is_specialization_of<U<Args...>, U> : std::true_type {
};

template <typename T, template <typename...> class U>
inline constexpr bool is_specialization_of_v
    = is_specialization_of<T, U>::value;

template <typename E, bool = std::is_enum_v<E>>
struct is_scoped_enum : std::false_type {
};

template <typename E>
struct is_scoped_enum<E, true>
    : std::bool_constant<!std::is_convertible_v<E, std::underlying_type_t<E>>> {
};

template <typename E>
inline constexpr bool is_scoped_enum_v = is_scoped_enum<E>::value;

template <typename... Ts>
struct pack_size : std::integral_constant<std::size_t, sizeof...(Ts)> {
};

template <typename... Ts>
inline constexpr auto pack_size_v = pack_size<Ts...>::value;

template <std::size_t Index, typename T, typename... Ts>
struct pack_element {
    static_assert(Index < pack_size_v<T, Ts...>);
    using type = typename pack_element<Index - 1, Ts...>::type;
};

template <typename T, typename... Ts>
struct pack_element<0, T, Ts...> {
    using type = T;
};

template <std::size_t Index, typename... Ts>
using pack_element_t = typename pack_element<Index, Ts...>::type;

template <typename... Ts>
struct front_of_pack_element {
    using type = pack_element_t<0, Ts...>;
};

template <typename... Ts>
using front_of_pack_element_t = typename front_of_pack_element<Ts...>::type;

template <typename... Ts>
struct back_of_pack_element {
    using type = pack_element_t<pack_size_v<Ts...> - 1, Ts...>;
};

template <typename... Ts>
using back_of_pack_element_t = typename back_of_pack_element<Ts...>::type;

}
