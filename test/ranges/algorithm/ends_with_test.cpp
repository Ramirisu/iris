#include <thirdparty/test.hpp>

#include <iris/ranges/algorithm/ends_with.hpp>

#include <span>
#include <vector>

using namespace iris;

TEST_SUITE_BEGIN("ends_with");

TEST_CASE("ends_with")
{
    static const int input[] = { 0, 1, 2, 3, 4 };
    for (std::size_t i = 0; i < std::size(input); ++i) {
        const auto subrange = std::span(input).subspan(i);
        CHECK(ranges::ends_with(
            std::ranges::begin(input), std::ranges::end(input),
            std::ranges::begin(subrange), std::ranges::end(subrange)));
        CHECK(ranges::ends_with(input, subrange));
    }

    const auto empty = std::views::empty<int>;
    CHECK(ranges::ends_with(input, empty));
    CHECK(ranges::ends_with(std::ranges::begin(input), std::ranges::end(input),
                            std::ranges::begin(empty),
                            std::ranges::end(empty)));

    const auto test_cases
        = std::vector { std::vector { 3 }, std::vector { 4, 4 },
                        std::vector { 0, 0, 1, 2, 3, 4 } };

    for (const auto& test_case : test_cases) {
        CHECK(!ranges::ends_with(input, test_case));
        CHECK(!ranges::ends_with(
            std::ranges::begin(input), std::ranges::end(input),
            std::ranges::begin(test_case), std::ranges::end(test_case)));
    }
}

TEST_SUITE_END();
