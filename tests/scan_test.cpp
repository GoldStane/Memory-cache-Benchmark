#include "scan.h"
#include <iostream>
#include <cstdint>
#include <vector>

int main() {
    const std::vector<std::uint64_t> test_vec1 = {}; // empty input
    const std::vector<std::uint64_t> test_vec2 = {7}; // vector with 1 number only
    const std::vector<std::uint64_t> test_vec3 = {1, 2, 3, 4, 5}; // vector with several numbers

    const auto checksum1{sequential_scan(test_vec1)};
    const auto checksum2{sequential_scan(test_vec2)};
    const auto checksum3{sequential_scan(test_vec3)};

    bool fail{false};

    if (checksum1 != 0) {
        fail = true;
        std::cout << "Test 1 got incorrect checksum\n";
        std::cout << "Expected checksum: 0; calculated checksum: " << checksum1 << "\n"; 
    }

    if (checksum2 != 7) {
        fail = true;
        std::cout << "Test 2 got incorrect checksum\n";
        std::cout << "Expected checksum: 7; calculated checksum: " << checksum2 << "\n"; 
    }

    if (checksum3 != 15) {
        fail = true;
        std::cout << "Test 3 got incorrect checksum\n";
        std::cout << "Expected checksum: 15; calculated checksum: " << checksum3 << "\n"; 
    }

    if (fail) {
        std::cout << "Exiting program...\n";
        return 1;
    }

    return 0;
}