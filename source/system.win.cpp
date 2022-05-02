#include <iris/system.hpp>

#include <iris/ranges/to.hpp>
#include <iris/ranges/view/unwrap_view.hpp>
#include <iris/ranges/view/utf_view.hpp>
#include <iris/win32/win32.hpp>

#include <ranges>

#include <Lmcons.h>

namespace iris {

expected<std::string, std::error_code> get_host_name() noexcept
{
    wchar_t buffer[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
    if (GetComputerNameW(buffer, &size) == TRUE) {
        buffer[size + 1] = 0;
        return std::wstring_view(buffer) | views::from_utf | views::unwrap
            | views::to_utf<char> | views::unwrap | ranges::to<std::string>();
    }

    return unexpected(std::error_code(GetLastError(), std::system_category()));
}

expected<std::string, std::error_code> get_user_name() noexcept
{
    wchar_t buffer[UNLEN + 1] = {};
    DWORD size = UNLEN + 1;
    if (GetUserNameW(buffer, &size) == TRUE) {
        buffer[size] = 0;
        return std::wstring_view(buffer) | views::from_utf | views::unwrap
            | views::to_utf<char> | views::unwrap | ranges::to<std::string>();
    }

    return unexpected(std::error_code(GetLastError(), std::system_category()));
}

}
