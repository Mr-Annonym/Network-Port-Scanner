/**
 * @file packets.cpp
 * @brief File for packet creation and manipulation
 * @author Martin Mendl <x247581>
 * @date 2025-15-03
 */

#include <string.h>
#include "packets.hpp"
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

// Constructor for base Packet class
Packet::Packet() {
    memset(&psh, 0, sizeof(psh));
    memset(&psh6, 0, sizeof(psh6));
    memset(datagram, 0, DATAGRAM_LEN);
}

// Destructor for Packet class
Packet::~Packet() {
    delete[] datagram;
}

// Constructor for SynPacket class
SynPacket::SynPacket() : Packet() {
    // Point to TCP header in datagram
    this->tcph = (struct tcphdr*)(datagram);

    // basic TCP header setup
    tcph->th_seq = htonl((uint32_t)rand());
    tcph->th_ack = htonl(0);
    tcph->th_off = 5; 
    tcph->th_flags = TH_SYN;
    tcph->th_win = htons(5840);
    tcph->th_sum = 0;
    tcph->th_urp = 0;
}

// Method to create the SYN packet for IPv4
void SynPacket::constructSynPacketIpv4(const SocketIpv4 &socket) {

    // TCP header setup
    tcph->th_sport = socket.getSender().sin_port;
    tcph->th_dport = socket.getReceiver().sin_port;

    // Prepare pseudo-header
    memset(&psh, 0, sizeof(struct pseudoHeaderIpv4));
    psh.sourceAdress = socket.getSender().sin_addr.s_addr;
    psh.destAdress = socket.getReceiver().sin_addr.s_addr;
    psh.tmp = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcphdr));

    // Calculate checksum
    int psize = sizeof(struct pseudoHeaderIpv4) + sizeof(struct tcphdr);
    std::vector<char> psdgram(psize);
    memcpy(psdgram.data(), &psh, sizeof(struct pseudoHeaderIpv4));
    memcpy(psdgram.data() + sizeof(struct pseudoHeaderIpv4), tcph, sizeof(struct tcphdr));

    tcph->th_sum = checkSum(psdgram.data(), psize);

}

// Method to create the SYN packet for IPv6
void SynPacket::constructSynPacketIpv6(const SocketIpv6 &socket) {

    // Configure TCP header fields
    tcph->th_sport = socket.getSender().sin6_port; // poroblem here
    tcph->th_dport = socket.getReceiver().sin6_port; // problem here

    // Prepare IPv6 pseudo header for checksum calculation
    memset(&psh6, 0, sizeof(psh6));
    psh6.sourceAddress = socket.getSender().sin6_addr;
    psh6.destAddress = socket.getReceiver().sin6_addr;
    psh6.tcp_length = htonl(sizeof(struct tcphdr));
    psh6.nextHeader = IPPROTO_TCP;

    // Create pseudo packet for checksum calculation
    int psize = sizeof(struct pseudoHeaderIpv6) + sizeof(struct tcphdr);
    std::vector<char> psdgram(psize);
    memcpy(psdgram.data(), &psh6, sizeof(struct pseudoHeaderIpv6));
    memcpy(psdgram.data() + sizeof(struct pseudoHeaderIpv6), tcph, sizeof(struct tcphdr));

    // Calculate checksum
    tcph->th_sum = checkSum(psdgram.data(), psize);
}


// Constructor for UDPpacket class
UDPpacket::UDPpacket() : Packet() {

    memset(&psh, 0, sizeof(psh));
    memset(&psh6, 0, sizeof(psh6));
    memset(datagram, 0, DATAGRAM_LEN);

    // Point to the correct offset for UDP header
    this->udph = (struct udphdr*)(datagram);

    udph->uh_ulen = htons(sizeof(struct udphdr));
    udph->uh_sum = 0;
}

// Method to create the UDP packet for IPv4
void UDPpacket::constructUDPpacketIpv4(const SocketIpv4 &socket) {

    // Configure UDP header fields
    udph->uh_sport = socket.getSender().sin_port;
    udph->uh_dport = socket.getReceiver().sin_port;

    // Prepare pseudo-header for checksum calculation
    memset(&psh, 0, sizeof(struct pseudoHeaderIpv4));
    psh.sourceAdress = socket.getSender().sin_addr.s_addr;
    psh.destAdress = socket.getReceiver().sin_addr.s_addr;
    psh.tmp = 0;
    psh.protocol = IPPROTO_UDP;
    psh.tcp_length = htons(sizeof(struct udphdr));

    // Calculate checksum
    int psize = sizeof(struct pseudoHeaderIpv4) + sizeof(struct udphdr);
    std::vector<char> psdgram(psize);
    memcpy(psdgram.data(), &psh, sizeof(struct pseudoHeaderIpv4));
    memcpy(psdgram.data() + sizeof(struct pseudoHeaderIpv4), udph, sizeof(struct udphdr));

    udph->uh_sum = checkSum(psdgram.data(), psize);

}

// Method to create the UDP packet for IPv6
void UDPpacket::constructUDPpacketIpv6(const SocketIpv6 &socket) {

    // Point to the correct offset for UDP header
    this->udph = (struct udphdr*)(datagram);

    // Configure UDP header fields
    udph->uh_sport = socket.getSender().sin6_port;
    udph->uh_dport = socket.getReceiver().sin6_port;

    // Prepare pseudo-header for checksum calculation
    memset(&psh6, 0, sizeof(psh6));
    psh6.sourceAddress = socket.getSender().sin6_addr;
    psh6.destAddress = socket.getReceiver().sin6_addr;
    psh6.tcp_length = htons(sizeof(struct udphdr));
    psh6.nextHeader = IPPROTO_UDP;

    // Create pseudo packet for checksum calculation
    int psize = sizeof(struct pseudoHeaderIpv6) + sizeof(struct udphdr);
    std::vector<char> psdgram(psize);
    memcpy(psdgram.data(), &psh6, sizeof(struct pseudoHeaderIpv6));
    memcpy(psdgram.data() + sizeof(struct pseudoHeaderIpv6), udph, sizeof(struct udphdr));

    // Calculate checksum
    udph->uh_sum = checkSum(psdgram.data(), psize);
}