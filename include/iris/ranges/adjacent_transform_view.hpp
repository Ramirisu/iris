#pragma once

#include <iris/config.hpp>

#include <iris/ranges/__detail/copyable_box.hpp>
#include <iris/ranges/__detail/utility.hpp>
#include <iris/ranges/adjacent_view.hpp>
#include <iris/ranges/range_adaptor_closure.hpp>
#include <iris/ranges/zip_transform_view.hpp>

namespace iris::ranges {

template <std::ranges::forward_range View,
          std::copy_constructible Fn,
          std::size_t N>
    // clang-format off
    requires std::ranges::view<View> 
        && std::is_object_v<Fn> 
        && __detail::__regular_invocable_repeat_n<
            Fn&,
            std::ranges::range_reference_t<View>,
            N> 
        && __detail::__can_reference<
            __detail::__invoke_result_repeat_n_t<
                Fn&,
                std::ranges::range_reference_t<View>,
                N>>
// clang-format on
class adjacent_transform_view
    : public std::ranges::view_interface<adjacent_transform_view<View, Fn, N>> {
    using InnerView = adjacent_view<View, N>;
    template <bool IsConst>
    using InnerIterator
        = std::ranges::iterator_t<__detail::__maybe_const<IsConst, InnerView>>;
    template <bool IsConst>
    using InnerSentinel
        = std::ranges::sentinel_t<__detail::__maybe_const<IsConst, InnerView>>;

public:
    template <bool IsConst>
    class iterator {
        friend class adjacent_transform_view;

        using Parent
            = __detail::__maybe_const<IsConst,
                                      adjacent_transform_view<View, Fn, N>>;
        using Base = __detail::__maybe_const<IsConst, View>;

    public:
        // clang-format off
        using iterator_category = std::conditional_t<
            !std::is_lvalue_reference_v<
                __detail::__invoke_result_repeat_n_t<
                    __detail::__maybe_const<IsConst, Fn>&, 
                    std::ranges::range_reference_t<Base>, 
                    N>>, 
            std::input_iterator_tag,
            std::conditional_t<
            std::derived_from<
                typename std::iterator_traits<
                    std::ranges::iterator_t<Base>
                >::iterator_category, std::random_access_iterator_tag>, 
            std::random_access_iterator_tag,
            std::conditional_t<
            std::derived_from<
                typename std::iterator_traits<
                    std::ranges::iterator_t<Base>
                >::iterator_category, std::bidirectional_iterator_tag>, 
            std::bidirectional_iterator_tag, 
            std::conditional_t<
            std::derived_from<
                typename std::iterator_traits<
                    std::ranges::iterator_t<Base>
                >::iterator_category, std::forward_iterator_tag>, 
            std::forward_iterator_tag, std::input_iterator_tag>>>>;
        // clang-format on
        using iterator_concept =
            typename InnerIterator<IsConst>::iterator_concept;
        using value_type
            = std::remove_cvref_t<__detail::__invoke_result_repeat_n_t<
                Fn&,
                std::ranges::range_reference_t<View>,
                N>>;
        using difference_type = std::ranges::range_difference_t<Base>;

        iterator() = default;

        constexpr iterator(iterator<!IsConst> other) requires(
            IsConst&& std::convertible_to<InnerIterator<false>,
                                          InnerIterator<IsConst>>)
            : parent_(other.parent_)
            , inner_iter_(std::move(other.inner_iter_))
        {
        }

        constexpr decltype(auto) operator*() const
            noexcept(noexcept(__zip_transform_view_detail::__tuple_invoke(
                *parent_->fn_,
                inner_iter_.__current(),
                std::make_index_sequence<N> {})))
        {
            return std::apply(
                [&](const auto&... iters) -> decltype(auto) {
                    return std::invoke(*parent_->fn_, *iters...);
                },
                inner_iter_.__current());
        }

        constexpr iterator& operator++()
        {
            ++inner_iter_;
            return *this;
        }

        constexpr iterator operator++(int)
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        constexpr iterator& operator--() //
            requires std::ranges::bidirectional_range<Base>
        {
            --inner_iter_;
            return *this;
        }

        constexpr iterator operator--(int) //
            requires std::ranges::bidirectional_range<Base>
        {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        constexpr iterator& operator+=(difference_type offset) //
            requires std::ranges::random_access_range<Base>
        {
            inner_iter_ += offset;
            return *this;
        }

        constexpr iterator& operator-=(difference_type offset) //
            requires std::ranges::random_access_range<Base>
        {
            inner_iter_ -= offset;
            return *this;
        }

        constexpr decltype(auto) operator[](difference_type offset) const //
            requires std::ranges::random_access_range<Base>
        {
            return std::apply(
                [&](const auto&... iters) -> decltype(auto) {
                    return std::invoke(*parent_->fn_, iters[offset]...);
                },
                inner_.__current());
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs)
        {
            return lhs.inner_iter_ == rhs.inner_iter_;
        }

        friend constexpr bool operator<(const iterator& lhs,
                                        const iterator& rhs) //
            requires std::ranges::random_access_range<Base>
        {
            return lhs.inner_iter_ < rhs.inner_iter_;
        }

        friend constexpr bool operator>(const iterator& lhs,
                                        const iterator& rhs) //
            requires std::ranges::random_access_range<Base>
        {
            return lhs.inner_iter_ > rhs.inner_iter_;
        }

        friend constexpr bool operator<=(const iterator& lhs,
                                         const iterator& rhs) //
            requires std::ranges::random_access_range<Base>
        {
            return lhs.inner_iter_ <= rhs.inner_iter_;
        }

        friend constexpr bool operator>=(const iterator& lhs,
                                         const iterator& rhs) //
            requires std::ranges::random_access_range<Base>
        {
            return lhs.inner_iter_ >= rhs.inner_iter_;
        }

        friend constexpr auto operator<=>(const iterator& lhs,
                                          const iterator& rhs) //
            requires std::ranges::random_access_range<
                Base> && std::three_way_comparable<InnerIterator<IsConst>>
        {
            return lhs.inner_iter_ <=> rhs.inner_iter_;
        }

        friend constexpr iterator operator+(const iterator& i,
                                            difference_type offset) //
            requires std::ranges::random_access_range<Base>
        {
            return iterator(*i.parent_, i.inner_iter_ + offset);
        }

        friend constexpr iterator operator+(difference_type offset,
                                            const iterator& i) //
            requires std::ranges::random_access_range<Base>
        {
            return iterator(*i.parent_, i.inner_iter_ + offset);
        }

        friend constexpr iterator operator-(const iterator& i,
                                            difference_type offset) //
            requires std::ranges::random_access_range<Base>
        {
            return iterator(*i.parent_, i.inner_iter_ - offset);
        }

        friend constexpr difference_type operator-(const iterator& lhs,
                                                   const iterator& rhs) //
            requires std::sized_sentinel_for<InnerIterator<IsConst>,
                                             InnerIterator<IsConst>>
        {
            return lhs.inner_iter_ - rhs.inner_iter_;
        }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
        void __placeholder();
#endif

    private:
        constexpr explicit iterator(Parent& parent,
                                    InnerIterator<IsConst> inner_iter)
            : parent_(std::addressof(parent))
            , inner_iter_(std::move(inner_iter))
        {
        }

        Parent* parent_ {};
        InnerIterator<IsConst> inner_iter_;
    };

    template <bool IsConst>
    class sentinel {
        friend class adjacent_transform_view;

    public:
        sentinel() = default;

        constexpr sentinel(sentinel<!IsConst> other) //
            requires(IsConst&& std::convertible_to<InnerSentinel<false>,
                                                   InnerSentinel<IsConst>>)
            : inner_iter_(std::move(other.inner_iter_))
        {
        }

        template <bool OtherIsConst>
            requires std::sentinel_for<InnerSentinel<IsConst>,
                                       InnerIterator<OtherIsConst>>
        friend constexpr bool operator==(const iterator<OtherIsConst>& lhs,
                                         const sentinel& rhs)
        {
            return lhs.inner_iter_ = rhs.inner_iter_;
        }

        template <bool OtherIsConst>
            requires std::sized_sentinel_for<InnerSentinel<IsConst>,
                                             InnerIterator<OtherIsConst>>
        friend constexpr std::ranges::range_difference_t<
            __detail::__maybe_const<OtherIsConst, InnerView>>
        operator-(const iterator<OtherIsConst>& lhs, const sentinel& rhs)
        {
            return lhs.inner_iter_ - rhs.inner_iter_;
        }

        template <bool OtherIsConst>
            requires std::sized_sentinel_for<InnerSentinel<IsConst>,
                                             InnerIterator<OtherIsConst>>
        friend constexpr std::ranges::range_difference_t<
            __detail::__maybe_const<OtherIsConst, InnerView>>
        operator-(const sentinel& lhs, const iterator<OtherIsConst>& rhs)
        {
            return lhs.inner_iter_ - rhs.inner_iter_;
        }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
        void __placeholder();
#endif

    private:
        constexpr explicit sentinel(InnerSentinel<IsConst> inner_iter)
            : inner_iter_(inner_iter)
        {
        }

        InnerSentinel<IsConst> inner_iter_;
    };

    adjacent_transform_view() = default;

    constexpr explicit adjacent_transform_view(View base, Fn fn)
        : inner_(std::move(base))
        , fn_(std::in_place, std::move(fn))
    {
    }

    constexpr auto begin()
    {
        return iterator<false>(*this, inner_.begin());
    }

    constexpr auto begin() const //
        requires std::ranges::range<const InnerView> && __detail::
            __regular_invocable_repeat_n<
                Fn&,
                std::ranges::range_reference_t<const View>,
                N>
    {
        return iterator<true>(*this, inner_.begin());
    }

    constexpr auto end()
    {
        if constexpr (std::ranges::common_range<InnerView>) {
            return iterator<false>(*this, inner_.end());
        } else {
            return sentinel<false>(inner_.end());
        }
    }

    constexpr auto end() const //
        requires std::ranges::range<const InnerView> && __detail::
            __regular_invocable_repeat_n<
                Fn&,
                std::ranges::range_reference_t<const View>,
                N>
    {
        if constexpr (std::ranges::common_range<const InnerView>) {
            return iterator<true>(*this, inner_.end());
        } else {
            return sentinel<true>(inner_.end());
        }
    }

    constexpr auto size() //
        requires std::ranges::sized_range<InnerView>
    {
        return inner_.size();
    }

    constexpr auto size() const //
        requires std::ranges::sized_range<const InnerView>
    {
        return inner_.size();
    }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
    void __placeholder();
#endif

private:
    InnerView inner_;
    __detail::__copyable_box<Fn> fn_;
};

namespace views {
    template <std::size_t N>
    class __adjacent_transform_fn
        : public range_adaptor_closure<__adjacent_transform_fn<N>> {
    public:
        constexpr __adjacent_transform_fn() noexcept = default;

        template <std::ranges::viewable_range Range, typename Fn>
        constexpr auto operator()(Range&& range, Fn&& fn) const
        {
            if constexpr (N == 0) {
                IRIS_UNUSED(range);
                return zip_transform(std::forward<Fn>(fn));
            } else {
                return adjacent_transform_view<std::views::all_t<Range>,
                                               std::decay_t<Fn>, N> {
                    std::forward<Range>(range), std::forward<Fn>(fn)
                };
            }
        }
    };

    template <std::size_t N>
    inline constexpr __adjacent_transform_fn<N> adjacent_transform;

    inline constexpr auto pairwise_transform = adjacent_transform<2>;
}
}

namespace iris {
namespace views = ranges::views;
}
