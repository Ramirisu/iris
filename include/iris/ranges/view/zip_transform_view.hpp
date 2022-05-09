#pragma once

#include <iris/config.hpp>

#include <iris/ranges/__detail/copyable_box.hpp>
#include <iris/ranges/__detail/utility.hpp>
#include <iris/ranges/view/zip_view.hpp>
#include <iris/type_traits.hpp>

namespace iris::ranges {
namespace __zip_transform_view_detail {
    template <typename Fn, typename Tuple, std::size_t... Is>
    constexpr auto
    __tuple_invoke(Fn&& fn, Tuple&& t, std::index_sequence<Is...>) //
        noexcept(noexcept(std::invoke(
            std::forward<Fn>(fn), *std::get<Is>(std::forward<Tuple>(t))...)))
    {
        return std::invoke(std::forward<Fn>(fn),
                           *std::get<Is>(std::forward<Tuple>(t))...);
    }
}

template <std::copy_constructible Fn, std::ranges::input_range... Views>
    // clang-format off
    requires (std::ranges::view<Views> && ...) 
        && (pack_size_v<Views...> > 0)
        && std::is_object_v<Fn> 
        && std::regular_invocable<Fn&, std::ranges::range_reference_t<Views>...> 
        && __detail::__can_reference<
            std::invoke_result_t<Fn&, std::ranges::range_reference_t<Views>...>>
class zip_transform_view
    // clang-format on
    : public std::ranges::view_interface<zip_transform_view<Fn, Views...>> {
    using InnerView = zip_view<Views...>;
    template <bool Const>
    using InnerIterator
        = std::ranges::iterator_t<__detail::__maybe_const<Const, InnerView>>;
    template <bool Const>
    using InnerSentinel
        = std::ranges::sentinel_t<__detail::__maybe_const<Const, InnerView>>;

public:
    template <bool Const, bool BaseIsForwardRange>
    class iterator_base {
    };

    template <bool Const>
    class iterator_base<Const, true> {
    public:
        // clang-format off
        using iterator_category = std::conditional_t<
            !std::is_lvalue_reference_v<
                std::invoke_result_t<
                    __detail::__maybe_const<Const, Fn>&, 
                    std::ranges::range_reference_t<__detail::__maybe_const<Const, Views>>...>>, 
            std::input_iterator_tag,
            std::conditional_t<
            (std::derived_from<
                typename std::iterator_traits<
                    std::ranges::iterator_t<__detail::__maybe_const<Const, Views>>
                >::iterator_category, std::random_access_iterator_tag> && ...), 
            std::random_access_iterator_tag,
            std::conditional_t<
            (std::derived_from<
                typename std::iterator_traits<
                    std::ranges::iterator_t<__detail::__maybe_const<Const, Views>>
                >::iterator_category, std::bidirectional_iterator_tag> && ...), 
            std::bidirectional_iterator_tag, 
            std::conditional_t<
            (std::derived_from<
                typename std::iterator_traits<
                    std::ranges::iterator_t<__detail::__maybe_const<Const, Views>>
                >::iterator_category, std::forward_iterator_tag> && ...), 
            std::forward_iterator_tag, std::input_iterator_tag>>>>;
        // clang-format on
    };

    template <bool Const>
    class iterator
        : public iterator_base<Const,
                               std::ranges::forward_range<
                                   __detail::__maybe_const<Const, InnerView>>> {
        friend class zip_transform_view;

        using Parent = __detail::__maybe_const<Const, zip_transform_view>;
        using Base = __detail::__maybe_const<Const, InnerView>;

    public:
        using iterator_concept =
            typename InnerIterator<Const>::iterator_concept;
        using value_type = std::remove_cvref_t<std::invoke_result_t<
            __detail::__maybe_const<Const, Fn>&,
            std::ranges::range_reference_t<
                __detail::__maybe_const<Const, Views>>...>>;
        using difference_type = std::ranges::range_difference_t<Base>;

        iterator() = default;

        constexpr iterator(iterator<!Const> other) requires(
            Const&&
                std::convertible_to<InnerIterator<false>, InnerIterator<Const>>)
            : parent_(other.parent_)
            , inner_iter_(std::move(other.inner_iter_))
        {
        }

        constexpr decltype(auto) operator*() const
            noexcept(noexcept(__zip_transform_view_detail::__tuple_invoke(
                *parent_->fn_,
                inner_iter_.__current(),
                std::index_sequence_for<Views...> {})))
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

        constexpr decltype(auto) operator++(int)
        {
            if constexpr (std::ranges::forward_range<Base>) {
                auto tmp = *this;
                ++*this;
                return tmp;
            } else {
                ++*this;
            }
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
                [&]<class... Is>(const Is&... iters)->decltype(auto) {
                    return std::invoke(
                        *parent_->fn_,
                        iters[std::iter_difference_t<Is>(offset)]...);
                },
                inner_iter_.__current());
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs) //
            requires std::equality_comparable<InnerIterator<Const>>
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
                Base> && std::three_way_comparable<InnerIterator<Const>>
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
            requires
            std::sized_sentinel_for<InnerIterator<Const>, InnerIterator<Const>>
        {
            return lhs.inner_iter_ - rhs.inner_iter_;
        }

        constexpr const auto& __inner() const
        {
            return inner_iter_;
        }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
        void __placeholder();
#endif

    private:
        constexpr iterator(Parent& parent, InnerIterator<Const> inner_iter)
            : parent_(std::addressof(parent))
            , inner_iter_(std::move(inner_iter))
        {
        }

        Parent* parent_ {};
        InnerIterator<Const> inner_iter_;
    };

    template <bool Const>
    class sentinel {
        friend class zip_transform_view;

    public:
        sentinel() = default;

        constexpr sentinel(sentinel<!Const> other) requires Const
            && std::convertible_to<InnerSentinel<false>, InnerSentinel<Const>>
            : inner_iter_(std::move(other.inner_iter_))
        {
        }

        template <bool OtherConst>
            requires std::sentinel_for<InnerSentinel<Const>,
                                       InnerIterator<OtherConst>>
        friend constexpr bool operator==(const iterator<OtherConst>& lhs,
                                         const sentinel& rhs)
        {
            return lhs.__inner() == rhs.inner_iter_;
        }

        template <bool OtherConst>
            requires std::sized_sentinel_for<InnerSentinel<Const>,
                                             InnerIterator<OtherConst>>
        friend constexpr std::ranges::range_difference_t<
            __detail::__maybe_const<OtherConst, InnerView>>
        operator-(const iterator<OtherConst>& lhs, const sentinel& rhs)
        {
            return lhs.inner_iter_ - rhs.inner_iter_;
        }

        template <bool OtherConst>
            requires std::sized_sentinel_for<InnerSentinel<Const>,
                                             InnerIterator<OtherConst>>
        friend constexpr std::ranges::range_difference_t<
            __detail::__maybe_const<OtherConst, InnerView>>
        operator-(const sentinel& lhs, const iterator<OtherConst>& rhs)
        {
            return lhs.inner_iter_ - rhs.inner_iter_;
        }

    private:
        constexpr explicit sentinel(InnerSentinel<Const> inner_iter)
            : inner_iter_(inner_iter)
        {
        }

        InnerSentinel<Const> inner_iter_;
    };

    zip_transform_view() = default;

    constexpr explicit zip_transform_view(Fn fn, Views... bases)
        : fn_(std::in_place, std::move(fn))
        , inner_(std::move(bases)...)
    {
    }

    constexpr auto begin()
    {
        return iterator<false>(*this, inner_.begin());
    }

    constexpr auto begin() const //
        requires std::ranges::range<const InnerView> && std::regular_invocable<
            const Fn&,
            std::ranges::range_reference_t<const Views>...>
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
        requires std::ranges::range<const InnerView> && std::regular_invocable<
            const Fn&,
            std::ranges::range_reference_t<const Views>...>
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
    __detail::__copyable_box<Fn> fn_;
    zip_view<Views...> inner_;
};

template <typename Fn, typename... Ranges>
zip_transform_view(Fn, Ranges&&...)
    -> zip_transform_view<Fn, std::views::all_t<Ranges>...>;

namespace views {
    class __zip_transform_fn {
    public:
        // clang-format off
        template <typename Fn>
            requires std::copy_constructible<std::decay_t<Fn>> 
                && std::regular_invocable<std::decay_t<Fn>&> 
                && std::is_object_v<std::invoke_result_t<std::decay_t<Fn>&>>
            // clang-format on
            constexpr auto operator()(Fn&& fn) const
        {
            IRIS_UNUSED(fn);
            return std::views::empty<
                std::decay_t<std::invoke_result_t<std::decay_t<Fn>&>>>;
        }

        template <typename Fn, std::ranges::viewable_range... Ranges>
            requires(pack_size_v<Ranges...> > 0)
        constexpr auto operator()(Fn&& fn, Ranges&&... ranges) const
        {
            return zip_transform_view<Fn, std::views::all_t<Ranges&&>...>(
                std::forward<Fn>(fn), std::forward<Ranges>(ranges)...);
        }
    };

    inline constexpr __zip_transform_fn zip_transform {};
}
}

namespace iris {
namespace views = ranges::views;
}

namespace std::ranges {
template <class... Views>
inline constexpr bool enable_borrowed_range<iris::ranges::zip_transform_view<
    Views...>> = (enable_borrowed_range<Views> && ...);
}
