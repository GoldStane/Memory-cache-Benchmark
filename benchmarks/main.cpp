#include "scan.h"
#include <iostream>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <random>
#include <algorithm>

int main() {
    constexpr std::size_t trials = 20;

    std::vector<std::chrono::nanoseconds> time_vec;
    std::vector<std::uint64_t> checksum_vec;
    time_vec.reserve(trials);
    checksum_vec.reserve(trials);
    
    std::vector<std::uint64_t> vec(1024 * 1024 / sizeof(std::uint64_t));

    std::mt19937_64 eng(55);
    std::uniform_int_distribution<std::uint64_t> dist(0, 1000);

    std::uint64_t expected_checksum{0};

    for (auto& val : vec) {
        val = dist(eng);
        expected_checksum += val;
    }
    
    // warmup
    const auto start{std::chrono::steady_clock::now()};
    const auto checksum{sequential_scan(vec)};
    const auto finish{std::chrono::steady_clock::now()};
    const auto time{std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start)};

    if (checksum != expected_checksum) {
        std::cout << "Warmup got an incorrect checksum\n";
        std::cout << "Expected checksum: " << expected_checksum << "; calculated checksum: " << checksum << "\n";
        std::cout << "Perfomance summary is skipped, exiting program...\n";
        return 1;
    }

    // actual trials
    for (std::size_t i = 0; i < trials; ++i) {
        const auto start{std::chrono::steady_clock::now()};
        const auto checksum{sequential_scan(vec)};
        const auto finish{std::chrono::steady_clock::now()};

        time_vec.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start));
        checksum_vec.push_back(checksum);
    }

    // checking the checksum
    for (std::size_t i = 0; i < trials; i++) {
        if (checksum_vec[i] != expected_checksum) {
            std::cout << "Trial " << i + 1 << " got an incorrect checksum\n";
            std::cout << "Expected checksum: " << expected_checksum << "; calculated checksum: " << checksum_vec[i] << "\n";
            std::cout << "Performance summary is skipped, exitting program...\n";
            return 1;
        }
    }
    
    // median and mean calculation
    std::sort(time_vec.begin(), time_vec.end());
    std::chrono::nanoseconds sum{};

    for (const auto& dur : time_vec) {
        sum += dur;
    }

    const auto median = (trials % 2 == 0) ? (time_vec[trials / 2 - 1] + time_vec[trials / 2]) / 2.0 : time_vec[trials / 2];

    // warmup timings
    std::cout << "WARMUP RESULTS\n";
    std::cout << "Number of elements: " << vec.size() << "\n";
    std::cout << "Checksum: " << checksum << "\n";
    std::cout << "Execution time: " << time.count() << "ns\n";
    std::cout << "Nanoseconds per element: " << static_cast<double>(time.count()) / vec.size() << "ns\n\n";

    // actual timings
    std::cout << "ACTUAL RESULTS\n";
    std::cout << "Fastest time: " << time_vec.front().count() << "ns\n";
    std::cout << "Slowest time: " << time_vec.back().count() << "ns\n";
    std::cout << "Median time: " << median.count() << "ns\n";
    std::cout << "Mean time: " << static_cast<double>(sum.count()) / trials << "ns\n";
    std::cout << "Nanoseconds per element (median): " << median.count() / vec.size() << "ns\n";

    return 0;
}