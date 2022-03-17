#include <iris/net/interface.hpp>

#include <cstring>
#include <unordered_map>

#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace iris::net {

std::vector<interface> get_interface() noexcept
{
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock == -1) {
        return {};
    }

    ifaddrs* ifaddrs;
    if (getifaddrs(&ifaddrs) == -1) {
        close(sock);
        return {};
    }

    std::unordered_map<std::string, interface> interface_map;
    for (auto ifaddr = ifaddrs; ifaddr != nullptr; ifaddr = ifaddr->ifa_next) {
        if (interface_map.find(ifaddr->ifa_name) != end(interface_map)) {
            continue;
        }

        ifreq ifr;
        strcpy(ifr.ifr_ifrn.ifrn_name, ifaddr->ifa_name);
        if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
            interface_map[ifaddr->ifa_name].mac_addr
                = mac_address(ifr.ifr_ifru.ifru_hwaddr.sa_data[0],
                              ifr.ifr_ifru.ifru_hwaddr.sa_data[1],
                              ifr.ifr_ifru.ifru_hwaddr.sa_data[2],
                              ifr.ifr_ifru.ifru_hwaddr.sa_data[3],
                              ifr.ifr_ifru.ifru_hwaddr.sa_data[4],
                              ifr.ifr_ifru.ifru_hwaddr.sa_data[5]);
        }
    }

    freeifaddrs(ifaddrs);
    close(sock);

    std::vector<interface> result;
    for (auto& [name, i] : interface_map) {
        result.push_back(std::move(i));
    }
    return result;
}

}
