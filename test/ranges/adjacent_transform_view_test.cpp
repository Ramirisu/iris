#include <thirdparty/test.hpp>

#include <iris/ranges/adjacent_transform_view.hpp>
#include <iris/type_traits.hpp>

#include <forward_list>
#include <list>

using namespace iris;

TEST_SUITE_BEGIN("adjacent_transform_view");

template <typename T>
struct multiply {
    constexpr T operator()() const
    {
        return 0;
    }

    template <typename... Ts>
        requires(sizeof...(Ts) > 0)
    constexpr front_of_t<T, Ts...> operator()(Ts... ts) const
    {
        return (ts * ...);
    }
};

TEST_CASE("value_type: tuple or pair")
{
    static_assert(std::same_as<decltype(views::adjacent_transform<0>(
                                   std::views::iota(0, 1), multiply<int> {})),
                               std::ranges::empty_view<int>>);
    static_assert(
        std::same_as<
            std::ranges::range_value_t<decltype(views::adjacent_transform<1>(
                std::views::iota(0, 1), multiply<int> {}))>,
            int>);
    static_assert(
        std::same_as<
            std::ranges::range_value_t<decltype(views::adjacent_transform<2>(
                std::views::iota(0, 1), multiply<int> {}))>,
            int>);
    static_assert(std::same_as<
                  std::ranges::range_value_t<decltype(views::pairwise_transform(
                      std::views::iota(0, 1), multiply<int> {}))>,
                  int>);
    static_assert(
        std::same_as<
            std::ranges::range_value_t<decltype(views::adjacent_transform<3>(
                std::views::iota(0, 1), multiply<int> {}))>,
            int>);
}

TEST_CASE("forward_range")
{
    static const auto input = std::forward_list { 0, 1, 2, 3, 4 };
    auto view = views::adjacent_transform<3>(input, multiply<int> {});
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 0);
    CHECK_EQ(*curr++, 6);
    CHECK_EQ(*curr++, 24);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("bidirectional_range")
{
    static const auto input = std::list { 0, 1, 2, 3, 4 };
    auto view = views::adjacent_transform<3>(input, multiply<int> {});
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 0);
    CHECK_EQ(*curr--, 6);
    CHECK_EQ(*curr++, 0);
    CHECK_EQ(*curr++, 6);
    CHECK_EQ(*curr--, 24);
    CHECK_EQ(*curr++, 6);
    CHECK_EQ(*curr++, 24);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("random_access_range")
{
    static const int input[] = { 0, 1, 2, 3, 4 };
    auto view = views::adjacent_transform<3>(input, multiply<int> {});
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr, 0);
    curr += 1;
    CHECK_EQ(*curr, 6);
    curr += 1;
    CHECK_EQ(*curr, 24);
    curr += 1;
    CHECK_EQ(curr, std::ranges::end(view));
    curr -= 3;
    CHECK_EQ(*curr, 0);
}

TEST_SUITE_END();
