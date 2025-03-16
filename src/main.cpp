/**
 * @file main.cpp
 * @brief Main file for the IPK project
 * @author Martin Mendl <x247581>
 * @date 2025-27-02
*/

#include <iostream>
#include "arguments.hpp"
#include "scanning.hpp"
#include "utils.hpp"


int main(int argc, char *argv[]) {
    // Parse arguments
    Settings settings(argc, argv);

    if (settings.getMode() == Mode::UNKNOWN) {
        std::cerr << "Invalid arguments seek --help" << std::endl;
        return 1;
    }

    std::vector<NetworkAdress> interfaces = getNetworkInterfaces();
    if (settings.getMode() == Mode::PRINT_INTERFACES) {
        representInterfaces(interfaces);
        return 0;
    }
    NetworkAdress receiver, sender;

    bool doIpv4 = settings.isTargetIpv4();
    bool doIpv6 = settings.isTargetIpv6();

    // TCP
    for (auto port : settings.getTCPports()) {

        // IPV4
        if (doIpv4) {
            receiver = settings.getTargetIp4();
            sender = validateInterface(interfaces, settings.getInterface(), true);
            scanPortTCP(sender, receiver, port, settings.getTimeout());
        } 
        
        // IPV6
        if (doIpv6) {
            receiver = settings.getTargetIp6();
            sender = validateInterface(interfaces, settings.getInterface(), false);
            scanPortTCP(sender, receiver, port, settings.getTimeout());
        }
    }

    // UPD
    for (auto port : settings.getUDPports()) {

        // IPV4
        if (doIpv4) {
            receiver = settings.getTargetIp4();
            sender = validateInterface(interfaces, settings.getInterface(), true);
            scanPortUDP(sender, receiver, port, settings.getTimeout());
        } 
        
        // IPV6
        if (doIpv6) {
            receiver = settings.getTargetIp6();
            sender = validateInterface(interfaces, settings.getInterface(), false);
            scanPortUDP(sender, receiver, port, settings.getTimeout());
        }
    }
    return 0;
}
  