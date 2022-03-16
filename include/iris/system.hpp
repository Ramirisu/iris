#pragma once

#include <iris/config.hpp>

#include <optional>
#include <string>

namespace iris {

std::optional<std::string> get_host_name() noexcept;

}
