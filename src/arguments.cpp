/**
 * @file arguments.cpp
 * @brief File for parsing arguments
 * @author Martin Mendl <x247581>
 * @date 2025-27-02
 */

#include <iostream>
#include <getopt.h>
#include <cstdlib>
#include <vector>
#include <regex>
#include <cstring>
#include <netdb.h>
#include <arpa/inet.h>
#include "arguments.hpp"


// Function to parse the ports
std::vector<int> parsePorts(const std::string &ports) {

    // look for the - in the string
    size_t found = ports.find("-");

    // port range
    if (found != std::string::npos) {

        // we need to do a few chesk, here, in this instanece, 
        
        // split the string, with the - index, and find lower and upper bounds
        std::string lowerBound = ports.substr(0, found);
        std::string upperBound = ports.substr(found + 1, ports.length());

        // if anythin otheer, than numbers are present in lowe and Uppoer, we have to throw an error
        if (lowerBound.find_first_not_of("0123456789") != std::string::npos || upperBound.find_first_not_of("0123456789") != std::string::npos) {
            // this may need to chnage a bit, since i dont want to use try catch with this calss, but returning a special vector or NULL, may be a good idea
            throw std::invalid_argument("Invalid port range: " + ports); 
        }

        // convert the strings to integers
        int lower = std::stoi(lowerBound);
        int upper = std::stoi(upperBound);

        // check if the lower bound is less than the upper bound
        if (lower > upper) {
            throw std::invalid_argument("Invalid port range: " + ports);
        }

        // create a vector of integers, and add the range to it
        std::vector<int> portList;
        for (int i = lower; i <= upper; i++) {
            portList.push_back(i);
        }

        return portList;
    } 

    // port selection
    std::vector<int> portList;
    bool skip = false;
    int start_index = 0;

    // pase the ports, and add them to the list
    for (size_t i = 0; i < ports.length(); i++) {

        // moving end index, to the end of the number
        if (ports[i] != ' ' && ports[i] != ',' && !skip) continue;
        
        if (!skip) {
            // if we have a comma, we add the port to the list
            std::string port = ports.substr(start_index, i - start_index);
            if (port.find_first_not_of("0123456789") != std::string::npos) {
                throw std::invalid_argument("Invalid port: " + port);
            }

            // save the port to the list
            portList.push_back(std::stoi(port));
            skip = true;
        }

        if (ports[i] != ' ' && ports[i] != ',') {
            skip = false;
            start_index = i;
        }
    }

    // add the last port to the list
    std::string port = ports.substr(start_index, ports.length() - start_index);
    if (port.find_first_not_of("0123456789") != std::string::npos) {
        throw std::invalid_argument("Invalid port: " + port);
    }
    portList.push_back(std::stoi(port));

    return portList;
}

// Function to determine the target type
TargetType determinTargetType(const std::string &target) {

    // Regular expressions for IPv4, IPv6, and domain name
    std::regex ipv4_regex("^(\\d{1,3}\\.){3}\\d{1,3}$");
    std::regex ipv6_regex("^[0-9a-fA-F:]+$");
    std::regex domain_regex("^[a-zA-Z0-9.-]+$");

    if (std::regex_match(target, ipv4_regex)) return TargetType::IP_v4;
    if (std::regex_match(target, ipv6_regex)) return TargetType::IP_v6;
    if (std::regex_match(target, domain_regex)) return TargetType::DOMAIN_NAME;
    return TargetType::UNKNOWN;
}

// Function to get the target IP from the domain name
std::pair<std::string, std::string> getTargetIPsFromDomain(const std::string &domain) {
    std::pair<std::string, std::string> addresses{"", ""};
    struct addrinfo hints{}, *res, *p;
    char ipstr[INET6_ADDRSTRLEN];
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;

    int status = getaddrinfo(domain.c_str(), nullptr, &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
        throw std::runtime_error("Failed to resolve domain name");
    }

    for (p = res; p != nullptr; p = p->ai_next) {
        void *addr = nullptr;
        if (p->ai_family == AF_INET && addresses.first.empty()) {
            addr = &reinterpret_cast<sockaddr_in*>(p->ai_addr)->sin_addr;
            inet_ntop(AF_INET, addr, ipstr, sizeof(ipstr));
            addresses.first = ipstr;
        } else if (p->ai_family == AF_INET6 && addresses.second.empty()) {
            addr = &reinterpret_cast<sockaddr_in6*>(p->ai_addr)->sin6_addr;
            inet_ntop(AF_INET6, addr, ipstr, sizeof(ipstr));
            addresses.second = ipstr;
        }
    }

    freeaddrinfo(res);
    return addresses;
}

// Constructor
Settings::Settings(int argc, char *argv[]) {

    int opt;
    int option_index = 0;
    struct option long_options[] = {
        {"interface", required_argument, 0, 'i'},
        {"pt", required_argument, 0, 't'},
        {"pu", required_argument, 0, 'u'},
        {"wait", required_argument, 0, 'w'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "i:t:u:w:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'i':
                interfaceName = optarg;
                break;
            case 't':
                TCPports = parsePorts(optarg);
                break;
            case 'u':
                UDPports = parsePorts(optarg);
                break;
            case 'w':
                timeout = std::stoi(optarg);
                break;
            case 'h':
                printHelp();
                exit(0);
            default:
                std::cout << "Invalid argument, seek -h|--help for help" << std::endl;
                exit(1);
        }
    }
    
    // emty target
    if (optind >= argc) {
        mode = Mode::SCAN; 
        return;
    }

    std::pair<std::string, std::string> adresses;

    // get the target
    switch(determinTargetType(argv[optind])) {
        case TargetType::IP_v4:
            targetIp4 = {"", argv[optind], IpVersion::IPV4, -1};
            Targetipv4 = true;
            break;
        case TargetType::IP_v6:
            targetIp6 = {"", argv[optind], IpVersion::IPV6, -1};
            Targetipv6 = true;
            break;
        case TargetType::DOMAIN_NAME:
            adresses = getTargetIPsFromDomain(argv[optind]);
            if (adresses.first.empty() && adresses.second.empty()) {
                throw std::invalid_argument("No IP address found for domain: " + std::string(argv[optind]));
            }

            if (!adresses.first.empty()) {
                targetIp4 = {argv[optind], adresses.first, IpVersion::IPV4, -1};
                Targetipv4 = true;
            }

            if (!adresses.second.empty()) {
                targetIp6 = {argv[optind], adresses.second, IpVersion::IPV6, -1};
                Targetipv6 = true;
            }
            break;
        default:
            throw std::invalid_argument("Invalid target");
    };

    // select between the two modes
    if (interfaceName.empty()) mode = Mode::PRINT_INTERFACES;
    else mode = Mode::SCAN;
}

void Settings::printHelp() const {
    std::cout << "Usage: ./ipk-l4-scan [OPTIONS] TARGET" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -i, --interface=INTERFACE  Interface to use for scanning" << std::endl;
    std::cout << "  -t, --pt=PORTS             TCP ports to scan" << std::endl;
    std::cout << "  -u, --pu=PORTS             UDP ports to scan" << std::endl;
    std::cout << "  -w, --wait=TIMEOUT         Timeout for the scan" << std::endl;
    std::cout << "  --help                     Print this help message" << std::endl;
    std::cout << "   TARGET                    Target to scan [IPv4 | IPv6 | Domain]" << std::endl;
}

