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

TEST_CASE("pack_size")
{
    static_assert(pack_size_v<> == 0);
    static_assert(pack_size_v<bool> == 1);
    static_assert(pack_size_v<bool, char> == 2);
}

TEST_CASE("pack_element")
{
    static_assert(std::same_as<pack_element_t<0, bool, char, int>, bool>);
    static_assert(std::same_as<pack_element_t<1, bool, char, int>, char>);
    static_assert(std::same_as<pack_element_t<2, bool, char, int>, int>);
}

TEST_CASE("front_of_pack_element")
{
    static_assert(std::same_as<front_of_pack_element_t<int>, int>);
    static_assert(std::same_as<front_of_pack_element_t<char, int>, char>);
    static_assert(std::same_as<front_of_pack_element_t<bool, char, int>, bool>);
}

TEST_CASE("back_of")
{
    static_assert(std::same_as<back_of_pack_element_t<int>, int>);
    static_assert(std::same_as<back_of_pack_element_t<int, char>, char>);
    static_assert(std::same_as<back_of_pack_element_t<int, char, bool>, bool>);
}

TEST_SUITE_END();
