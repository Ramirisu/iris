#pragma once

#include <iris/config.hpp>

#include <iris/net/ipv4_address.hpp>
#include <iris/net/ipv6_address.hpp>

#include <optional>

namespace iris::net {

class ip_address {
public:
    enum class family {
        ipv4,
        ipv6,
    };

    ip_address()
        : family_(family::ipv4)
        , v4_()
    {
    }

    ip_address(ipv4_address addr)
        : family_(family::ipv4)
        , v4_(addr)
    {
    }

    ip_address(ipv6_address addr)
        : family_(family::ipv6)
        , v6_(addr)
    {
    }

    family family() const noexcept { return family_; }

    std::string to_string() const noexcept
    {
        return (family_ == family::ipv4) ? v4_.to_string() : v6_.to_string();
    }

    ipv4_address to_ipv4() const noexcept
    {
        IRIS_ASSERT(family_ == family::ipv4);
        return v4_;
    }

    ipv6_address to_ipv6() const noexcept
    {
        IRIS_ASSERT(family_ == family::ipv6);
        return v6_;
    }

    static std::optional<ip_address> from_string(std::string_view addr) noexcept
    {
        if (addr.find(':') != std::string_view::npos) {
            return ipv6_address::from_string(addr);
        }

        return ipv4_address::from_string(addr);
    }

private:
    enum class family family_;

    union {
        ipv4_address v4_;
        ipv6_address v6_;
    };
};

}
