/**
 * @file scanning.cpp
 * @brief File for networking (sending and receiving packets and scenning ports)
 * @author Martin Mendl <x247581>
 * @date 2025-27-02
*/

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <fcntl.h>
#include <thread>
#include <netinet/ip_icmp.h>  
#include <netinet/icmp6.h>    
#include "utils.hpp"
#include "scanning.hpp"
#include "sockets.hpp"

// Constructor for Scanner class
Scanner::Scanner(NetworkAdress sender, NetworkAdress receiver, int timeout) {
    this->sender = sender;
    this->receiver = receiver;
    this->timeout = timeout;

    if (sender.ipVer != receiver.ipVer) {
        throw std::runtime_error("Sender and receiver IP versions do not match");
    }
}

// Destructor for Scanner class
Scanner::~Scanner() {
    if (socketip6 != nullptr) delete socketip6;
    if (socketip4 != nullptr) delete socketip4;
}

// Method for sending the packet
void Scanner::sendTo(const char *datagram, size_t datagramSize) {
    // ipv4
    if (sender.ipVer == IpVersion::IPV4) {
        sockaddr_in recv = socketip4->getReceiver();

        if (sendto(socketip4->getSocket(), datagram, datagramSize, 0, (struct sockaddr*)&recv, sizeof(recv)) < 0) {
            perror("sendto failed");
            throw std::runtime_error("Failed to send packet ipv4");
        }
        return;
    } 

    // ipv6
    sockaddr_in6 recv = socketip6->getReceiver();
    recv.sin6_port = htons(0);

    if (sendto(socketip6->getSocket(), datagram, datagramSize, 0, (struct sockaddr*)&recv, sizeof(recv)) < 0) {
        perror("sendto failed");
        throw std::runtime_error("Failed to send packet ipv6");
    }
}

// Method for receiving the packet
ssize_t Scanner::recvFrom(char *buffer, size_t bufferSize, struct sockaddr *senderAddr, socklen_t *senderLen) {

    // ipv4
    if (sender.ipVer == IpVersion::IPV4) {

        // non-blocking
        if (socketip4->isNonBlocking()) {
            auto start = std::chrono::steady_clock::now();
            auto timeout_ms = this->timeout;

            ssize_t recv_len = 0;
            do {
                recv_len = recvfrom(socketip4->getSocket(), buffer, bufferSize, 0, senderAddr, senderLen);
                if (recv_len > 0) break;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            } while (std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - start)
                        .count() < timeout_ms);

            return recv_len;
        }

        // timeout set
        return recvfrom(socketip4->getSocket(), buffer, bufferSize, 0, senderAddr, senderLen);
    }

    // ipv6
    // non-blocking
    if (socketip6->isNonBlocking()) {
        auto start = std::chrono::steady_clock::now();
        auto timeout_ms = this->timeout;

        ssize_t recv_len = 0;
        do {
            recv_len = recvfrom(socketip6->getSocket(), buffer, bufferSize, 0, senderAddr, senderLen);
            if (recv_len > 0) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } while (std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - start)
                    .count() < timeout_ms);

        return recv_len;
    }

    // timeout set
    return recvfrom(socketip6->getSocket(), buffer, bufferSize, 0, senderAddr, senderLen);
}

// Constructor for Scanner class
ScannerTCP::ScannerTCP(NetworkAdress sender, NetworkAdress receiver, int timeout) : Scanner(sender, receiver, timeout) {
    // create the syn packet
    synPacket = new SynPacket();   

    // ipv4
    if (sender.ipVer == IpVersion::IPV4) {
        socketip4 = new SocketIpv4(sender, receiver, Protocol::TCP);
        socketip4->setNonBlocking();
        synPacket->constructSynPacketIpv4(*socketip4);
        return;
    }

    // ipv6
    socketip6 = new SocketIpv6(sender, receiver, Protocol::TCP);
    socketip6->setNonBlocking();
    synPacket->constructSynPacketIpv6(*socketip6);
}

// Method for senning the port
ScanResult ScannerTCP::scanPort() {

    ScanResult rslt;
    const char *datagram = synPacket->getPacket();

    // ipv4
    if (sender.ipVer == IpVersion::IPV4) {
        // first send
        sendTo(datagram, sizeof(struct tcphdr));
        rslt = getResponseIpv4();
        if (rslt != ScanResult::UNKNOWN) return rslt;

        // second send
        sendTo(datagram, sizeof(struct tcphdr));
        rslt = getResponseIpv4();
        return (rslt == ScanResult::UNKNOWN) ? ScanResult::FILTERED : rslt;
    } 

    // ipv6
    // first send
    sendTo(datagram, sizeof(struct tcphdr));
    rslt = getResponseIpv6();
    if (rslt != ScanResult::UNKNOWN) return rslt;   

    // second send
    sendTo(datagram, sizeof(struct tcphdr));
    rslt = getResponseIpv6();
    return (rslt == ScanResult::UNKNOWN) ? ScanResult::FILTERED : rslt;
}

// Method to get the response for IPv4
ScanResult ScannerTCP::getResponseIpv4() {
    sockaddr_in in;
    socklen_t in_len = sizeof(in);
    ssize_t recv_len = recvFrom(readBuffer, DATAGRAM_LEN, (struct sockaddr *)&in, &in_len);
    if (recv_len <= 0) return ScanResult::UNKNOWN;

    // Parse incoming packet
    struct iphdr* ip_hdr = (struct iphdr*)readBuffer;
    int ip_header_len = ip_hdr->ihl * 4; 

    // Minimum size for TCP RST packet
    if (recv_len < ip_header_len + 8) return ScanResult::UNKNOWN;
    if (ip_hdr->protocol != IPPROTO_TCP) return ScanResult::UNKNOWN;

    struct tcphdr* tcp_hdr = (struct tcphdr*)(readBuffer + ip_header_len);

    // Ensure response is from the correct source and destination
    if (ip_hdr->daddr != socketip4->getSender().sin_addr.s_addr) return ScanResult::UNKNOWN;

    // Check TCP flags
    if (tcp_hdr->rst) return ScanResult::CLOSED; 
    if (tcp_hdr->ack || tcp_hdr->syn) return ScanResult::OPEN; 

    return ScanResult::UNKNOWN;
}

// Method for receiving the response for IPv6
ScanResult ScannerTCP::getResponseIpv6() {

    struct sockaddr_in6 in;
    socklen_t in_len = sizeof(in);

    ssize_t recv_len = recvFrom(readBuffer, DATAGRAM_LEN, (struct sockaddr *)&in, &in_len);
    if (recv_len <= 0) return ScanResult::UNKNOWN;

    struct tcphdr* tcp_hdr = (struct tcphdr*)readBuffer;

    // Check TCP flags
    if (tcp_hdr->th_flags & TH_RST) return ScanResult::CLOSED; // RST means closed port
    if (tcp_hdr->th_flags & TH_ACK && tcp_hdr->th_flags & TH_SYN) return ScanResult::OPEN; // SYN/ACK means open port

    return ScanResult::UNKNOWN;
}

// Destructor for Scanner class
ScannerTCP::~ScannerTCP() {
    delete synPacket;
}

// Constructor for ScannerUDP class
ScannerUDP::ScannerUDP(NetworkAdress sender, NetworkAdress receiver, int timeout) : Scanner(sender, receiver, timeout) {
    // create the udp packet
    this->udpPacket = new UDPpacket();

    // ipv4
    if (sender.ipVer == IpVersion::IPV4) {
        socketip4 = new SocketIpv4(sender, receiver, Protocol::UDP);
        icmpSocketip4 = new SocketIpv4(sender, receiver, Protocol::ICMP);
        udpPacket->constructUDPpacketIpv4(*socketip4);
        socketip4->setTimeout(timeout);
        icmpSocketip4->setTimeout(timeout);
        return;
    }

    // ipv6
    socketip6 = new SocketIpv6(sender, receiver, Protocol::UDP);
    icmpSocketip6 = new SocketIpv6(sender, receiver, Protocol::ICMP6);
    udpPacket->constructUDPpacketIpv6(*socketip6);
    socketip6->setTimeout(timeout);
    icmpSocketip6->setTimeout(timeout);

}

// Method for scanning the port
ScanResult ScannerUDP::ScanPort() {
    const char* daragram = udpPacket->getPacket();

    // ipv4
    if (sender.ipVer == IpVersion::IPV4) {
        sendTo(daragram, sizeof(struct udphdr));
        return getResponseIpv4();
    }

    // ipv6
    sendTo(daragram, sizeof(struct udphdr));
    return getResponseIpv6();
}

// IPv4 ICMP response handling
ScanResult ScannerUDP::getResponseIpv4() {
    struct sockaddr_in senderAddr;
    socklen_t addrLen = sizeof(senderAddr);
    ssize_t recvLen = recvfrom(icmpSocketip4->getSocket(), readBuffer, sizeof(readBuffer), 0, (struct sockaddr*)&senderAddr, &addrLen);
    if (recvLen < 0) return ScanResult::OPEN;  // No response = Open

    // Ensure the sender matches the expected receiver
    struct sockaddr_in expectedReceiver = socketip4->getReceiver();
    if (senderAddr.sin_addr.s_addr != expectedReceiver.sin_addr.s_addr) return ScanResult::OPEN;

    // Extract the IP header
    struct iphdr *ipHeader = (struct iphdr*)readBuffer;
    int ipHeaderLen = ipHeader->ihl * 4;  // Get IP header length (ihl is in 4-byte words)

    // Check if this is actually an ICMP response
    if (ipHeader->protocol != IPPROTO_ICMP) return ScanResult::OPEN;
    if (recvLen < ssize_t(ipHeaderLen + sizeof(struct icmphdr))) return ScanResult::OPEN;

    // Extract the ICMP header
    struct icmphdr *icmpHeader = (struct icmphdr*)(readBuffer + ipHeaderLen);

    // ICMP Type 3 = Destination Unreachable (filtered ports)
    if (icmpHeader->type == ICMP_DEST_UNREACH) return ScanResult::CLOSED;
    return ScanResult::OPEN;
}


// IPv6 ICMP response handling
ScanResult ScannerUDP::getResponseIpv6() {
    struct sockaddr_in6 senderAddr;
    socklen_t addrLen = sizeof(senderAddr);

    ssize_t recvLen = recvfrom(icmpSocketip6->getSocket(), readBuffer, sizeof(readBuffer), 0, (struct sockaddr*)&senderAddr, &addrLen);
    if (recvLen < 0) return ScanResult::OPEN;  // No response = Open 

    // Ensure the sender matches the expected receiver
    struct sockaddr_in6 expectedReceiver = socketip6->getReceiver();
    if (memcmp(&senderAddr.sin6_addr, &expectedReceiver.sin6_addr, sizeof(struct in6_addr)) != 0) return ScanResult::OPEN;

    // Check if the packet is large enough to contain an ICMPv6 header
    if (recvLen < ssize_t(sizeof(struct icmp6_hdr))) return ScanResult::OPEN;

    // Extract the ICMPv6 header
    struct icmp6_hdr *icmp6Header = (struct icmp6_hdr*)readBuffer;

    // ICMPv6 Type 1 = Destination Unreachable (filtered ports)
    if (icmp6Header->icmp6_type == ICMP6_DST_UNREACH) return ScanResult::CLOSED;
    return ScanResult::OPEN;
}

// Destructor for ScannerUDP class
ScannerUDP::~ScannerUDP() {
    delete udpPacket;
    if (icmpSocketip6 != nullptr) delete icmpSocketip6;
    if (icmpSocketip4 != nullptr) delete icmpSocketip4;
}


// setup the sender and receiver ports
void setupSenderReceiverPorts(NetworkAdress &sender, NetworkAdress &receiver, int port) {
    receiver.port = port;
    sender.port = 49152 + (std::rand() % (65535 - 49152)); // choose a random port
}

// scan the TCP port
void scanPortTCP(NetworkAdress sender, NetworkAdress receiver, int port, int timeout) {

    if (sender.ip.empty() || receiver.ip.empty()) return;
    setupSenderReceiverPorts(sender, receiver, port);

    // create the scanner
    ScannerTCP *scannerTCP = new ScannerTCP(sender, receiver, timeout);
    // scan the port
    ScanResult result = scannerTCP->scanPort();
    // print the result
    std::cout << receiver.ip << " " << port << " tcp " << toString(result) << std::endl;
    delete scannerTCP;
}

// scan the UDP port
void scanPortUDP(NetworkAdress sender, NetworkAdress receiver, int port, int timeout) {

    if (sender.ip.empty() || receiver.ip.empty()) return;
    setupSenderReceiverPorts(sender, receiver, port);

    // create the scanner
    ScannerUDP *scannerUDP = new ScannerUDP(sender, receiver, timeout);
    // scan the port
    ScanResult result = scannerUDP->ScanPort();
    // print the result
    std::cout << receiver.ip << " " << port << " udp " << toString(result) << std::endl;
    delete scannerUDP;
}
