#include <thirdparty/test.hpp>

#include <iris/type_traits.hpp>

#include <list>
#include <vector>

using namespace iris;

TEST_SUITE_BEGIN("type_traits");

TEST_CASE("is_specialized_of")
{
    static_assert(is_specialized_of_v<std::vector<int>, std::vector>);
    static_assert(!is_specialized_of_v<std::vector<int>, std::list>);
}

TEST_SUITE_END();
