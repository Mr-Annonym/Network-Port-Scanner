/**
 * @file packets.hpp
 * @brief header file for packets
 * @author Martin Mendl <x247581>
 * @date 2025-15-03
 */

#ifndef PACKETS_HPP
#define PACKETS_HPP

#include <netinet/in.h>
#include "sockets.hpp"

#define DATAGRAM_LEN 4096

struct pseudoHeaderIpv4 {
    u_int32_t sourceAdress;
    u_int32_t destAdress;
    u_int8_t tmp;
    u_int8_t protocol;
    u_int16_t tcp_length;
};

/**
 * @struct pseudoHeaderIpv6
 * @brief Represents the pseudo-header structure used in IPv6 for TCP checksum calculation.
 */
struct pseudoHeaderIpv6 {
    struct in6_addr sourceAddress; // 128-bit source IP address
    struct in6_addr destAddress;   // 128-bit destination IP address
    uint32_t tcp_length;           // TCP segment length (excluding IPv6 header)
    uint8_t zero[3];               // Three bytes of zero padding
    uint8_t nextHeader;            // Next Header (should be IPPROTO_TCP)
};

/**
 * @class Packet
 * @brief Base class for packet creation
 */
class Packet {
    public:
        /**
         * @brief Constructor for Packet class
         */
        Packet();
        /**
         * @brief Destructor for Packet class
         */
        virtual ~Packet();
        /**
         * @brief Method to get the packet
         * 
         * @return char* The packet
         */
        char *getPacket() const { return datagram; };
    protected:
        struct pseudoHeaderIpv4 psh;                // pseudo header for checksum calculation
        struct pseudoHeaderIpv6 psh6;               // pseudo header for checksum calculation
        char *datagram = new char[DATAGRAM_LEN]();  // packet buffer
};

/**
 * @class SynPacket
 * @brief Class for creating SYN packets
 */
class SynPacket : public Packet {
    public:
        /**
         * @brief Constructor for SynPacket class
         */
        SynPacket();
        /**
         * @brief Destructor for SynPacket class
         */
        ~SynPacket() override {};
        /**
         * @brief Method to create the SYN packet for IPv4
         * 
         * @param socket The socket to use
         */
        void constructSynPacketIpv4(const SocketIpv4 &socket);
        /**
         * @brief Method to create the SYN packet for IPv6
         * 
         * @param socket The socket to use
         */
        void constructSynPacketIpv6(const SocketIpv6 &socket);
    private:
        struct tcphdr *tcph; // TCP header
};
    
/**
 * @class UDPpacket
 * @brief Class for creating UDP packets
*/
class UDPpacket : public Packet {
    public:
        /**
         * @brief Constructor for UDPpacket class
         */
        UDPpacket();
        /**
         * @brief Destructor for UDPpacket class
         */
        ~UDPpacket() override {};
        /**
         * @brief Method to create the UDP packet for IPv4
         * 
         * @param socket The socket to use
         */
        void constructUDPpacketIpv4(const SocketIpv4 &socket);
        /**
         * @brief Method to create the UDP packet for IPv6
         * 
         * @param socket The socket to use
         */
        void constructUDPpacketIpv6(const SocketIpv6 &socket);
    private:
        struct udphdr *udph; // UDP header
};

#endif // PACKETS_HPP