#pragma once

#include <iris/config.hpp>

#include <iris/type_traits.hpp>

#include <algorithm>
#include <ranges>

namespace iris::ranges {
namespace __to_detail {

    template <typename C>
    concept container_reservable = std::ranges::sized_range<C> && requires(
        C& c, std::ranges::range_size_t<C> n)
    {
        c.reserve(n);
        // clang-format off
        { c.capacity() } -> std::same_as<std::ranges::range_size_t<C>>;
        { c.max_size() } -> std::same_as<std::ranges::range_size_t<C>>;
        // clang-format on
    };

    template <typename C, typename Ref>
    concept container_insertable = requires(C& c, Ref&& ref)
    {
        c.push_back(std::forward<Ref>(ref));
        c.insert(std::ranges::end(c), std::forward<Ref>(ref));
    };

    template <typename Ref, typename C>
    auto container_inserter(C& c)
    {
        if constexpr (requires { c.push_back(std::declval<Ref>()); }) {
            return std::back_inserter(c);
        } else {
            return std::inserter(c, std::ranges::end(c));
        }
    }
}

template <typename Container, std::ranges::input_range Range, typename... Args>
    requires(!std::ranges::view<Container>)
constexpr auto to(Range&& range, Args&&... args)
{
    if constexpr (std::is_convertible_v<
                      std::ranges::range_reference_t<Range>,
                      std::ranges::range_value_t<Container>>) {
        if constexpr (std::constructible_from<Container, Range, Args...>) {
            return Container(std::forward<Range>(range),
                             std::forward<Args>(args)...);
            // clang-format off
        } else if constexpr (std::ranges::common_range<Range> 
            && std::constructible_from<
                Container, 
                std::ranges::iterator_t<Range>, 
                std::ranges::sentinel_t<Range>, Args...>) {
            // clang-format on
            return Container(std::ranges::begin(range), std::ranges::end(range),
                             std::forward<Args>(args)...);
        } else if constexpr (__to_detail::container_insertable<
                                 Container,
                                 std::ranges::range_reference_t<Range>>) {
            Container container(std::forward<Args>(args)...);
            if constexpr (
                std::ranges::sized_range<
                    Range> && __to_detail::container_reservable<Container>) {
                container.reserve(std::ranges::size(range));
            }
            std::ranges::copy(
                range,
                __to_detail::container_inserter<
                    std::ranges::range_reference_t<Range>>(container));
            return container;
        } else {
            static_assert(always_false_v<Container>);
        }
    } else if constexpr (std::ranges::input_range<
                             std::ranges::range_reference_t<Range>>) {
        return to<Container>(
            range | std::views::transform([](auto&& element) {
                return to<std::ranges::range_value_t<Container>>(
                    std::forward<decltype(element)>(element));
            }));
    } else {
        static_assert(always_false_v<Container>);
    }
}

#if defined(_MSC_VER) || defined(__GNUC__) && __GNUC__ >= 11
template <typename Container>
struct __to_fn {
    template <typename Range, typename... Args>
    constexpr auto operator()(Range&& range, Args&&... args) const
    {
        return to<Container>(std::forward<Range>(range),
                             std::forward<Args>(args)...);
    }
};
#endif

template <typename Container, typename... Args>
    requires(!std::ranges::view<Container>)
constexpr auto to(Args&&... args)
{
#if defined(_MSC_VER)
    return std::ranges::_Range_closure<__to_fn<Container>, Args...> {
        std::forward<Args>(args)...
    };
#elif defined(__GNUC__) && __GNUC__ >= 11
    return std::ranges::views::__adaptor::_Partial<__to_fn<Container>, Args...>(
        std::forward<Args>(args)...);
#elif defined(__GNUC__) && __GNUC__ == 10
    return std::ranges::views::__adaptor::_RangeAdaptorClosure(
        []<typename Range>(Range&& range, auto&&... args) {
            return to<Container>(std::forward<Range>(range),
                                 std::forward<decltype(args)>(args)...);
        });
#endif
}

}
