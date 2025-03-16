/**
 * @file sockets.cpp
 * @brief File for socket creation and manipulation
 * @author Martin Mendl <x247581>
 * @date 2025-27-02
 */

#include <iostream>
#include <string>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/in.h> 
#include <fcntl.h>
#include <unistd.h>
#include "sockets.hpp"
#include "utils.hpp"

// Function to return the protocol
int returnProtocol(Protocol protocol) {
    switch (protocol) {
        case Protocol::TCP:
            return IPPROTO_TCP;
        case Protocol::UDP:
            return IPPROTO_UDP;
        case Protocol::ICMP:
            return IPPROTO_ICMP;
        case Protocol::ICMP6:
            return IPPROTO_ICMPV6;
    }
    return -1;
}

// Base Socket class constructor
Socket::Socket(NetworkAdress sender, NetworkAdress receiver) {
    this->sender = sender;
    this->receiver = receiver;
}

// Base Socket class destructor
Socket::~Socket() {
    close(sockfd);
}

// Method to set the socket to non-blocking
void Socket::setNonBlocking() {

    if (nonBlocking || timeoutSet) {
        throw std::runtime_error("Socket already set to non-blocking or timeout is set");
    }

    // set the socket for not blocking
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        throw std::runtime_error("Failed to get socket flags");
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        throw std::runtime_error("Failed to set socket flags");
    }

    nonBlocking = true;

}

// Method to set the timeout for the socket
void Socket::setTimeout(int timeout) {

    if (nonBlocking || timeoutSet) {
        throw std::runtime_error("Socket already set to non-blocking or timeout is set");
    }

    // set timeout for the socket
    this->timeout.tv_sec = timeout / 1000;
    this->timeout.tv_usec = (timeout % 1000) * 1000;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &this->timeout, sizeof(struct timeval)) < 0) {
        throw std::runtime_error("Failed to set socket timeout");
    }

    timeoutSet = true;
}

// Method to bind the socket to the sender interface
void Socket::bindToInterface() {
    // bind the socket to the sender interface 
    if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, sender.hostName.c_str(), sender.hostName.length()) < 0) {
        throw std::runtime_error("Failed to bind socket to interface");
    }
}

// Constructor for the SocketIpv4 class
SocketIpv4::SocketIpv4(NetworkAdress sender, NetworkAdress receiver, Protocol protocol) : Socket(sender, receiver) {
    
    // Create socket
    sockfd = socket(AF_INET, SOCK_RAW, returnProtocol(protocol));
    if (sockfd < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    // bind the socket to the sender interface
    bindToInterface();

    // set the sender adress
    setupNetworkAdress(sender, senderAddr);

    // set the receiver adress
    setupNetworkAdress(receiver, receiverAddr);
}

// Method to set the network adress for the socket
void SocketIpv4::setupNetworkAdress(NetworkAdress &adress, struct sockaddr_in &sockAddr) {
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(adress.port);
    if (inet_pton(AF_INET, adress.ip.c_str(), &sockAddr.sin_addr) <= 0) {
        throw std::runtime_error("Invalid sender IP4 address");
    }
}

// Constructor for the SocketIpv6 class
SocketIpv6::SocketIpv6(NetworkAdress sender, NetworkAdress receiver, Protocol protocol) : Socket(sender, receiver) {
    // Create socket
    sockfd = socket(AF_INET6, SOCK_RAW, returnProtocol(protocol));
    if (sockfd < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    // bind the socket to the sender interface
    bindToInterface();

    // set the sender adress
    setupNetworkAdress(sender, senderAddr);

    // set the receiver adress
    setupNetworkAdress(receiver, receiverAddr);
}

// Method to set the network adress for the socket
void SocketIpv6::setupNetworkAdress(NetworkAdress &adress, struct sockaddr_in6 &sockAddr) {
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin6_family = AF_INET6;
    sockAddr.sin6_port = htons(adress.port);
    if (inet_pton(AF_INET6, adress.ip.c_str(), &sockAddr.sin6_addr) <= 0) {
        throw std::runtime_error("Invalid sender IP6 address");
    }
}

