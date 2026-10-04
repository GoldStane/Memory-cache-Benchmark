#include <iostream>
#include <cstddef>
#include <chrono>
#include <vector>
#include <algorithm>

int main() {
    const std::size_t iterations{100000};
    std::vector<std::chrono::nanoseconds> results;
    results.reserve(iterations);

    for (std::size_t i = 0; i < iterations; ++i) {
        const auto start{std::chrono::steady_clock::now()};
        const auto finish{std::chrono::steady_clock::now()};
        const auto elapsed{std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start)};

        results.push_back(elapsed);
    }

    std::sort(results.begin(), results.end());

    std::cout << "Minimum overhead: " << results.front().count() << "ns\n";
    std::cout << "Maximum overhead: " << results.back().count() << "ns\n";
    
    if (iterations % 2 == 0)
        std::cout << "Median overhead: " << (results[iterations / 2 - 1].count() + results[iterations / 2].count()) / 2.0 << "ns\n";
    else
        std::cout << "Median overhead: " << results[iterations / 2].count() << "ns\n";
    
    return 0;
}