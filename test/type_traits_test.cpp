#include <thirdparty/test.hpp>

#include <iris/type_traits.hpp>

#include <bit>
#include <list>
#include <vector>

using namespace iris;

TEST_SUITE_BEGIN("type_traits");

TEST_CASE("is_specialized_of")
{
    static_assert(is_specialized_of_v<std::vector<int>, std::vector>);
    static_assert(!is_specialized_of_v<std::vector<int>, std::list>);
}

TEST_CASE("is_scoped_enum")
{
    enum e {};
    static_assert(is_scoped_enum_v<std::endian>);
    static_assert(!is_scoped_enum_v<e>);
    static_assert(!is_scoped_enum_v<int>);
}

TEST_SUITE_END();