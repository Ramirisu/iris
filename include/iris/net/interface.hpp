#pragma once

#include <iris/config.hpp>

#include <iris/net/mac_address.hpp>

#include <string>
#include <vector>

namespace iris::net {

struct interface {
    mac_address mac_addr;
};

std::vector<interface> get_interface() noexcept;

}
