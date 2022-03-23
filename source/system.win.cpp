#include <iris/system.hpp>
#include <iris/win32/win32.hpp>

#include <codecvt>

#include <Lmcons.h>

namespace iris {

expected<std::string, std::error_code> get_host_name() noexcept
{
    wchar_t buffer[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
    if (GetComputerNameW(buffer, &size) == TRUE) {
        buffer[size + 1] = 0;
        return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>()
            .to_bytes(buffer);
    }

    return unexpected(std::error_code(GetLastError(), std::system_category()));
}

expected<std::string, std::error_code> get_user_name() noexcept
{
    wchar_t buffer[UNLEN + 1] = {};
    DWORD size = UNLEN + 1;
    if (GetUserNameW(buffer, &size) == TRUE) {
        buffer[size] = 0;
        return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>()
            .to_bytes(buffer);
    }

    return unexpected(std::error_code(GetLastError(), std::system_category()));
}

}
