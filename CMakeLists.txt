cmake_minimum_required(VERSION 3.20)

project(CacheMemoryBenchmark VERSION 0.1 LANGUAGES CXX)

add_executable(main benchmarks/main.cpp src/scan.cpp)
target_include_directories(main PRIVATE src)
target_compile_features(main PRIVATE cxx_std_17)

include(CTest)

if(BUILD_TESTING)
    add_executable(scan_test tests/scan_test.cpp src/scan.cpp)
    target_include_directories(scan_test PRIVATE src)
    target_compile_features(scan_test PRIVATE cxx_std_17)

    add_test(NAME scan_correctness COMMAND scan_test)
endif()