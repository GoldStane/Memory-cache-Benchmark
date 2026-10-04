# Cache Memory Benchmark

A C++17 learning project exploring sequential access to contiguous memory. The
current benchmark sums a 1 MiB vector of unsigned 64-bit integers and measures
repeated scans after warm-up.

The goal is to learn how to build, validate, and interpret memory benchmarks.
Results are measurements of this scan on a particular machine and build, not a
direct measurement of RAM bandwidth or individual cache latency.

## Current behaviour

- Allocate 1 MiB of element storage and initialise values in the range 0–1000.
- Use a Mersenne Twister engine seeded with 55.
- Calculate a reference checksum during initialisation.
- Run and validate one warm-up scan, followed by 20 measured scans.
- Validate every trial against the reference; exit with status 1 on a mismatch.
- Report warm-up results separately from minimum, maximum, median, and mean scan
  durations, plus median nanoseconds per element.

The size and trial count are currently fixed in the driver. There are no
command-line options, CSV export, or plotting scripts yet.

## Requirements

- CMake 3.20 or newer.
- A C++17-capable compiler and C++ standard library.
- A build tool supported by CMake, such as Make or Ninja.

There are no third-party library dependencies.

## Build and run

From the repository root, using a single-configuration generator such as Unix
Makefiles or Ninja:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build
./build/main
ctest --test-dir build --output-on-failure
```

The first command configures the project and creates the build directory. The
second builds the executables; the third runs the scan benchmark. The final
command runs the correctness tests. Use Release builds for performance experiments. A Debug build is useful for debugging but is not
directly comparable to an optimised build.

For multi-configuration generators such as Xcode or Visual Studio, select
Release with `cmake --build build --config Release`; the executable is normally
inside the build directory's `Release` subdirectory (with `.exe` on Windows).

For a clean build check, configure a fresh directory instead of reusing a cache
from another machine or source location.

## Clock-overhead diagnostic

After building, run the separate diagnostic:

```sh
./build/clock_overhead
```

It measures 100,000 pairs of consecutive `steady_clock::now()` calls with no
scan between them, then reports minimum, maximum, and median empty-interval
durations in nanoseconds. Compare these with scan durations to assess timing
overhead. Zero readings can occur, and occasional large readings may reflect
interruptions. Do not automatically subtract this estimate from scan timings.
The diagnostic is not a CTest test and has no timing pass/fail threshold.

## Continuous integration

The GitHub Actions workflow builds Release executables and runs CTest on Ubuntu
with GCC and Clang, and on macOS with Clang. It runs on pushes and pull requests
targeting `main`. CI checks correctness; it does not enforce performance thresholds
or run the clock-overhead diagnostic.

## Source layout

- `CMakeLists.txt`: executable targets, source files, header path, and C++17 requirements.
- `src/scan.h`: declaration of the sequential checksum operation.
- `src/scan.cpp`: implementation of one sequential scan.
- `benchmarks/main.cpp`: input generation, warm-up, timing, validation, and reporting.
- `benchmarks/clock_overhead.cpp`: standalone empty timing-interval diagnostic.
- `.github/workflows/ci.yml`: Linux and macOS build and correctness checks.
- [docs/methodology.md](docs/methodology.md): measurement procedure and limitations.
- `tests/scan_test.cpp`: correctness checks for empty, single-element, and multi-element inputs.
- [LICENSE](LICENSE): MIT licence.

## Interpreting results

Use median nanoseconds per element as the main normalised metric. Minimum and
maximum durations provide context about variation within the run. Repeat whole
runs before drawing conclusions; no performance results are claimed here yet.

Record the CPU, operating system, compiler and standard-library versions, build
configuration, compiler flags, and source revision alongside any published
results. See the [methodology](docs/methodology.md) for timing boundaries,
optimisation caveats, and reproducibility details.

## Planned next steps

1. Compare repeated runs for stability and record timing-overhead observations.
2. Express the working-set size explicitly in bytes.
3. Reassess timing overhead and generated code when extending the experiment.
4. Sweep powers-of-two sizes from 4 KiB through 256 MiB.
5. Report useful read throughput alongside timing and variability.
6. Export validated measurements to CSV and plot results against working-set size.

## Licence

MIT. See [LICENSE](LICENSE).
