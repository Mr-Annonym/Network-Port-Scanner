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

// struct to represetn entwork interface
struct NetworkInterface {
    std::string name;
    std::string address;
};

// struct to represent the target IP
struct Target {
    std::string domain;
    std::string ip;
    int family;
};



#endif // UTILS_HPP