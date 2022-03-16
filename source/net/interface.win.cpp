#include <iris/net/interface.hpp>

#include <iris/win32/win32.hpp>

#include <iphlpapi.h>

#include <codecvt>
#include <memory>

namespace iris::net {

std::vector<interface> get_interface() noexcept
{
    ULONG size = 0;
    if (GetAdaptersAddresses(AF_UNSPEC, 0, nullptr, nullptr, &size)
        != ERROR_BUFFER_OVERFLOW) {
        return {};
    }

    auto buffer = std::make_unique<uint8_t[]>(size);
    if (GetAdaptersAddresses(
            AF_UNSPEC, 0, nullptr,
            reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.get()), &size)
        != ERROR_SUCCESS) {
        return {};
    }

    std::vector<interface> interfaces;

    auto adapters = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.get());
    for (auto adapter = adapters; adapter != nullptr; adapter = adapter->Next) {
        interfaces.push_back(interface { mac_address(
            adapter->PhysicalAddress[0], adapter->PhysicalAddress[1],
            adapter->PhysicalAddress[2], adapter->PhysicalAddress[3],
            adapter->PhysicalAddress[4], adapter->PhysicalAddress[5]) });
    }

    return interfaces;
}

}
