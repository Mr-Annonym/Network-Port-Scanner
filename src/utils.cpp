/**
 * @file utils.cpp
 * @brief File for utility functions for the whole project
 * @author Martin Mendl <x247581>
 * @date 2025-27-02
*/

#include <iostream>
#include <string>
#include <vector>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unordered_set>
#include <stdexcept>
#include "utils.hpp"

// function for header checksums
unsigned short checkSum(const char *buf, unsigned size) {
    unsigned sum = 0, i;
    for (i = 0; i < size -1; i += 2) {
        sum += *(unsigned short *)&buf[i];
    }

    if (size & 1) sum += (unsigned char) buf[i];
    while (sum >> 16) sum = (sum & 0xFFFF)+(sum >> 16);
    return ~sum;
}

// function returning the available network interfaces
std::vector<NetworkAdress> getNetworkInterfaces() {
    std::vector<NetworkAdress> interfaces;
    struct ifaddrs *ifaddr, *ifa;
    char addr[INET6_ADDRSTRLEN];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        throw std::runtime_error("Failed to get network interfaces");
    }

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr) continue;

        NetworkAdress netAddr;
        netAddr.hostName = ifa->ifa_name;
        netAddr.port = -1; // deefault value, not used

        void *addr_ptr = nullptr;
        if (ifa->ifa_addr->sa_family == AF_INET) { // IPv4
            addr_ptr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            netAddr.ipVer = IpVersion::IPV4;
        } else if (ifa->ifa_addr->sa_family == AF_INET6) { // IPv6
            addr_ptr = &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            netAddr.ipVer =  IpVersion::IPV6;
        } else {
            continue;
        }

        inet_ntop(ifa->ifa_addr->sa_family, addr_ptr, addr, sizeof(addr));
        netAddr.ip = addr;
        interfaces.push_back(netAddr);
    }

    freeifaddrs(ifaddr);
    return interfaces;
    
}

// function to represent the available network interfaces
void representInterfaces(std::vector<NetworkAdress> interfaces) {

    std::unordered_set<std::string> printedInterfaces;

    for (const NetworkAdress& interface : interfaces) {
        if (printedInterfaces.find(interface.hostName) == printedInterfaces.end()) {
            printedInterfaces.insert(interface.hostName);
            std::cout << "Interface: " << interface.hostName << std::endl;
        }
    }
}   

// function to validate the network interface by name form the accepted interfaces
NetworkAdress validateInterface(std::vector<NetworkAdress>& interfaces, const std::string& interface_name, bool ipv4) {

    IpVersion ipVer = ipv4 ? IpVersion::IPV4 : IpVersion::IPV6;
    bool found = false;

    for (auto& interface : interfaces) {
        if (interface.hostName == interface_name) {
            found = true;
            if (interface.ipVer == ipVer) return interface;
        }
    }

    if (found) return NetworkAdress{"", "", ipVer, -1}; // return empty address if found but not matching version
    throw std::runtime_error("Invalid network interface name");
}