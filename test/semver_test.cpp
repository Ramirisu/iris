#include <thirdparty/test.hpp>

#include <iris/semver.hpp>

using namespace iris;

TEST_SUITE_BEGIN("semver");

TEST_CASE("from_string")
{
    CHECK(!semver::from_string(""));
    CHECK(!semver::from_string("1"));
    CHECK(!semver::from_string("1."));
    CHECK(!semver::from_string("1.2"));
    CHECK(!semver::from_string("1.2."));
    CHECK(!semver::from_string("1.2.3."));
    CHECK(!semver::from_string("1.2.3.4"));

    CHECK(semver::from_string("1.2.3"));
    CHECK(semver::from_string("123.456.789"));
}

TEST_CASE("to_string")
{
    CHECK_EQ(semver().to_string(), "0.0.0");
    CHECK_EQ(semver(1, 2, 3).to_string(), "1.2.3");
}

TEST_CASE("comparator")
{
    CHECK_EQ(semver(), semver());
    CHECK_NE(semver(), semver(1, 0, 0));
    CHECK_GT(semver(1, 0, 0), semver());
    CHECK_GE(semver(1, 0, 0), semver());
    CHECK_GE(semver(), semver());
    CHECK_LT(semver(), semver(1, 0, 0));
    CHECK_LE(semver(), semver(1, 0, 0));
    CHECK_LE(semver(), semver());

    CHECK_GT(semver(1, 0, 0), semver(0, 10, 10));
    CHECK_GT(semver(1, 1, 0), semver(1, 0, 10));
    CHECK_GT(semver(1, 1, 1), semver(1, 1, 0));
}

TEST_SUITE_END();
