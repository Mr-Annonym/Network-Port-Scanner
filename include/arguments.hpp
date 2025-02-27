/**
 * @file arguments.hpp
 * @brief Header file for parsing arguments
 * @author Martin Mendl <x247581>
 * @date 2025-27-02
 */

#ifndef ARGUMENTS_HPP
#define ARGUMENTS_HPP

#include <vector>
#include <string>
#include "utils.hpp"

/**
 * @enum Mode
 * @brief Enumeration for different scanning modes.
 */
enum class Mode {
    SCAN,
    PRINT_INTERFACES,
    UNKNOWN
};

/**
 * @enum Mode
 * @brief Enumeration for different scanning modes.
 */
enum class TargetType {
    IP_v4,
    IP_v6,
    DOMAIN_NAME,
    UNKNOWN
};

const int MAX_PORT_NUMBER = 65535;

/**
 * @brief Function to get the target IP from the domain name
 * 
 * @param domain The domain name
 * @return std::string The target IP
*/
Target getTargetIPfromDomain(const std::string& domain);

/**
 * @brief Function to parse the ports
 * 
 * @param ports The string containing the ports
 * @return std::vector<int> The vector containing the ports
*/
std::vector<int> parsePorts(const std::string &ports);

/**
 * @brief Function to determine the target type
 * 
 * @param target The target string
 * @return TargetType The target type
*/
TargetType determinTargetType(const std::string &target);

/**
 * @brief Function to get the target IP from the domain name
 * 
 * @param domain The domain name
 * @return std::string The target IP
*/
Target getTargetIPfromDomain(const std::string& domain);

/**
 * @class Arguments
 * @brief Parses and stores command-line arguments for the scanner.
 *
 * This class processes command-line arguments, storing relevant information
 * such as the network interface, target, port lists, and timeout settings.
 */
class Settings {
    public:
        
        /**
         * @brief Constructs an Arguments object by parsing command-line arguments.
         * 
         * @param argc Number of command-line arguments.
         * @param argv Array of command-line arguments.
         */
        Settings(int argc, char *argv[]);

        /**
         * @brief Destructor for the Arguments class.
         */
        std::string getInterface() const;

        /**
         * @brief Retrieves the list of TCP ports.
         * @return A vector of integers representing the TCP ports.
        */
        std::vector<int> getTCPports() const;

        /**
         * @brief Retrieves the list of UDP ports.
         * @return A vector of integers representing the UDP ports.
        */
        std::vector<int> getUDPports() const;

        /**
         * @brief Retrieves the timeout value.
         * @return An integer representing the timeout in milliseconds.
        */
        int getTimeout() const;

        /**
         * @brief Retrieves the target information.
         * @return A Target object containing the target information.
        */
        Target getTarget() const;

        /**
         * @brief Retrieves the operation mode.
         * @return A Mode enum value representing the operation mode.
        */
        Mode getMode() const;

        /**
         * @brief Prints the command-line arguments.
        */
        void representArguments() const;

    private:

        std::string interfaceName;     // network interface
        std::vector<int> TCPports;     // tcp ports
        std::vector<int> UDPports;     // udp ports
        int timeout = 5000;             // timeout
        Target target;                  // target
        Mode mode = Mode::UNKNOWN;      // operation mode

};

#endif // ARGUMENTS_HPP