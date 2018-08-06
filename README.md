## Fork Changes
This is a fork of libmorton which includes [Morton ND](https://github.com/kevinhartman/morton-nd) as a Git submodule.

The purpose of this fork is to provide benchmarking for Morton ND using the libmorton validation and performance test library written by Jeroen Baert (@Forceflow). See [Morton ND's README.md](https://github.com/kevinhartman/morton-nd/blob/master/README.md#performance) for a sample set of performance metrics.

As such, its test code includes various Morton ND configurations for 32-bit and 64-bit 2D and 3D applications.

The hope is that this will demonstrate:
  - Morton ND's implementation is performance equivalent to a static LUT approach (hard-coded) such as the one found in libmorton.
  - Tuning LUT table size based on target architecture and access pattern (possible using Morton ND's compile-time LUT generation) can significantly improve performance, something infeasible for hard-coded LUT approaches.

Furthermore, this fork may provide an easy way for users to compare algorithms based on their target application and architecture. 

### Building
Note that C++14 is required to build, a requirement imposed by Morton ND. Also, ensure release build settings are used for accurate performance metrics, and that BMI2 is defined if your CPU supports it to see results for the BMI approach found in libmorton.

Building will take a long time (2-3 minutes on a decent laptop from ~2016). This is because many large LUTs are generated for these tests (some as large at 2^21 entries used to validate extreme cases).

### macOS
The original libmorton `LUT ET` and `LUT Shifted ET` tests do not seem to work on macOS. Comment out any lines in `libmorton_test.cpp::registerFunctions` with either of those names if you experience a runtime hang, or relevant build failures. See [this commit](https://github.com/kevinhartman/libmorton/commit/740595b7011dbad9f0bae5722293aca84eacc76c) for an example.

*Original README.md contents below*

# Libmorton
[![Build Status](https://travis-ci.org/Forceflow/libmorton.svg?branch=master)](https://travis-ci.org/Forceflow/libmorton) [![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://opensource.org/licenses/MIT)

 * Libmorton is a **C++ header-only library** with methods to efficiently encode/decode 64, 32 and 16-bit Morton codes and coordinates, in 2D and 3D. *Morton order* is also known as *Z-order* or *[the Z-order curve](https://en.wikipedia.org/wiki/Z-order_curve)*.
 * Libmorton is a **lightweight and portable** library - in its most basic form it only depends on standard C++ headers. Architecture-specific optimizations are implemented incrementally.
 * This library is under active development. SHIT WILL BREAK.
 * More info and some benchmarks in these blogposts: [*Morton encoding*](http://www.forceflow.be/2013/10/07/morton-encodingdecoding-through-bit-interleaving-implementations/), [*Libmorton*](http://www.forceflow.be/2016/01/18/libmorton-a-library-for-morton-order-encoding-decoding/) and [*BMI2 instruction set*](http://www.forceflow.be/2016/11/25/using-the-bmi2-instruction-set-to-encode-decode-morton-codes/)

## Usage
Just include `libmorton/morton.h`. This will always have functions that point to the most efficient way to encode/decode Morton codes. If you want to test out alternative (and possibly slower) methods, you can find them in `libmorton/morton2D.h` and `libmorton/morton3D.h`. **All libmorton functionality is in the `libmorton` namespace.**

<pre>
// ENCODING 2D / 3D morton codes, of length 32 and 64 bits
inline uint_fast32_t morton2D_32_encode(const uint_fast16_t x, const uint_fast16_t y);
inline uint_fast64_t morton2D_64_encode(const uint_fast32_t x, const uint_fast32_t y);
inline uint_fast32_t morton3D_32_encode(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z);
inline uint_fast64_t morton3D_64_encode(const uint_fast32_t x, const uint_fast32_t y, const uint_fast32_t z);
// DECODING 2D / 3D morton codes, of length 32 and 64 bits
inline void morton2D_32_decode(const uint_fast32_t morton, uint_fast16_t& x, uint_fast16_t& y);
inline void morton2D_64_decode(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y);
inline void morton3D_32_decode(const uint_fast32_t morton, uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& z);
inline void morton3D_64_decode(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z);
</pre>

If you want to take advantage of the BMI2 instruction set (only available on Intel Haswell processors and newer), make sure `__BMI2__` is defined before you include `morton.h`.

## Testing
The *test* folder contains tools I use to test correctness and performance of the libmorton implementation. This section is under heavy re-writing, but might contain some useful code for advanced usage.

## Citation
If you use libmorton in your paper or work, please reference it, for example as follows:
<pre>
@Misc{libmorton18,
author = "Jeroen Baert",
title = "Libmorton: C++ Morton Encoding/Decoding Library",
howpublished = "\url{https://github.com/Forceflow/libmorton}",
year = "2018"}
</pre>

## Thanks
 * To [@gnzlbg](https://github.com/gnzlbg) and his Rust implementation [bitwise](https://github.com/gnzlbg) for finding bugs in the Magicbits code 
 * Everyone making comments and suggestions on the [original blogpost](http://www.forceflow.be/2013/10/07/morton-encodingdecoding-through-bit-interleaving-implementations/)
 * Fabian Giesen's [post](https://fgiesen.wordpress.com/2009/12/13/decoding-morton-codes/) on Morton Codes

## TODO
 * Write better test suite (with L1/L2 trashing, better tests, ...)

 * A better naming system for the functions, because m3D_e_sLUT_shifted? That escalated quickly.
