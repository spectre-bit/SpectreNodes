A custom seed node for ComfyUI written as a C++ Python extension using Python.h. This node utilizes the modern state-of-the-art PRNG Xoshiro256**, developed by David Blackman and Sebastiano Vigna, to ensure high-quality randomness that passes even the most stringent statistical tests like BigCrush. The original xoshiro / xoroshiro source: https://prng.di.unimi.it/. I used the C++ implemenation of Nessan Fitzmaurice: https://github.com/nessan/xoshiro. The node outputs a full 64 bit unsigned integer other than most nodes whichs output is limited by javascripts Number.MAX_SAFE_INTEGER which is only $2^{53} - 1$. This node uses a string as the datatype for the input fied to bypass javascripts limitations (the string has to contain a valid uint64_t), therefore the outputted seed utilises the full 0 - $2^{64} - 1$ range of uint64_t.

# Usage

clone this repository in your ComfyUI\custom_nodes folder and then build the C++ python extension module. 

# Build

## requirements

- A C++ Compiler which supports C++23 (recommendation: clang++)
- CMake
- A build tool (recommendation: Ninja)
- Python installation with development headers and libraries (which is the standard)

in the cmake command you need to do `-DPYTHON_VERSION="3.<xx>"` replace \<xx\> with the minor release verion of the python version you are using in your ComfyUI venv otherwise the node will not work.

## tutorial

for LLVM install with MSVC toolchain and Ninja:

```
cmake -G Ninja -B build -DPYTHON_VERSION="3.<xx>" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_FLAGS="-O3 -march=native -flto -DNDEBUG" -DCMAKE_MODULE_LINKER_FLAGS="-fuse-ld=lld-link"
```

note that you replace 3.\<xx\> with your actual python version e.g. `-DPYTHON_VERSION="3.13"`. The build will fail if you do not set the python version.

```
ninja -C build
```
