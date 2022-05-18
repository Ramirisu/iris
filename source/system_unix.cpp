#include <iris/system.hpp>

#include <limits.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

namespace iris {

expected<std::string, std::error_code> get_host_name() noexcept
{
    char hostname[HOST_NAME_MAX];
    if (gethostname(hostname, HOST_NAME_MAX) == 0) {
        return hostname;
    }

    return unexpected(std::error_code(errno, std::generic_category()));
}

expected<std::string, std::error_code> get_user_name() noexcept
{
    char username[LOGIN_NAME_MAX];
    if (getlogin_r(username, LOGIN_NAME_MAX) == 0) {
        return username;
    }

    if (auto pwd = getpwuid(getuid()); pwd != nullptr) {
        return pwd->pw_name;
    }

    return unexpected(std::error_code(errno, std::generic_category()));
}

}
