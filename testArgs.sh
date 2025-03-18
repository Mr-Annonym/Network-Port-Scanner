#!/bin/bash
# TEST SCRIPT FOR ARGTEST
#  * 
#  * @file test_testArgs.sh
#  * @brief This script runs unit tests for the argTest binary.
#  */

# ANSI escape codes for colored output
RESET="\033[0m"
RED="\033[1;31m"
GREEN="\033[1;32m"
YELLOW="\033[1;33m"
BLUE="\033[1;34m"
MAGENTA="\033[1;35m"

# Decorations
SEPARATOR="${BLUE}========================================================${RESET}"

# Helper function to print a formatted message
print_section() {
    echo -e "\n$SEPARATOR"
    echo -e "${MAGENTA}$1${RESET}"
    echo -e "$SEPARATOR"
}

# Helper function to run a test and check its result
run_test() {
    local description=$1
    local command=$2
    local expected=$3

    echo -e "${YELLOW}Running: $description${RESET}"
    eval "$command" > output.txt
    if grep -q "$expected" output.txt; then
        echo -e "${GREEN}Test passed.${RESET}"
    else
        echo -e "${RED}Test failed.${RESET}"
    fi
    rm -f output.txt
}

# Clean and build
print_section "CLEANING AND BUILDING"
make clean && make argTest
if [ ! -f "./argTest" ]; then
    echo -e "${RED}Error: argTest binary not created.${RESET}"
    exit 1
fi

# Run tests
print_section "RUNNING TESTS"

run_test "Help message" "./argTest --help" "Interface to use for scanning"
run_test "Interface argument" "./argTest -i eth0 example.com" "eth0"
run_test "TCP ports" "./argTest -t 80,443 example.com" "80 443"
run_test "UDP ports" "./argTest -u 53,123 example.com" "53 123"
run_test "Target IPv4" "./argTest -i eth0 -t 80 -u 53 192.168.1.1" "192.168.1.1"
run_test "Target IPv6" "./argTest -i eth0 -t 80 -u 53 fe80::1" "fe80::1"
run_test "Timeout" "./argTest -w 5000 example.com" "5000"

# Cleanup
print_section "CLEANING UP"
make clean

echo -e "${GREEN}All tests completed.${RESET}"
exit 0