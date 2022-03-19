#pragma once

#include <iris/config.hpp>

#include <iris/net/ip_address.hpp>
#include <iris/net/mac_address.hpp>
#include <iris/utility.hpp>

#include <string>
#include <vector>

namespace iris::net {

enum class network_interface_flags : uint32_t {
    none = 0x00000000,
    up = 0x00000001,
    loopback = 0x00000002,
};

IRIS_BITWISE_OP(network_interface_flags)

struct network_interface {
    std::string name;
    std::uint64_t max_transmission_unit = 0;
    network_interface_flags iflags = network_interface_flags::none;
    std::vector<ip_address> unicast_addrs;
    std::vector<ip_address> multicast_addrs;
    mac_address mac_addr;
};

std::vector<network_interface> get_network_interface() noexcept;

}
