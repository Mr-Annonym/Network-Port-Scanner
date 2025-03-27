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

    // Get available network interfaces
    std::vector<NetworkAdress> interfaces = getNetworkInterfaces();

    // Print out interfaces, if no target or interface is specified
    if (settings.getMode() == Mode::PRINT_INTERFACES) {
        representInterfaces(interfaces);
        return 0;
    }

    NetworkAdress *recv;
    NetworkAdress sender;

    while (1) {

        // get the sender
        recv = settings.getTargetIp4();
        sender = validateInterface(interfaces, settings.getInterface(), true);
        if (recv == nullptr) {
            recv = settings.getTargetIp6();
            if (recv == nullptr) break;
            sender = validateInterface(interfaces, settings.getInterface(), false);
        }
        // tcp
        for (auto port : settings.getTCPports()) {
            scanPortTCP(sender, *recv, port, settings.getTimeout());
        }

        // udp
        for (auto port : settings.getUDPports()) {
            scanPortUDP(sender, *recv, port, settings.getTimeout());
        }
    }
}
  