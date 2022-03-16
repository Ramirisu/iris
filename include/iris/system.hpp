#pragma once

#include <iris/config.hpp>

#include <optional>
#include <string>

namespace iris {

std::optional<std::string> get_host_name() noexcept;

std::optional<std::string> get_user_name() noexcept;

}
