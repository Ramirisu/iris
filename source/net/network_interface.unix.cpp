#include <iris/net/network_interface.hpp>

#include <cstring>
#include <unordered_map>

#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace iris::net {

std::vector<network_interface> get_network_interface() noexcept
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

    std::unordered_map<std::string, network_interface> ni_map;
    for (auto ifaddr = ifaddrs; ifaddr != nullptr; ifaddr = ifaddr->ifa_next) {
        if (ni_map.find(ifaddr->ifa_name) != end(ni_map)) {
            continue;
        }
        auto& ni = ni_map[ifaddr->ifa_name];
        ni.name = ifaddr->ifa_name;

        ifreq ifr;
        strcpy(ifr.ifr_ifrn.ifrn_name, ifaddr->ifa_name);
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0
            && (ifr.ifr_ifru.ifru_flags & IFF_LOOPBACK)) {
            ni.iflags |= interface_flags::loopback;
        }
        if (ioctl(sock, SIOCGIFMTU, &ifr) == 0) {
            ni.max_transmission_unit = ifr.ifr_ifru.ifru_mtu;
        }
        if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
            ni.mac_addr = mac_address(ifr.ifr_ifru.ifru_hwaddr.sa_data[0],
                                      ifr.ifr_ifru.ifru_hwaddr.sa_data[1],
                                      ifr.ifr_ifru.ifru_hwaddr.sa_data[2],
                                      ifr.ifr_ifru.ifru_hwaddr.sa_data[3],
                                      ifr.ifr_ifru.ifru_hwaddr.sa_data[4],
                                      ifr.ifr_ifru.ifru_hwaddr.sa_data[5]);
        }
    }

    freeifaddrs(ifaddrs);
    close(sock);

    std::vector<network_interface> nis;
    for (auto& [name, ni] : ni_map) {
        nis.push_back(std::move(ni));
    }
    return nis;
}

}
