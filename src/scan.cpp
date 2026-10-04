#include "scan.h"

std::uint64_t sequential_scan(const std::vector<std::uint64_t> &values) {
    std::uint64_t checksum{0};

    for (const auto &value : values) {
        checksum += value;
    }
    return checksum;
}