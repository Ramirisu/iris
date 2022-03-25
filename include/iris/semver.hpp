#pragma once

#include <iris/config.hpp>

#include <iris/expected.hpp>

#include <charconv>
#include <string_view>
#include <system_error>

namespace iris {

class semver {
public:
    semver() = default;

    semver(std::uint32_t major, std::uint32_t minor, std::uint32_t patch)
        : major_(major)
        , minor_(minor)
        , patch_(patch)
    {
    }

    std::string to_string() const noexcept
    {
        std::string result = std::to_string(major_);
        result += '.';
        result += std::to_string(minor_);
        result += '.';
        result += std::to_string(patch_);
        return result;
    }

    static expected<semver, std::error_code>
    from_string(std::string_view str) noexcept
    {
        semver ver;

        const auto last = str.data() + str.size();
        const char* first = str.data();
        if (auto [ptr, ec] = std::from_chars(first, last, ver.major_);
            ec == std::errc() && ptr < last && *ptr == '.') {
            first = ptr + 1;
        } else {
            return unexpected(
                std::make_error_code(std::errc::invalid_argument));
        }
        if (auto [ptr, ec] = std::from_chars(first, last, ver.minor_);
            ec == std::errc() && ptr < last && *ptr == '.') {
            first = ptr + 1;
        } else {
            return unexpected(
                std::make_error_code(std::errc::invalid_argument));
        }
        if (auto [ptr, ec] = std::from_chars(first, last, ver.patch_);
            ec == std::errc() && (ptr == last)) {
            first = ptr;
        } else {
            return unexpected(
                std::make_error_code(std::errc::invalid_argument));
        }

        IRIS_ASSERT(first == last);

        return ver;
    }

    friend constexpr auto operator<=>(const semver& lhs,
                                      const semver& rhs) noexcept = default;

private:
    std::uint32_t major_ = 0;
    std::uint32_t minor_ = 0;
    std::uint32_t patch_ = 0;
};

}
