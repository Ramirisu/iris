#include <thirdparty/test.hpp>

#include <iris/type_traits.hpp>

#include <bit>
#include <list>
#include <vector>

using namespace iris;

TEST_SUITE_BEGIN("type_traits");

TEST_CASE("is_specialized_of")
{
    static_assert(is_specialization_of_v<std::vector<int>, std::vector>);
    static_assert(!is_specialization_of_v<std::vector<int>, std::list>);
}

TEST_CASE("is_scoped_enum")
{
    enum e {};
    static_assert(is_scoped_enum_v<std::endian>);
    static_assert(!is_scoped_enum_v<e>);
    static_assert(!is_scoped_enum_v<int>);
}

TEST_CASE("front_of")
{
    static_assert(std::same_as<front_of_t<int>, int>);
    static_assert(std::same_as<front_of_t<int, char>, char>);
    static_assert(std::same_as<front_of_t<int, char, bool>, char>);
}

TEST_CASE("back_of")
{
    static_assert(std::same_as<back_of_t<int>, int>);
    static_assert(std::same_as<back_of_t<int, char>, char>);
    static_assert(std::same_as<back_of_t<int, char, bool>, bool>);
}

TEST_SUITE_END();
