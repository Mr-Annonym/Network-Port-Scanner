#include <iostream>
#include <string>
#include "arguments.hpp"
#include "utils.hpp"

int main(int argc, char *argv[]) {

    NetworkAdress addr;

    // Parse arguments
    Settings settings(argc, argv);

    std::cout << settings.getInterface() << std::endl;
    std::cout << (settings.getMode() == Mode::SCAN ? "SCAN" : "PRINT_INTERFACES") << std::endl;
    std::cout << settings.getTCPports().size() << std::endl;
    for (auto port : settings.getTCPports()) {
        std::cout << port << " ";
    }
    std::cout << std::endl;
    std::cout << settings.getUDPports().size() << std::endl;
    for (auto port : settings.getUDPports()) {
        std::cout << port << " ";
    }
    std::cout << std::endl;

    addr = settings.getTargetIp4();
    std::cout << addr.hostName << std::endl;
    std::cout << addr.ip << std::endl;
    std::cout << (addr.ipVer == IpVersion::IPV4 ? "IPV4" : "IPV6") << std::endl;
    std::cout << settings.isTargetIpv4() << std::endl;

    addr = settings.getTargetIp6();
    std::cout << addr.hostName << std::endl;
    std::cout << addr.ip << std::endl;
    std::cout << (addr.ipVer == IpVersion::IPV4 ? "IPV4" : "IPV6") << std::endl;
    std::cout << settings.isTargetIpv6() << std::endl;

    std::cout << settings.getTimeout() << std::endl;

}
