#include "scan.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <random>
#include <algorithm>

int main() {
    constexpr std::size_t trials = 20; // number of trials for accessing memory
    constexpr std::size_t scans_per_trial = 100; // scans in each measured batch

    // formatting
    constexpr int size_width = 10;
    constexpr int elements_width = 12;
    constexpr int timing_width = 17;
    constexpr int per_element_width = 20;
    constexpr int throughput_width = 14;
    constexpr int table_width = size_width + elements_width + 3 * timing_width
                              + per_element_width + throughput_width + 6 * 3;
    const std::string separator(table_width, '-');

    std::cout << "Trials per size: " << trials
              << " | Scans per trial: " << scans_per_trial
              << " | Warm-up scans: 1\n\n"
              << std::right
              << std::setw(size_width) << "Size (KiB)" << " | "
              << std::setw(elements_width) << "Elements" << " | "
              << std::setw(timing_width) << "Min batch ns" << " | "
              << std::setw(timing_width) << "Median batch ns" << " | "
              << std::setw(timing_width) << "Max batch ns" << " | "
              << std::setw(per_element_width) << "Median ns/element" << " | "
              << std::setw(throughput_width) << "Useful GB/s" << '\n'
              << separator << '\n';

    for (std::size_t bytes = 4 * 1024; bytes <= 256 * 1024 * 1024; bytes *= 2) {
        std::vector<std::chrono::nanoseconds> time_vec;
        std::vector<std::uint64_t> checksum_vec;
        time_vec.reserve(trials);
        checksum_vec.reserve(trials);

        std::vector<std::uint64_t> vec(bytes / sizeof(std::uint64_t));

        std::mt19937_64 eng(55);
        std::uniform_int_distribution<std::uint64_t> dist(0, 1000);

        std::uint64_t expected_checksum{0};

        for (auto& val : vec) {
            val = dist(eng);
            expected_checksum += val;
        }

        // warmup
        const auto checksum{sequential_scan(vec)};

        if (checksum != expected_checksum) {
            std::cout << "Warmup got an incorrect checksum\n";
            std::cout << "Expected checksum: " << expected_checksum << "; calculated checksum: " << checksum << "\n";
            std::cout << "Performance summary is skipped, exiting program...\n";
            return 1;
        }

        // actual trials
        for (std::size_t i = 0; i < trials; ++i) {
            std::uint64_t checksum_batch{0};
            const auto start{std::chrono::steady_clock::now()};

            for (std::size_t j = 0; j < scans_per_trial; ++j) {
                const auto checksum{sequential_scan(vec)};
                checksum_batch += checksum;
            }
            const auto finish{std::chrono::steady_clock::now()};

            time_vec.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start));
            checksum_vec.push_back(checksum_batch);
        }

        const std::uint64_t expected_batch_checksum = expected_checksum * scans_per_trial;

        // checking the checksum
        for (std::size_t i = 0; i < trials; ++i) {
            if (checksum_vec[i] != expected_batch_checksum) {
                std::cout << "Trial " << i + 1 << " got an incorrect checksum\n";
                std::cout << "Expected checksum: " << expected_batch_checksum << "; calculated checksum: " << checksum_vec[i] << "\n";
                std::cout << "Performance summary is skipped, exiting program...\n";
                return 1;
            }
        }

        // median calculation
        std::sort(time_vec.begin(), time_vec.end());

        const auto median = (trials % 2 == 0) ? (time_vec[trials / 2 - 1] + time_vec[trials / 2]) / 2.0 : time_vec[trials / 2];

        // Batch durations retain half-nanosecond medians; normalised metrics use three decimals.
        std::cout << std::fixed
                  << std::setw(size_width) << bytes / 1024 << " | "
                  << std::setw(elements_width) << vec.size() << " | "
                  << std::setw(timing_width) << time_vec.front().count() << " | "
                  << std::setprecision(1)
                  << std::setw(timing_width) << median.count() << " | "
                  << std::setw(timing_width) << time_vec.back().count() << " | "
                  << std::setprecision(3)
                  << std::setw(per_element_width)
                  << median.count() / (scans_per_trial * vec.size()) << " | "
                  << std::setw(throughput_width)
                  << (scans_per_trial * bytes) / median.count() << '\n';
    }
    std::cout << separator << '\n';
    return 0;
}
