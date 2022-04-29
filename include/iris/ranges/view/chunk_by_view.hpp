#pragma once

#include <iris/config.hpp>

#include <iris/ranges/__detail/copyable_box.hpp>
#include <iris/ranges/range_adaptor_closure.hpp>

#include <functional>

namespace iris::ranges {

template <std::ranges::forward_range View,
          std::indirect_binary_predicate<std::ranges::iterator_t<View>,
                                         std::ranges::iterator_t<View>> Pred>
    requires std::ranges::view<View> && std::is_object_v<Pred>
class chunk_by_view
    : public std::ranges::view_interface<chunk_by_view<View, Pred>> {
public:
    class iterator {
        friend class chunk_by_view;

    public:
        using iterator_category = std::input_iterator_tag;
        using iterator_concept
            = std::conditional_t<std::ranges::bidirectional_range<View>,
                                 std::bidirectional_iterator_tag,
                                 std::forward_iterator_tag>;
        using value_type = std::ranges::subrange<std::ranges::iterator_t<View>>;
        using difference_type = std::ranges::range_difference_t<View>;

        iterator() = default;

        constexpr value_type operator*() const
        {
            return std::ranges::subrange(current_, next_);
        }

        constexpr iterator& operator++()
        {
            IRIS_ASSERT(current_ != next_);
            current_ = next_;
            next_ = parent_->find_next(current_);
            return *this;
        }

        constexpr iterator operator++(int)
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        constexpr iterator& operator--() //
            requires std::ranges::bidirectional_range<View>
        {
            next_ = current_;
            current_ = parent_->find_prev(next_);
            return *this;
        }

        constexpr iterator operator--(int) //
            requires std::ranges::bidirectional_range<View>
        {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs)
        {
            return lhs.current_ == rhs.current_;
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         std::default_sentinel_t)
        {
            return lhs.current_ == lhs.next_;
        }

    private:
        constexpr iterator(chunk_by_view& parent,
                           std::ranges::iterator_t<View> current,
                           std::ranges::iterator_t<View> next)
            : parent_(std::addressof(parent))
            , current_(current)
            , next_(next)
        {
        }

        chunk_by_view* parent_ {};
        std::ranges::iterator_t<View> current_ {};
        std::ranges::iterator_t<View> next_ {};
    };

    chunk_by_view() requires
        std::default_initializable<View> && std::default_initializable<Pred>
    = default;

    constexpr explicit chunk_by_view(View base, Pred pred)
        : base_(std::move(base))
        , pred_(std::in_place, std::move(pred))
    {
    }

    constexpr View base() const& requires std::copy_constructible<View>
    {
        return base_;
    }

    constexpr View base() && requires std::move_constructible<View>
    {
        return std::move(base_);
    }

    constexpr const Pred& pred() const
    {
        return *pred_;
    }

    constexpr iterator begin()
    {
        IRIS_ASSERT(pred_);
        return iterator { *this, std::ranges::begin(base_),
                          find_next(std::ranges::begin(base_)) };
    }

    constexpr auto end()
    {
        if constexpr (std::ranges::common_range<View>) {
            return iterator { *this, std::ranges::end(base_),
                              std::ranges::end(base_) };
        } else {
            return std::default_sentinel;
        }
    }

private:
    constexpr std::ranges::iterator_t<View>
    find_next(std::ranges::iterator_t<View> current)
    {
        IRIS_ASSERT(pred_);

        return std::ranges::next(
            std::ranges::adjacent_find(current, std::ranges::end(base_),
                                       std::not_fn(std::ref(*pred_))),
            1, std::ranges::end(base_));
    }

    constexpr std::ranges::iterator_t<View>
    find_prev(std::ranges::iterator_t<View> current) //
        requires std::ranges::bidirectional_range<View>
    {
        IRIS_ASSERT(current != std::ranges::begin(base_));
        IRIS_ASSERT(pred_);

        using namespace std::placeholders;
        std::ranges::reverse_view rv(
            std::ranges::subrange(std::ranges::begin(base_), current));
        return std::ranges::prev(
            std::ranges::adjacent_find(
                rv, std::not_fn(std::bind(std::ref(*pred_), _2, _1)))
                .base(),
            1, std::ranges::begin(base_));
    }

    View base_ {};
    __detail::__copyable_box<Pred> pred_ {};
};

template <typename Range, typename Pred>
chunk_by_view(Range&&, Pred)
    -> chunk_by_view<std::ranges::views::all_t<Range>, Pred>;

namespace views {
    class __chunk_by_fn {
    public:
        template <std::ranges::viewable_range Range, typename Pred>
        constexpr auto operator()(Range&& range, Pred&& pred) const noexcept(
            noexcept(chunk_by_view(std::forward<Range>(range),
                                   std::forward<Pred>(pred)))) requires requires
        {
            chunk_by_view(std::forward<Range>(range), std::forward<Pred>(pred));
        }
        {
            return chunk_by_view(std::forward<Range>(range),
                                 std::forward<Pred>(pred));
        }

        template <typename Pred>
        constexpr auto operator()(Pred&& pred) const //
            noexcept(std::is_nothrow_constructible_v<std::decay_t<Pred>,
                                                     Pred>) //
            requires std::constructible_from<std::decay_t<Pred>, Pred>
        {
            return range_adaptor_closure<__chunk_by_fn, std::decay_t<Pred>>(
                std::forward<Pred>(pred));
        }
    };

    inline constexpr __chunk_by_fn chunk_by {};
}
}

namespace iris {
namespace views = ranges::views;
}
