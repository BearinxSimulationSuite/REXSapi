![Supported Platforms](https://img.shields.io/badge/platforms-Linux%20%7C%20Windows-blue.svg)
![License: Apache 2](https://img.shields.io/badge/license-Apache%202-blue)
![Language: C++17](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)
![Version:](https://img.shields.io/badge/version-pre--alpha-orange)
[![GitHub Build Status](https://github.com/BearinxSimulationSuite/REXSapi/workflows/CMake%20Build%20Matrix/badge.svg)](https://github.com/BearinxSimulationSuite/REXSapi/actions)

# REXSapi

C++ implementation of the [REXS specification](https://www.rexs.info/rexs_de.html).

# Status

The project is still in an early phase and the API may still change considerable in the near future. We wouldn't recommend the usage of the library at the current point in time. 

# Supported REXS Versions

The implementation currently supports versions 1.0 to 1.4. It supports model files in xml and json format.

# Supported Platforms

The library can be used on
- Linux
- Windows

# Build Instructions

The library is header only. A build is only necessary if you want to run the tests.

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
- [nlohmann json](https://github.com/nlohmann/json)
- [pugixml](https://github.com/zeux/pugixml)
- [valijson](https://github.com/tristanpenman/valijson)
- [doctest](https://github.com/doctest/doctest)
