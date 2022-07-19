![Supported Platforms](https://img.shields.io/badge/platforms-Linux%20%7C%20Windows%20%7C%20Mac-blue.svg)
![License: Apache 2](https://img.shields.io/badge/license-Apache%202-blue)
![Language: C++17](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)
![Version:](https://img.shields.io/badge/version-alpha-orange)
[![GitHub Build Status](https://github.com/BearinxSimulationSuite/REXSapi/workflows/CMake%20Build%20Matrix/badge.svg)](https://github.com/BearinxSimulationSuite/REXSapi/actions)

# REXSapi

The REXSapi library is a C++ implementation of the [REXS specification](https://www.rexs.info/rexs_de.html).

# Status

The project is still in an early phase and the API may still change in the near future. We wouldn't recommend the usage of the library at the current point in time. 

# Supported REXS Versions

The library uses REXS database model files in order to validate REXS model files. Database model files can be downloaded from the [REXS database page](https://database.rexs.info/). Currently, the implementation supports versions 1.0 to 1.4, but newer database files should also work. Version 1.0 to 1.4 database model files in english and german can also be found in the models directory of this project.

The library supports REXS model files in xml and json format. Compressed REXS zip archives can also be loaded.

# Supported Platforms

The library can be used on
- Linux
- Windows
- Mac OS X

You need a C++17 compatible compiler to use the library.

# Getting Started

In order to use REXSapi it is most convinient to just include the `Rexsapi.hxx` header. Mind that you have to include this header along the `REXSAPI_MINIZ_IMPL` define right before the include in *exactly* one compilation unit (cpp file) in order to add the miniz implementation to the project.

```c++
#define REXSAPI_MINIZ_IMPL
#include <rexsapi/Rexsapi.hxx>
```

## Load a REXS Model File

Loading a REXS model file is straight forward. You need the REXS database model files for the API to validate the model.

```c++
#define REXSAPI_MINIZ_IMPL
#include <rexsapi/Rexsapi.hxx>


const rexsapi::TModelLoader loader{"/path/to/rexs/database/models"};
rexsapi::TResult result;
const std::optional<rexsapi::TModel> model = 
        loader.load("/path/to/your/rexs/model/file", result, rexsapi::TMode::STRICT_MODE);
if (result) {
  std::cout << "sucessfully loaded REXS model\n";
}
```

The `TModelLoader` class can load json and xml REXS model files. If successful, the result will convert to true and the model optional will contain a model. In case of a failure, the result will contain a collection of messages, describing the issues. The issues can either be errors or warnings. It is perfectly possible, that the result converts to false, a failure, but the model optional contains a model. This means that the model could be loaded in general, but that there are issues with the model like incorrect value types, missing references, etc.

# Tools

The library comes packaged with two tools: `model_converter` and `model_checker`. The tools can come handy with working with rexs model files and can also serve as examples how to use the library.

## model_checker

The `model_checker` checks files for compatibility with the REXSapi library. You can check complete directories with one go. The library expects REXS model files to be conformant to the REXS specification. However, the library supports a so called _relaxed_ mode where most errors to the specfication are turned into warnings in order to process files even if they are not 100% compliant to the specifcation. The tool will print the found issues to the console. You can use the tools output to fix problems in the files.

### Options

| Option | Description |
|:--|:--|
| help | Show usage and options |
| mode-strict | This is the default mode. Files will be checked to comply strictly to the standard. |
| mode-relaxed | This mode will relax the checking and produce warnings instead of errors for non-standard constructs. |
| warnings | This will enables the printing of warnings to the console. Otherwise, only errors will be printed. |
| database | The path to the model database files including the schemas (json and xml). |
| models | The path to a directory to look for model files to check. |

Additionally, you can specify single files to process.

```bash
> ./model_checker --mode-relaxed -d ../models FVA-Industriegetriebe_2stufig_1-4.rexs
File ".FVA-Industriegetriebe_2stufig_1-4.rexs" processed with warnings
```

## model_converter

The `model_converter` can convert REXS model files between xml and json format. Files can be converted in any direction, even into the same format. You can convert complete directories with one go. As with the `model_checker`, the tool supports a relaxed mode for loading non-standard complying model files.

### Options
| Option | Description |
|:--|:--|
| help | Show usage and options |
| mode-strict | This is the default mode. Files will be checked to comply strictly to the standard. |
| mode-relaxed | This mode will relax the checking and produce warnings instead of errors for non-standard constructs. |
| format | The output format of the tool. Either json or xml. |
| output | The output path to write converted file to. |
| database | The path to the model database files including the schemas (json and xml). |
| models | The path to a directory to look for model files to check. |

Additionally, you can specify single files to process.

```bash
> ./model_converter --mode-relaxed -f json -d ../models --output /out FVA-Industriegetriebe_2stufig_1-4.rexs
Converted FVA-Industriegetriebe_2stufig_1-4.rexs to /out/FVA-Industriegetriebe_2stufig_1-4.rexsj
```

# Integration

The library is header only and can be easily integrated into existing projects. Using CMake is the recommended way to use the library. However, the library also comes as a zip package which can be used without CMake. You have to set the C++ standard of your project to C++17 in order to build with the library. 

The library has dependencies to other open source software. This dependencies will be either automatically downloaded by CMake or are prepackaged in the zip package. You can also use your own versions of the thirdparty libraries, but make sure the version are compatible to the versions used by the REXSapi.

## CMake

Just clone the git repository and add REXSapi as a sub directory in an appropriate CMakeLists.txt file. Then use the provided rexsapi interface as library. If you want to build with the examples, tools or the tests, you can set `BUILD_WITH_EXAMPLES`, `BUILD_WITH_TESTS`, and/or `BUILD_WITH_TOOLS` to `ON`.

```cmake
set(CMAKE_CXX_STANDARD 17)
add_executable(test
  main.cxx
)
set(BUILD_WITH_EXAMPLES ON)
add_subdirectory(REXSapi)
target_link_libraries(test PRIVATE rexsapi)
```

Alternatively, you can use CMakes FetchContent module to download REXSapi and make the projects interface available.

```cmake
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

## Mac
- You will need the following software packages
  - XCode 12.4
  - cmake 3.16.3 or higher
- To install cmake on Mac
  - Call `brew install cmake`
- Create a build directory in the source directory of REXSapi and change to it
- Call `cmake -GXcode -DCMAKE_BUILD_TYPE=Release ..`
- Call `cmake --build . --config Release`
- To run the tests
  - Call `ctest -VV -C Release`

## External Dependencies

REXSapi uses the following thirdparty open source software

- [cli11 2.2.0](https://github.com/CLIUtils/CLI11)
- [fmt 8.1.1](https://github.com/fmtlib/fmt)
- [nlohmann json 3.10.5](https://github.com/nlohmann/json)
- [miniz 2.2.0](https://github.com/richgel999/miniz)
- [pugixml 1.12.1](https://github.com/zeux/pugixml)
- [valijson 0.6](https://github.com/tristanpenman/valijson)
- [doctest 2.4.8](https://github.com/doctest/doctest)

# License
REXsapi is licensed under the Apache-2.0 license.

See the LICENSE.txt file for more information.
