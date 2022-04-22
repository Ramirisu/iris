#include <thirdparty/test.hpp>

#include <iris/algorithm/starts_with.hpp>

#include <span>
#include <vector>

using namespace iris;

TEST_SUITE_BEGIN("starts_with");

TEST_CASE("starts_with")
{
    static const int input[] = { 0, 1, 2, 3, 4 };
    for (std::size_t i = 0; i < std::size(input); ++i) {
        const auto subrange = std::span(input).subspan(0, i + 1);
        CHECK(ranges::starts_with(
            std::ranges::begin(input), std::ranges::end(input),
            std::ranges::begin(subrange), std::ranges::end(subrange)));
        CHECK(ranges::starts_with(input, subrange));
    }

    const auto empty = std::views::empty<int>;
    CHECK(ranges::starts_with(input, empty));
    CHECK(ranges::starts_with(
        std::ranges::begin(input), std::ranges::end(input),
        std::ranges::begin(empty), std::ranges::end(empty)));

    const auto test_cases
        = std::vector { std::vector { 1 }, std::vector { 0, 0 },
                        std::vector { 0, 1, 2, 3, 4, 5 } };

    for (const auto& test_case : test_cases) {
        CHECK(!ranges::starts_with(input, test_case));
        CHECK(!ranges::starts_with(
            std::ranges::begin(input), std::ranges::end(input),
            std::ranges::begin(test_case), std::ranges::end(test_case)));
    }
}

TEST_SUITE_END();
