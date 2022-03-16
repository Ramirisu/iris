#include <iris/system.hpp>

#include <limits.h>
#include <unistd.h>

namespace iris {

std::optional<std::string> get_host_name() noexcept
{
    char hostname[HOST_NAME_MAX];
    if (gethostname(hostname, HOST_NAME_MAX) == 0) {
        return hostname;
    }

    return std::nullopt;
}

std::optional<std::string> get_user_name() noexcept
{
    char username[LOGIN_NAME_MAX];
    if (getlogin_r(username, LOGIN_NAME_MAX) == 0) {
        return username;
    }

    return std::nullopt;
}

}
