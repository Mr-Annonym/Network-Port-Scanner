/**
 * @file main.cpp
 * @brief Main file for the IPK project
 * @author Martin Mendl <x247581>
 * @date 2025-27-02
*/

#include <iostream>
#include "arguments.hpp"
#include "network.hpp"
#include "utils.hpp"

int main(int argc, char *argv[]) {
    // Parse arguments
    Settings settings(argc, argv);
    settings.representArguments();
    return 0;
}