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

    if (std::regex_match(target, ipv4_regex)) {
        return TargetType::IP_v4;
    } else if (std::regex_match(target, ipv6_regex)) {
        return TargetType::IP_v6;
    } else if (std::regex_match(target, domain_regex)) {
        return TargetType::DOMAIN_NAME;
    } else {
        return TargetType::UNKNOWN;
    }

}

// Function to return IPv4 and IPv6 address from domain name
Target getTargetIPfromDomain(const std::string& domain) {
    struct addrinfo hints, *res;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // Support both IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(domain.c_str(), nullptr, &hints, &res)) != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
        return {"", AF_UNSPEC, -1};
    }

    void *addr;
    int family = res->ai_family;

    if (family == AF_INET) { // IPv4
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
        addr = &(ipv4->sin_addr);
    } else { // IPv6
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)res->ai_addr;
        addr = &(ipv6->sin6_addr);
    }

    inet_ntop(family, addr, ipstr, sizeof(ipstr));
    freeaddrinfo(res);

    // construct the return struct
    Target target = {domain, ipstr, family};
    return target;
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
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "i:t:u:w:", long_options, &option_index)) != -1) {
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
            default:
            throw std::invalid_argument("Invalid argument");
        }
    }
    
    // emty target
    if (optind >= argc) throw std::invalid_argument("Target not specified");
    
    // get the target
    switch(determinTargetType(argv[optind])) {
        case TargetType::IP_v4:
            target = {"", argv[optind], AF_INET};
            break;
        case TargetType::IP_v6:
            target = {"", argv[optind], AF_INET6};
            break;
        case TargetType::DOMAIN_NAME:
            target = getTargetIPfromDomain(argv[optind]);
            if (target.ip.empty()) throw std::invalid_argument("Invalid domain name");
            break;
        default:
            throw std::invalid_argument("Invalid target");
    };

    // select between the two modes
    if (interfaceName.empty()) mode = Mode::PRINT_INTERFACES;
    else mode = Mode::SCAN;
}

std::string Settings::getInterface() const {
    return interfaceName;
}
std::vector<int> Settings::getTCPports() const {
    return TCPports;
}
std::vector<int> Settings::getUDPports() const {
    return UDPports;
}
int Settings::getTimeout() const {
    return timeout;
}
Target Settings::getTarget() const {
    return target;
}
Mode Settings::getMode() const {
    return mode;
}

void Settings::representArguments() const {
    std::cout << "-- Arguments --" << std::endl;
    std::cout << "Interface: " << interfaceName << std::endl;
    std::cout << "TCP Ports: ";
    for (const auto &port : TCPports) {
        std::cout << port << " ";
    }
    std::cout << std::endl;
    std::cout << "UDP Ports: ";
    for (const auto &port : UDPports) {
        std::cout << port << " ";
    }
    std::cout << std::endl;
    std::cout << "Timeout: " << timeout << " ms" << std::endl;
    std::cout << "Target: " << target.domain << " (" << target.ip << ")" << std::endl;
}

