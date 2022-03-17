#pragma once

#include <iris/config.hpp>

#include <iris/net/mac_address.hpp>
#include <iris/utility.hpp>

#include <string>
#include <vector>

namespace iris::net {

enum class interface_flags : uint32_t {
    none = 0x00000000,
    loopback = 0x00000001,
};

IRIS_BITWISE_OP(interface_flags)

struct network_interface {
    std::string name;
    std::uint64_t max_transmission_unit = 0;
    interface_flags iflags = interface_flags::none;
    mac_address mac_addr;
};

std::vector<network_interface> get_network_interface() noexcept;

}
