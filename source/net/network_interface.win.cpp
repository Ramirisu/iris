#include <iris/net/network_interface.hpp>

#include <iris/win32/win32.hpp>

#include <WS2tcpip.h>
#include <iphlpapi.h>

#include <codecvt>
#include <memory>

namespace iris::net {

std::vector<network_interface> get_network_interface() noexcept
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

    std::vector<network_interface> nis;

    auto adapters = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.get());
    for (auto adapter = adapters; adapter != nullptr; adapter = adapter->Next) {
        auto iflags = network_interface_flags::none;
        switch (adapter->OperStatus) {
        case IfOperStatusUp:
            iflags |= network_interface_flags::up;
            break;
        }
        switch (adapter->IfType) {
        case IF_TYPE_SOFTWARE_LOOPBACK:
            iflags |= network_interface_flags::loopback;
            break;
        }

        auto append_addr = [](std::vector<ip_address>& addrs, auto addr) {
            switch (addr->Address.lpSockaddr->sa_family) {
            case AF_INET6:
                addrs.push_back(ipv6_address(
                    reinterpret_cast<sockaddr_in6*>(addr->Address.lpSockaddr)
                        ->sin6_addr.u.Word));
                break;
            case AF_INET:
                addrs.push_back(ipv4_address(
                    reinterpret_cast<sockaddr_in*>(addr->Address.lpSockaddr)
                        ->sin_addr.S_un.S_addr));
                break;
            }
        };

        std::vector<ip_address> unicast_addrs;
        for (auto addr = adapter->FirstUnicastAddress; addr != nullptr;
             addr = addr->Next) {
            append_addr(unicast_addrs, addr);
        }
        std::vector<ip_address> multicast_addrs;
        for (auto addr = adapter->FirstMulticastAddress; addr != nullptr;
             addr = addr->Next) {
            append_addr(multicast_addrs, addr);
        }

        nis.push_back(network_interface {
            adapter->AdapterName, adapter->Mtu, iflags,
            std::move(unicast_addrs), std::move(multicast_addrs),
            mac_address(
                adapter->PhysicalAddress[0], adapter->PhysicalAddress[1],
                adapter->PhysicalAddress[2], adapter->PhysicalAddress[3],
                adapter->PhysicalAddress[4], adapter->PhysicalAddress[5]) });
    }

    return nis;
}

}