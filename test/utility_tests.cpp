#include <thirdparty/test.hpp>

#include <iris/utility.hpp>

using namespace iris;

TEST_SUITE_BEGIN("utility");

TEST_CASE("to_unsigned")
{
    static_cast(std::is_same_v<decltype(to_unsigned(char(0))), unsigned char>);
    static_cast(std::is_same_v<decltype(to_unsigned(unsigned signed char(0))),
                               unsigned char>);
    static_cast(
        std::is_same_v<decltype(to_unsigned(signed char(0))), unsigned char>);
    static_cast(
        std::is_same_v<decltype(to_unsigned(std::uint8_t(0))), std::uint8_t>);
    static_cast(
        std::is_same_v<decltype(to_unsigned(std::int8_t(0))), std::uint8_t>);
    static_cast(
        std::is_same_v<decltype(to_unsigned(std::uint16_t(0))), std::uint16_t>);
    static_cast(
        std::is_same_v<decltype(to_unsigned(std::int16_t(0))), std::uint16_t>);
    static_cast(
        std::is_same_v<decltype(to_unsigned(std::uint32_t(0))), std::uint32_t>);
    static_cast(
        std::is_same_v<decltype(to_unsigned(std::int32_t(0))), std::uint32_t>);
    static_cast(
        std::is_same_v<decltype(to_unsigned(std::uint64_t(0))), std::uint64_t>);
    static_cast(
        std::is_same_v<decltype(to_unsigned(std::int64_t(0))), std::uint64_t>);
}

TEST_CASE("to_signed")
{
    static_cast(std::is_same_v<decltype(to_signed(char(0))), char>);
    static_cast(
        std::is_same_v<decltype(to_signed(unsigned signed char(0))), char>);
    static_cast(std::is_same_v<decltype(to_signed(signed char(0))), char>);
    static_cast(
        std::is_same_v<decltype(to_signed(std::uint8_t(0))), std::int8_t>);
    static_cast(
        std::is_same_v<decltype(to_signed(std::int8_t(0))), std::int8_t>);
    static_cast(
        std::is_same_v<decltype(to_signed(std::uint16_t(0))), std::int16_t>);
    static_cast(
        std::is_same_v<decltype(to_signed(std::int16_t(0))), std::int16_t>);
    static_cast(
        std::is_same_v<decltype(to_signed(std::uint32_t(0))), std::int32_t>);
    static_cast(
        std::is_same_v<decltype(to_signed(std::int32_t(0))), std::int32_t>);
    static_cast(
        std::is_same_v<decltype(to_signed(std::uint64_t(0))), std::int64_t>);
    static_cast(
        std::is_same_v<decltype(to_signed(std::int64_t(0))), std::int64_t>);
}

TEST_CASE("to_underlying")
{
    auto value = to_underlying(std::byte(1));
    static_cast(
        std::is_same_v<decltype(value)), unsigned char>);
    CHECK_EQ(value, 1);
}

TEST_SUITE_END();
