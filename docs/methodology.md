# Measurement methodology

## Scope

The current experiment measures the elapsed time of a sequential checksum scan
over a fixed 1 MiB vector of `std::uint64_t` values. On the current macOS target,
this is 1,048,576 bytes and 131,072 elements. One scan visits every element in
order and returns an unsigned 64-bit sum.

Random values do not imply random access: the access pattern is sequential.
Initialisation and warm-up touch the data before the measured trials. This is a
repeated-access experiment, not a guaranteed cold-cache measurement.

## Current procedure

1. Reserve storage for 20 durations and 20 checksums.
2. Allocate the input vector.
3. Initialise the input with `std::mt19937_64`, seed 55, and a uniform integer
   distribution over the inclusive range 0–1000. Accumulate a reference checksum
   during initialisation, independently of the scan function.
4. Time one warm-up scan and validate its checksum. A mismatch ends the program
   with status 1 before measured trials begin.
5. Run 20 trials. In each trial, read `std::chrono::steady_clock` immediately
   before and after the scan function call. Store the duration and returned
   checksum after the finish timestamp.
6. Validate all measured checksums. A mismatch invalidates the entire run: report
   the failing trial and expected/actual values, return status 1, and omit the
   performance summary. Do not silently discard a failed trial.
7. Sort durations, calculate summary statistics, and print the warm-up and trial
   results in separate sections. Successful execution returns status 0.

The fixed engine seed supports repeatability within the same implementation.
The distribution's mapping is not guaranteed identical across different C++
standard-library implementations; record the toolchain for cross-machine work.

## Timing boundaries

The timed region contains the scan function call, including call/return overhead
that remains after optimisation. Clock-reading overhead also affects the observed
interval. Allocation, random generation, reference-checksum generation,
result-vector insertion, validation, sorting, and printing are outside it.

Durations are converted to integer nanoseconds. That unit does not imply that the
clock has one-nanosecond resolution or accuracy. Very short scans can be dominated
by measurement overhead. A later size sweep must assess whether several scans
per measurement are needed, and must count all processed elements when
normalising such measurements.

## Statistics and units

- Minimum and maximum: smallest and largest measured trial durations.
- Median: the middle sorted duration, or the average of the two middle durations
  for an even number of trials.
- Mean: sum of trial durations divided by the number of trials using
  floating-point division.
- Median ns/element: median scan duration in nanoseconds divided by the number
  of elements scanned once.

The warm-up is excluded from these trial statistics. Durations are currently
sorted in place, so their chronological order is not retained. Preserve raw
trial order before sorting if later exporting per-trial data or diagnosing drift.

KiB means 1,024 bytes and MiB means 1,048,576 bytes. A planned useful-read-throughput
metric is total bytes processed / elapsed seconds / 1,000,000,000, expressed in
decimal GB/s. It is not implemented yet. Useful bytes count element data consumed
by the scan, not measured hardware memory traffic.

## Correctness and optimisation

Every recorded checksum is checked against the independently accumulated
reference. This is necessary for correctness, but does not by itself guarantee
that every source-level scan survives compiler optimisation as intended.
Storing or printing results, and placing the scan in another source file, are
not universal optimisation barriers, especially with link-time optimisation.

Before making performance claims, inspect the optimised program or adopt an
appropriate benchmark harness and understand its optimisation controls. Record
whether link-time optimisation is enabled. Do not disable optimisation simply
to obtain plausible-looking timings: that changes the operation being measured.

## Reproducibility record

For each retained experiment, record:

- Source revision, including whether there are uncommitted changes.
- CPU model, operating system/version, and installed memory.
- Compiler and C++ standard-library versions.
- CMake version, generator, build configuration, effective compiler flags, and
  link-time optimisation settings.
- Input size in bytes, element count/type, seed, value range, warm-up count,
  measured trial count, and scans per trial.
- Power mode, whether the machine is on battery, and notable background load.
- Validation status and several independent runs with their summary statistics.

The program does not collect this metadata automatically. Use an optimised
Release build and keep experimental conditions reasonably consistent. A seed
alone is not a complete reproducibility record.

## Interpretation limits

Elapsed scan time includes effects of generated instructions, vectorisation,
cache behaviour, prefetching, CPU frequency, scheduling, and other system activity.
Repeated scans may be served by caches and do not directly measure RAM bandwidth.
One 1 MiB input cannot establish cache boundaries or describe the whole memory
hierarchy. Even a future curve over many sizes will require care before attributing
changes to individual cache levels.

The current benchmark is a learning baseline. CTest covers empty, single-element,
and multi-element scan inputs. A size sweep, CSV export, and plots remain planned work.
