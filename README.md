![Supported Platforms](https://img.shields.io/badge/platforms-Linux%20%7C%20Windows-blue.svg)
![License: Apache 2](https://img.shields.io/badge/license-Apache%202-blue)
![Language: C++17](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)
![Version:](https://img.shields.io/badge/version-pre--alpha-orange)
[![GitHub Build Status](https://github.com/BearinxSimulationSuite/REXSapi/workflows/CMake%20Build%20Matrix/badge.svg)](https://github.com/BearinxSimulationSuite/REXSapi/actions)

# REXSapi

The REXSapi library is a C++ implementation of the [REXS specification](https://www.rexs.info/rexs_de.html).

# Status

The project is still in an early phase and the API may still change considerable in the near future. We wouldn't recommend the usage of the library at the current point in time. 

# Supported REXS Versions

The implementation currently supports versions 1.0 to 1.4. It supports model files in xml and json format.

# Supported Platforms

The library can be used on
- Linux
- Windows

You need a C++17 compatible compiler to use the library.

# Integration

The library is header only and can be easily integrated into existing projects. Using CMake is the recommended way to use the library. However, the library also comes as a zip package which can be used without CMake. You have to set the C++ standard of your project to C++17 in order to build with the library. The library has dependencies to other open source software. This dependencies will be either automatically downloaded by CMake or are prepackaged in the zip package.

## CMake

Just clone the git repository and add REXSapi as a sub directory in an appropriate CMakeLists.txt file. Then use the provided rexsapi interface as library. If you want to build with the examples or the tests, you can set `BUILD_WITH_EXAMPLES` and/or `BUILD_WITH_TESTS` to `ON`.

```
  set(CMAKE_CXX_STANDARD 17)
  add_executable(test
    main.cxx
  )
  set(BUILD_WITH_EXAMPLES ON)
  add_subdirectory(REXSapi)
  target_link_libraries(test PRIVATE rexsapi)
```

Alternatively, you can use CMakes FetchContent module to download REXSapi and make the projects interface available.

```
include(FetchContent)
FetchContent_Declare(
  rexsapi
  GIT_REPOSITORY https://github.com/BearinxSimulationSuite/REXSapi.git
  GIT_TAG origin/main
)

FetchContent_MakeAvailable(rexsapi)
```

## Package

If you do not want to use CMake, you can download a REXSapi zip package. The package contains all necessary header files, including all dependencies. In order to build a REXSapi project, unzip the archive and add the resulting directories `include` and `deps/include` directory as additional header search directories to your build.

# Build Instructions

The library is header only. A build is only necessary if you want to run the tests or build the examples.

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

- [fmt 8.1.1](https://github.com/fmtlib/fmt)
- [nlohmann json 3.10.5](https://github.com/nlohmann/json)
- [pugixml 1.12.1](https://github.com/zeux/pugixml)
- [valijson 0.6](https://github.com/tristanpenman/valijson)
- [doctest 2.4.8](https://github.com/doctest/doctest)
