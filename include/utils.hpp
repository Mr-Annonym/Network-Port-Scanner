/**
 * @file utils.hpp
 * @brief Header file for utility functions for the whole project
 * @author Martin Mendl <x247581>
 * @date 2025-27-02
*/

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

/**
 * @enum ipVersion
 * @brief Enumeration for different IP versions
 */
enum class ipVersion {
    IPV4,
    IPV6
};

/**
 * @struct NetworkAdress
 * @brief Represents the network address
 */
enum class IpVersion {
    IPV4,
    IPV6
};

/**
 * @struct NetworkAdress
 * @brief Represents the network address
 */
struct NetworkAdress {
    std::string hostName;
    std::string ip;
    IpVersion ipVer;
    int port;
};

/**
 * @brief Function returning the available network interfaces
 * 
 * @return std::vector<NetworkInterface> The vector containing the network interfaces
*/
std::vector<NetworkAdress> getNetworkInterfaces();

/**
 * @brief Function to represent the available network interfaces    
 * 
 * @return void
*/
void representInterfaces(std::vector<NetworkAdress> interfaces);


/**
 * @brief Function to validate the network interface
 * 
 * @param interfaces The vector containing the network interfaces
 * @param interface_name The name of the interface
 * @return NetworkInterface* The pointer to the network interface
*/
NetworkAdress validateInterface(std::vector<NetworkAdress>& interfaces, const std::string& interface_name, bool ipv4);

/**
 * @brief Function to calc the checksum
 * 
 * @param buf The buffer
 * @param len The length of the buffer
 * @return unsigned short The checksum
*/
unsigned short checkSum(const char *buf, unsigned size);

#endif // UTILS_HPP