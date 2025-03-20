/**
 * @file scanning.hpp
 * @brief Header file for scanning (sending and receiving packets and scanning ports)
 * @author Martin Mendl <x247581>
 * @date 2025-27-02
 */

#ifndef SCANNING_HPP
#define SCANNING_HPP

#include <iostream>
#include <string>
#include <netinet/in.h>
#include "sockets.hpp"
#include "packets.hpp"

// enum for port scan results
enum class ScanResult {
    OPEN,
    CLOSED,
    FILTERED,
    INCOMPLETE,
    UNKNOWN
};

/**
 * @brief Function to convert the scan result to string
 * 
 * @param result - scan result
 * @return const char* - string representation of the scan result
*/
inline const char* toString(ScanResult result) {
    switch (result) {
        case ScanResult::OPEN: return "open";
        case ScanResult::CLOSED: return "closed";
        case ScanResult::FILTERED: return "filtered";
        case ScanResult::INCOMPLETE: return "incomplete";
        case ScanResult::UNKNOWN: return "unknown";
        default: return "error";
    }
}

/**
 * @brief Function to setup the sender and receiver ports
 * 
 * @param sender - sender network address
 * @param receiver - receiver network address
 * @param port - port number
 */
void setupSenderReceiverPorts(NetworkAdress &sender, NetworkAdress &receiver, int port);

/**
 * @brief Function to scan the TCP port
 * 
 * @param sender - sender network address
 * @param receiver - receiver network address
 * @param port - port number
 * @param timeout - timeout for the scan
 */
void scanPortTCP(NetworkAdress sender, NetworkAdress receiver, int port, int timeout);


/**
 * @brief Function to scan the UDP port
 * 
 * @param sender - sender network address
 * @param receiver - receiver network address
 * @param port - port number
 * @param timeout - timeout for the scan
*/
void scanPortUDP(NetworkAdress sender, NetworkAdress receiver, int port, int timeout);


/**
 * @class Scanner
 * @brief Class for scanning ports
 */
class Scanner {
    public:
        /**
         * @brief Constructor for Scanner class
         * 
         * @param sender - sender network address
         * @param receiver - receiver network address
         * @param timeout - timeout for the scan
         */
        Scanner(NetworkAdress sender, NetworkAdress receiver, int timeout);
        /**
         * @brief Destructor for Scanner class
         */
        ~Scanner();
    protected:
        /**
         * @brief Method to send the packet
         * 
         * @param datagram - datagram to send
         * @param datagramSize - size of the datagram
         */
        void sendTo(const char *datagram, size_t datagramSize);
        /**
         * @brief Method to receive the packet
         * 
         * @param buffer - buffer to store the packet
         * @param bufferSize - size of the buffer
         * @return ssize_t - size of the received packet
         */
        ssize_t recvFrom(char *buffer, size_t bufferSize, struct sockaddr *senderAdrr, socklen_t *senderLen);

        NetworkAdress sender;                   // sender network address
        NetworkAdress receiver;                 // receiver network address
        SocketIpv4* socketip4 = nullptr;        // socket for IPv4
        SocketIpv6* socketip6 = nullptr;        // socket for IPv6
        SocketIpv4* icmpSocketip4 = nullptr;    // ICMP socket for IPv4
        SocketIpv6* icmpSocketip6 = nullptr;    // ICMP socket for IPv6
        char readBuffer[DATAGRAM_LEN];          // buffer for reading the packet
        int timeout;                            // timeout for the scan
};

/**
 * @class ScannerTCP
 * @brief Class for scanning TCP ports
 */
class ScannerTCP : public Scanner {
    public:
        /**
         * @brief Constructor for ScannerTCP class
         * 
         * @param sender - sender network address
         * @param receiver - receiver network address
         * @param timeout - timeout for the scan
         */
        ScannerTCP(NetworkAdress sender, NetworkAdress receiver, int timeout);
        /**
         * @brief Destructor for ScannerTCP class
         */
        ~ScannerTCP();
        /**
         * @brief Method to scan the port
         * 
         * @return ScanResult - scan result
         */
        ScanResult scanPort();
    private:
        /**
         * @brief Method to get the response for IPv4
         * @return ScanResult - scan result
         */
        ScanResult getResponseIpv4();
        /**
         * @brief Method to get the response for IPv6
         * @return ScanResult - scan result
         */
        ScanResult getResponseIpv6();
        SynPacket* synPacket;            // SYN packet
};

/**
 * @class ScannerUDP
 * @brief Class for scanning UDP ports
 */
class ScannerUDP : public Scanner{
    public:
        /**
         * @brief Constructor for ScannerUDP class
         * 
         * @param sender - sender network address
         * @param receiver - receiver network address
         * @param timeout - timeout for the scan
         */
        ScannerUDP(NetworkAdress sender, NetworkAdress receiver, int timeout);
        /**
         * @brief Destructor for ScannerUDP class
         */
        ~ScannerUDP();
        /**
         * @brief Method to scan the port
         * 
         * @return ScanResult - scan result
         */
        ScanResult scanPort();
    private:
        /**
         * @brief Method to get the response for IPv4
         * @return ScanResult - scan result
         */
        ScanResult getResponseIpv6();
        /**
         * @brief Method to get the response for IPv6
         * @return ScanResult - scan result
         */
        ScanResult getResponseIpv4();
        UDPpacket *udpPacket;       // UDP packet
};

#endif // SCANNING_HPP
