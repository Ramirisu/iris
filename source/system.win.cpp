#include <iris/system.hpp>
#include <iris/win32/win32.hpp>

#include <codecvt>

namespace iris {

std::optional<std::string> get_host_name() noexcept
{
    std::wstring buffer(MAX_COMPUTERNAME_LENGTH, '\0');
    DWORD size = buffer.size() + 1;
    if (GetComputerNameW(buffer.data(), &size) == TRUE) {
        buffer.resize(size);
        return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>()
            .to_bytes(buffer);
    }

    return std::nullopt;
}

}
