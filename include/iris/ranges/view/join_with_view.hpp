#pragma once

#include <iris/config.hpp>

#include <iris/ranges/__detail/non_propagating_cache.hpp>
#include <iris/ranges/__detail/utility.hpp>
#include <iris/ranges/range_adaptor_closure.hpp>

#include <variant>

namespace iris::ranges {
namespace __join_with_view_detail {
    // clang-format off
    template <typename Range, typename Pattern>
    concept __compatible_joinable_ranges = 
        std::common_with<
            std::ranges::range_value_t<Range>, 
            std::ranges::range_value_t<Pattern>> 
        && std::common_reference_with<
            std::ranges::range_reference_t<Range>,
            std::ranges::range_reference_t<Pattern>> 
        && std::common_reference_with<
            std::ranges::range_rvalue_reference_t<Range>,
            std::ranges::range_rvalue_reference_t<Pattern>>;
    // clang-format on

    template <typename Range>
    concept __bidirectional_common = std::ranges::bidirectional_range<
        Range> && std::ranges::common_range<Range>;
}

template <std::ranges::input_range View, std::ranges::forward_range Pattern>
    requires std::ranges::view<View> && std::ranges::input_range<
        std::ranges::range_reference_t<View>> && std::ranges::
        view<Pattern> && __join_with_view_detail::__compatible_joinable_ranges<
            std::ranges::range_reference_t<View>,
            Pattern>
class join_with_view;

template <typename View, typename Pattern>
class join_with_view_base
    : public std::ranges::view_interface<join_with_view<View, Pattern>> {
public:
    __detail::__non_propagating_cache<
        std::remove_cv_t<std::ranges::range_reference_t<View>>>
        inner_;
};

template <typename View, typename Pattern>
    requires std::is_reference_v<std::ranges::range_reference_t<View>>
class join_with_view_base<View, Pattern>
    : public std::ranges::view_interface<join_with_view<View, Pattern>> {
};

template <std::ranges::input_range View, std::ranges::forward_range Pattern>
    requires std::ranges::view<View> && std::ranges::input_range<
        std::ranges::range_reference_t<View>> && std::ranges::
        view<Pattern> && __join_with_view_detail::__compatible_joinable_ranges<
            std::ranges::range_reference_t<View>,
            Pattern>
class join_with_view : public join_with_view_base<View, Pattern> {
    using InnerRange = std::ranges::range_reference_t<View>;

public:
    template <typename Outer,
              typename Inner,
              typename Ptn,
              bool IsRefGlvalue = false>
    class iterator_base {
    };

    template <std::ranges::forward_range Outer,
              std::ranges::forward_range Inner,
              typename Ptn>
    class iterator_base<Outer, Inner, Ptn, true> {
        using OuterC = typename std::iterator_traits<
            std::ranges::iterator_t<Outer>>::iterator_category;
        using InnerC = typename std::iterator_traits<
            std::ranges::iterator_t<Inner>>::iterator_category;
        using PtnC = typename std::iterator_traits<
            std::ranges::iterator_t<Ptn>>::iterator_category;
        static constexpr bool not_only_input
            = std::is_lvalue_reference_v<std::common_reference_t<
                std::iter_reference_t<std::ranges::iterator_t<Inner>>,
                std::iter_reference_t<std::ranges::iterator_t<Ptn>>>>;
        // clang-format off
        static constexpr bool can_be_bidirectional
            = std::derived_from<OuterC, std::bidirectional_iterator_tag>
                && std::derived_from<InnerC, std::bidirectional_iterator_tag> 
                && std::derived_from<PtnC, std::bidirectional_iterator_tag>
                && std::ranges::common_range<Inner>
                && std::ranges::common_range<Ptn>;
        static constexpr bool can_be_forward
            = std::derived_from<OuterC, std::forward_iterator_tag>
                && std::derived_from<InnerC, std::forward_iterator_tag> 
                && std::derived_from<PtnC, std::forward_iterator_tag>
                && std::ranges::common_range<Inner>
                && std::ranges::common_range<Ptn>;
        // clang-format on
    public:
        // clang-format off
        using iterator_category = std::conditional_t<
            not_only_input,  
            std::conditional_t<
                can_be_bidirectional, 
                std::bidirectional_iterator_tag, 
                std::conditional_t<
                    can_be_forward,
                    std::forward_iterator_tag, 
                    std::input_iterator_tag>>,
            std::input_iterator_tag>;
        // clang-format on
    };

    template <bool IsConst>
        requires std::ranges::view<View> && std::ranges::input_range<
            std::ranges::range_reference_t<View>> && std::ranges::
            view<Pattern> && __join_with_view_detail::
                __compatible_joinable_ranges<
                    std::ranges::range_reference_t<View>,
                    Pattern>
    class iterator : public iterator_base<
                         __detail::__maybe_const<IsConst, View>,
                         std::ranges::range_reference_t<
                             __detail::__maybe_const<IsConst, View>>,
                         __detail::__maybe_const<IsConst, Pattern>,
                         std::is_reference_v<std::ranges::range_reference_t<
                             __detail::__maybe_const<IsConst, View>>>> {
        friend class join_with_view;

        using Parent = __detail::__maybe_const<IsConst, join_with_view>;
        using Base = __detail::__maybe_const<IsConst, View>;
        using InnerBase = std::ranges::range_reference_t<Base>;
        using PatternBase = __detail::__maybe_const<IsConst, Pattern>;

        using OuterIter = std::ranges::iterator_t<Base>;
        using InnerIter = std::ranges::iterator_t<InnerBase>;
        using PatternIter = std::ranges::iterator_t<PatternBase>;

        static constexpr bool ref_is_glvalue = std::is_reference_v<InnerBase>;

        constexpr iterator(Parent& parent,
                           std::ranges::iterator_t<View> outer_it)
            : parent_(std::addressof(parent))
            , outer_it_(std::move(outer_it))
        {
            if (outer_it_ != std::ranges::end(parent_->base_)) {
                auto&& inner = update_inner(outer_it_);
                inner_it_.template emplace<1>(std::ranges::begin(inner));
                satisfy();
            }
        }

        constexpr auto&& update_inner(const OuterIter& outer)
        {
            if constexpr (ref_is_glvalue) {
                return *outer;
            } else {
                return parent_->inner_.emplace(*outer);
            }
        }

        constexpr auto&& get_inner(const OuterIter& outer)
        {
            if constexpr (ref_is_glvalue) {
                return *outer;
            } else {
                return *parent_->inner_;
            }
        }

        constexpr void satisfy()
        {
            while (true) {
                if (inner_it_.index() == 0) {
                    if (std::get<0>(inner_it_)
                        != std::ranges::end(parent_->pattern_)) {
                        break;
                    }

                    auto&& inner = update_inner(outer_it_);
                    inner_it_.template emplace<1>(std::ranges::begin(inner));
                } else {
                    auto&& inner = get_inner(outer_it_);
                    if (std::get<1>(inner_it_) != std::ranges::end(inner)) {
                        break;
                    }

                    if (++outer_it_ == std::ranges::end(parent_->base_)) {
                        if constexpr (ref_is_glvalue) {
                            inner_it_.template emplace<0>();
                        }
                        break;
                    }

                    inner_it_.template emplace<0>(
                        std::ranges::begin(parent_->pattern_));
                }
            }
        }

    public:
        // clang-format off
        using iterator_concept = std::conditional_t<
            ref_is_glvalue
                && std::ranges::bidirectional_range<Base> 
                && __join_with_view_detail::__bidirectional_common<InnerBase> 
                && __join_with_view_detail::__bidirectional_common<PatternBase>,
            std::bidirectional_iterator_tag,
            std::conditional_t<
                ref_is_glvalue
                    && std::ranges::forward_range<Base> 
                    && std::ranges::forward_range<InnerBase>,
                std::forward_iterator_tag,
                std::input_iterator_tag>>;
        // clang-format on
        using value_type = std::common_type_t<std::iter_value_t<InnerIter>,
                                              std::iter_value_t<PatternIter>>;
        using difference_type
            = std::common_type_t<std::iter_difference_t<OuterIter>,
                                 std::iter_difference_t<InnerIter>,
                                 std::iter_difference_t<PatternIter>>;

        // clang-format off
        iterator() 
            requires std::default_initializable<OuterIter> = default;
        // clang-format on

        // clang-format off
        constexpr iterator(iterator<!IsConst> other) 
            requires IsConst
                && std::convertible_to<std::ranges::iterator_t<View>, OuterIter> 
                && std::convertible_to<std::ranges::iterator_t<InnerRange>, InnerIter> 
                && std::convertible_to<std::ranges::iterator_t<Pattern>, PatternIter>
            : outer_it_(std::move(other.outer_it_))
            , parent_(other.parent_)
        // clang-format on
        {
            if (other.inner_it_.index() == 0) {
                inner_it_.emplace<0>(std::get<0>(std::move(other.inner_it_)));
            } else {
                inner_it_.emplace<1>(std::get<1>(std::move(other.inner_it_)));
            }
        }

        constexpr decltype(auto) operator*() const
        {
            using reference
                = std::common_reference_t<std::iter_reference_t<InnerIter>,
                                          std::iter_reference_t<PatternIter>>;
            return std::visit([](auto& it) -> reference { return *it; },
                              inner_it_);
        }

        constexpr iterator& operator++()
        {
            std::visit([](auto& it) { ++it; }, inner_it_);
            satisfy();
            return *this;
        }

        constexpr void operator++(int)
        {
            ++*this;
        }

        constexpr iterator operator++(int) requires ref_is_glvalue
            && std::forward_iterator<OuterIter> && std::forward_iterator<
                InnerIter>
        {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }

        constexpr iterator& operator--()
            // clang-format off
            requires ref_is_glvalue
                && std::ranges::bidirectional_range<Base> 
                && __join_with_view_detail::__bidirectional_common<InnerBase> 
                && __join_with_view_detail::__bidirectional_common<PatternBase>
        // clang-format on
        {
            if (outer_it_ == std::ranges::end(parent_->base_)) {
                auto&& inner = *--outer_it_;
                inner_it_.emplace<1>(std::ranges::end(inner));
            }

            while (true) {
                if (inner_it_.index() == 0) {
                    auto& it = std::get<0>(inner_it_);
                    if (it == std::ranges::begin(parent_->pattern_)) {
                        auto&& inner = *--outer_it_;
                        inner_it_.emplace<1>(std::ranges::end(inner));
                    } else {
                        break;
                    }
                } else {
                    auto& it = std::get<1>(inner_it_);
                    auto&& inner = *outer_it_;
                    if (it == std::ranges::begin(inner)) {
                        inner_it_.emplace<0>(
                            std::ranges::end(parent_->pattern_));
                    } else {
                        break;
                    }
                }
            }

            std::visit([](auto& it) { --it; }, inner_it_);
            return *this;
        }

        constexpr iterator operator--(int)
            // clang-format off
            requires ref_is_glvalue
                && std::ranges::bidirectional_range<Base> 
                && __join_with_view_detail::__bidirectional_common<InnerBase> 
                && __join_with_view_detail::__bidirectional_common<PatternBase>
        // clang-format on
        {
            iterator tmp = *this;
            --*this;
            return tmp;
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs)
            // clang-format off
            requires ref_is_glvalue
                && std::equality_comparable<OuterIter> 
                && std::equality_comparable<InnerIter>
        // clang-format on
        {
            return lhs.outer_it_ == rhs.outer_it_
                && lhs.inner_it_ == rhs.inner_it_;
        }

        friend constexpr decltype(auto) iter_move(const iterator& rhs)
        {
            using rvalue_reference = std::common_reference_t<
                std::iter_rvalue_reference_t<InnerIter>,
                std::iter_rvalue_reference_t<PatternIter>>;
            return std::visit<rvalue_reference>(std::ranges::iter_move,
                                                rhs.inner_it_);
        }

        friend constexpr void iter_swap(const iterator& lhs,
                                        const iterator& rhs) requires
            std::indirectly_swappable<InnerIter, PatternIter>
        {
            std::visit(std::ranges::iter_swap, lhs.inner_it_, rhs.inner_it_);
        }

        const OuterIter& __outer_it() const noexcept
        {
            return outer_it_;
        }

        // clang-format off
    private:
        // clang-format on
        Parent* parent_ {};
        OuterIter outer_it_ {};
        std::variant<PatternIter, InnerIter> inner_it_ {};
    };

    template <bool IsConst>
    class sentinel {
        friend class join_with_view;

        using Parent = __detail::__maybe_const<IsConst, join_with_view>;
        using Base = __detail::__maybe_const<IsConst, View>;

        constexpr explicit sentinel(Parent& parent)
            : end_(std::ranges::end(parent.base_))
        {
        }

    public:
        sentinel() = default;

        constexpr sentinel(sentinel<!IsConst> other)
            // clang-format off
            requires IsConst
                && std::convertible_to<
                    std::ranges::sentinel_t<View>,
                    std::ranges::sentinel_t<Base>>
            : end_(std::move(other.end_))
        // clang-format on
        {
        }

        template <bool OtherIsConst>
            // clang-format off
            requires std::sentinel_for<
                std::ranges::sentinel_t<Base>,
                std::ranges::iterator_t<__detail::__maybe_const<OtherIsConst, View>>>
        // clang-format on
        friend constexpr bool operator==(const iterator<OtherIsConst>& lhs,
                                         const sentinel& rhs)
        {
            return lhs.__outer_it() == rhs.end_;
        }

    private:
        std::ranges::sentinel_t<Base> end_ {};
    };

    // clang-format off
    join_with_view() 
        requires std::default_initializable<View> 
            && std::default_initializable<Pattern> = default;
    // clang-format on

    constexpr join_with_view(View base, Pattern pattern)
        : base_(std::move(base))
        , pattern_(std::move(pattern))
    {
    }

    // clang-format off
    template <std::ranges::input_range Range>
        requires std::constructible_from<View, std::views::all_t<Range>> 
            && std::constructible_from<Pattern, std::ranges::single_view<InnerRange>>
    // clang-format on
    constexpr join_with_view(Range&& range,
                             std::ranges::range_value_t<InnerRange> element)
        : base_(std::views::all(std::forward<Range>(range)))
        , pattern_(std::views::single(std::move(element)))
    {
    }

    constexpr View base() const& requires std::copy_constructible<View>
    {
        return base_;
    }
    constexpr View base() &&
    {
        return std::move(base_);
    }

    constexpr auto begin()
    {
        // clang-format off
        constexpr bool is_const = __detail::__simple_view<View> 
            && std::is_reference_v<InnerRange> 
            && __detail::__simple_view<Pattern>;
        // clang-format on
        return iterator<is_const> { *this, std::ranges::begin(base_) };
    }

    constexpr auto begin() const
        // clang-format off
        requires std::ranges::input_range<const View> 
            && std::ranges::forward_range<const Pattern> 
            && std::is_reference_v<std::ranges::range_reference_t<const View>>
    // clang-format on
    {
        return iterator<true> { *this, std::ranges::begin(base_) };
    }

    constexpr auto end()
    {
        // clang-format off
        if constexpr (std::ranges::forward_range<View> 
            && std::is_reference_v<InnerRange> 
            && std::ranges::forward_range<InnerRange> 
            && std::ranges::common_range<View> 
            && std::ranges::common_range<InnerRange>) {
            return iterator<__detail::__simple_view<View> 
                && __detail::__simple_view<Pattern>> { *this, std::ranges::end(base_) };
        } else {
            return sentinel<__detail::__simple_view<View> 
                && __detail::__simple_view<Pattern>> { *this };
        }
        // clang-format on
    }

    constexpr auto end() const
        // clang-format off
        requires std::ranges::input_range<const View> 
            && std::ranges::forward_range<const Pattern> 
            && std::is_reference_v<std::ranges::range_reference_t<const View>>
    // clang-format on
    {
        using InnerConstRange = std::ranges::range_reference_t<const View>;
        // clang-format off
        if constexpr (std::ranges::forward_range<const View> 
            && std::ranges::forward_range<InnerConstRange> 
            && std::ranges::common_range<const View> 
            && std::ranges::common_range<InnerConstRange>) {
            // clang-format on
            return iterator<true> { *this, std::ranges::end(base_) };
        } else {
            return sentinel<true> { *this };
        }
    }

    // clang-format off
private:
    // clang-format on
    View base_ {};
    Pattern pattern_ {};
};

template <typename View, typename Pattern>
join_with_view(View&&, Pattern&&)
    -> join_with_view<std::views::all_t<View>, std::views::all_t<Pattern>>;

template <std::ranges::input_range Range>
join_with_view(
    Range&&, std::ranges::range_value_t<std::ranges::range_reference_t<Range>>)
    -> join_with_view<std::views::all_t<Range>,
                      std::ranges::single_view<std::ranges::range_value_t<
                          std::ranges::range_reference_t<Range>>>>;

namespace views {
    class __join_with_fn {
    public:
        template <std::ranges::viewable_range Range, typename Pattern>
        constexpr auto operator()(Range&& range, Pattern&& pattern) const
        {
            return join_with_view(std::forward<Range>(range),
                                  std::forward<Pattern>(pattern));
        }

        template <typename Pattern>
            requires std::constructible_from<std::decay_t<Pattern>, Pattern>
        constexpr auto operator()(Pattern&& pattern) const
        {
            return range_adaptor_closure<__join_with_fn,
                                         std::decay_t<Pattern>> {
                std::forward<Pattern>(pattern)
            };
        }
    };

    inline constexpr __join_with_fn join_with {};
}

}

namespace iris {
namespace views = ranges::views;
}
