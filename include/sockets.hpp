/**
 * @file sockets.hpp
 * @brief Header file for socket creation and manipulation
 * @author Martin Mendl <x247581>
 * @date 2025-16-03
 */

#ifndef SOCKETS_HPP
#define SOCKETS_HPP

#include <iostream>
#include <string>
#include "utils.hpp"

/**
 * @enum Protocol
 * @brief Enumeration for different protocols
 */
enum class Protocol {
    TCP,
    UDP,
    ICMP,
    ICMP6
};

/**
 * @brief Function to return the protocol
 * 
 * @param protocol The protocol
 * @return int The protocol number
 */
int returnProtocol(Protocol protocol);

/**
 * @class Socket
 * @brief Base class for socket creation
*/
class Socket {
    public:
        /**
         * @class Socket
         * @brief Constructor for Socket class
        */
        Socket(NetworkAdress sender, NetworkAdress receiver);

        /**
         * @brief Method to get the socket
         * 
         * @return int The socket
        */
        int getSocket() const { return sockfd; };

        /**
         * @brief Method to set the socket to non-blocking
        */
        void setNonBlocking();

        /**
         * @brief Method to set the timeout for the socket
         * 
         * @param timeout The timeout
        */
        void setTimeout(int timeout);
        
        /**
         * @brief Method to bind the socket to the sender interface
        */
        virtual ~Socket();

        /**
         * @brief Method to bind the socket to the sender interface
        */
        bool isNonBlocking() const { return nonBlocking; };

        /**
         * @brief Method to bind the socket to the sender interface
        */
        bool isTimeoutSet() const { return timeoutSet; };
        
    protected:
        /**
         * @brief Method to bind the socket to the sender interface
        */
        void bindToInterface();
        /**
         * @brief Method to bind the socket to the sender interface
        */
        virtual void setupNetworkAdress() {};

        int sockfd;                 // socket   
        NetworkAdress sender;       // sender network address
        NetworkAdress receiver;     // receiver network address
        struct timeval timeout;     // timeout
        bool nonBlocking = false;   // non-blocking flag 
        bool timeoutSet = false;    // timeout flag
};

/**
 * @class SocketIpv4
 * @brief Class for creating IPv4 sockets
 */
class SocketIpv4 : public Socket {
    public:
        /**
         * @brief Constructor for SocketIpv4 class
         * 
         * @param sender - sender network address
         * @param receiver - receiver network address
         * @param protocol - protocol
         */
        SocketIpv4(NetworkAdress sender, NetworkAdress receiver, Protocol protocol);

        /**
         * @brief Destructor for SocketIpv4 class
         */
        ~SocketIpv4() override {};

        /**
         * @brief Method to get the sender address
         * 
         * @return struct sockaddr_in The sender address
         */
        struct sockaddr_in getSender() const { return senderAddr; };

        /**
         * @brief Method to get the receiver address
         * 
         * @return struct sockaddr_in The receiver address
         */
        struct sockaddr_in getReceiver() const { return receiverAddr; };
    private:
        void setupNetworkAdress() override {};
        /**
         * @brief Method to set the network adress for the socket
         * 
         * @param adress The network address
         * @param sockAddr The socket address
         * @return void
        */
        void setupNetworkAdress(NetworkAdress &adress, struct sockaddr_in &sockAddr);

        struct sockaddr_in senderAddr;    // sender address
        struct sockaddr_in receiverAddr;  // receiver address
};

/**
 * @class SocketIpv6
 * @brief Class for creating IPv6 sockets
*/
class SocketIpv6 : public Socket {
    public:
        /**
         * @brief Constructor for SocketIpv6 class
         * 
         * @param sender - sender network address
         * @param receiver - receiver network address
         * @param protocol - protocol
        */
        SocketIpv6(NetworkAdress sender, NetworkAdress receiver, Protocol protocol);

        /**
         * @brief Destructor for SocketIpv6 class
        */
        ~SocketIpv6() override {};

        /**
         * @brief Method to get the sender address
         */
        struct sockaddr_in6 getSender() const { return senderAddr; };

        /**
         * @brief Method to get the receiver address
         */
        struct sockaddr_in6 getReceiver() const { return receiverAddr; };

    private:
        void setupNetworkAdress() override {};
        /**
         * @brief Method to set the network adress for the socket
         * 
         * @param adress The network address
         * @param sockAddr The socket address
         * @return void
        */
        void setupNetworkAdress(NetworkAdress &adress, struct sockaddr_in6 &sockAddr);

        struct sockaddr_in6 senderAddr;     // sender address
        struct sockaddr_in6 receiverAddr;   // receiver address
};

#endif // SOCKETS_HPP