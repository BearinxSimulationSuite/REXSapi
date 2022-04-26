![Supported Platforms](https://img.shields.io/badge/Linux%20%7C%20Windows-blue.svg)
![Language: C++17](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)
[![GitHub Build Status](https://github.com/BearinxSimulationSuite/REXSapi/workflows/CMake%20Build%20Matrix/badge.svg)](https://github.com/BearinxSimulationSuite/REXSapi/actions)

# REXSapi

C++ implementation of the REXS spezification.

# Supported REXS Versions

The implementation currently supports versions 1.0 to 1.4.

# Supported Platforms

The library can be used on
- Linux
- Windows

# Build Instructions

## Linux

- You will need the following software packages
  - g++ 9.3.0 or higher
  - cmake 3.16.3 or higher
- To install the dependencies on ubuntu
  - Call `sudo apt-get install cmake g++`
- Create a build directory in the source directory of REXSapi and change to it
- Call `cmake -DCMAKE_BUILD_TYPE=Release ..`
- Call `cmake --build . --config Release`
- To run the tests
  - Call `ctest -VV -C Release`

## Windows

- You will need the following software packages
  - Visual Studio 2019 or higher
- CMake support is build into Visual Studio 2019
- Open the local folder of the REXSapi project
- Visual Studio will configure the project automatically

## External Dependencies

RESTapi uses the following thirdparty open source software

- [fmt](https://github.com/fmtlib/fmt)
- [pugixml](https://github.com/zeux/pugixml)
- [doctest](https://github.com/doctest/doctest)
