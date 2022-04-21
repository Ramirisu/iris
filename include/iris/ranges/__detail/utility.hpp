#pragma once

#include <iris/config.hpp>

#include <ranges>

namespace iris::ranges::__detail {

// clang-format off
template <typename Range>
concept __simple_view = std::ranges::view<Range> 
    && std::ranges::range<Range> 
    && std::same_as<
        std::ranges::iterator_t<Range>, 
        std::ranges::iterator_t<const Range>> 
    && std::same_as<
        std::ranges::sentinel_t<Range>, 
        std::ranges::sentinel_t<const Range>>;
// clang-format on

template <typename T>
using __with_reference = T&;

template <typename T>
concept __can_reference = requires
{
    typename __with_reference<T>;
};

template <bool IsConst, typename T>
using __maybe_const = std::conditional_t<IsConst, const T, T>;

template <typename... Ts>
struct __tuple_or_pair : std::type_identity<std::tuple<Ts...>> {
};

template <typename T, typename U>
struct __tuple_or_pair<T, U> : std::type_identity<std::pair<T, U>> {
};

template <typename... Ts>
using __tuple_or_pair_t = typename __tuple_or_pair<Ts...>::type;

template <typename F, typename Tuple>
constexpr auto __tuple_transform(F&& f, Tuple&& tuple)
{
    return std::apply(
        [&]<class... Ts>(Ts && ... elements) {
            return __tuple_or_pair_t<std::invoke_result_t<F&, Ts>...>(
                std::invoke(f, std::forward<Ts>(elements))...);
        },
        std::forward<Tuple>(tuple));
}

template <typename T, std::size_t N>
struct __repeat_n_helper {
    using type = T;
};

template <template <typename...> class Template,
          typename T,
          std::size_t N,
          typename = std::make_index_sequence<N>>
    requires(N > 0)
struct __repeat_n;

template <template <typename...> class Template,
          typename T,
          std::size_t N,
          std::size_t... Is>
struct __repeat_n<Template, T, N, std::index_sequence<Is...>> {
    using type = Template<typename __repeat_n_helper<T, Is>::type...>;
};

template <template <typename...> class Template, typename T, std::size_t N>
    requires(N > 0)
using __repeat_n_t = typename __repeat_n<Template, T, N>::type;

}