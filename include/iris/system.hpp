#pragma once

#include <iris/config.hpp>

#include <iris/expected.hpp>

#include <string>
#include <system_error>

namespace iris {

expected<std::string, std::error_code> get_host_name() noexcept;

expected<std::string, std::error_code> get_user_name() noexcept;

}
