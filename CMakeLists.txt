cmake_minimum_required(VERSION 3.16)
project(rexsapi VERSION 0.1.28 LANGUAGES CXX)

if(POLICY CMP0110)
  cmake_policy(SET CMP0110 OLD)
endif()

include(FetchContent)

if(NOT DEFINED REXSAPI_MASTER_PROJECT)
  set(REXSAPI_MASTER_PROJECT OFF)

  if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(REXSAPI_MASTER_PROJECT ON)
    message(STATUS "Building REXSapi as master project")
  endif()
endif()

option(BUILD_WITH_EXAMPLES "Build with examples" ${REXSAPI_MASTER_PROJECT})
option(BUILD_WIHT_TESTS "Build with tests" ${REXSAPI_MASTER_PROJECT})
option(BUILD_WITH_TOOLS "Build with tools" ON)

if(NOT CMAKE_BUILD_TYPE AND REXSAPI_MASTER_PROJECT)
  set(CMAKE_BUILD_TYPE Release)
endif()

include(cmake/fetch_fmt.cmake)
include(cmake/fetch_json.cmake)
include(cmake/fetch_pugixml.cmake)
include(cmake/fetch_valijson.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT APPLE AND UNIX)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(REXSAPI_COMPILE_OPTIONS -Wall -Wheader-hygiene -Wcast-align -Wconversion -Wfloat-equal -Wformat=2 -Wmissing-declarations -Woverlength-strings -Wshadow -Wunreachable-code -Wextra -Wpedantic -Wnon-virtual-dtor -Werror -Wno-nested-anon-types -Wdelete-non-virtual-dtor -Wdeprecated -Wfloat-conversion -Wfloat-overflow-conversion -Wmove -Wredundant-move -Wshadow-all -Wsign-compare -Wsign-conversion -Wunreachable-code -Wunused-value -Wignored-qualifiers -Wno-unused-private-field)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(REXSAPI_COMPILE_OPTIONS -Wall -Wcast-align -Wconversion -Wfloat-equal -Wformat=2 -Wmissing-declarations -Woverlength-strings -Wshadow -Wunreachable-code -Wextra -Wnon-virtual-dtor -Werror -Wno-psabi -Wignored-qualifiers -Wpedantic)
  else()
    message(FATAL_ERROR "unsupported compiler")
  endif()
elseif(WIN32)
  set(REXSAPI_COMPILE_OPTIONS)
elseif(APPLE)
  set(REXSAPI_COMPILE_OPTIONS -g -Wall -Wheader-hygiene -Wcast-align -Wconversion -Wfloat-equal -Wformat=2 -Wmissing-declarations -Woverlength-strings -Wshadow -Wunreachable-code -Wextra -Wpedantic -Wnon-virtual-dtor -Werror -Wno-nested-anon-types -Wdelete-non-virtual-dtor -Wdeprecated -Wfloat-conversion -Wfloat-overflow-conversion -Wmove -Wredundant-move -Wshadow-all -Wsign-compare -Wsign-conversion -Wunreachable-code -Wunused-value -Wignored-qualifiers -Wno-unused-private-field)
else()
  message(FATAL_ERROR "unsupported platform")
endif()

add_subdirectory(src)

if(BUILD_WITH_EXAMPLES)
  message(STATUS "Building with examples")
  add_subdirectory(examples)
endif()

if(BUILD_WIHT_TESTS)
  enable_testing()
  message(STATUS "Building with tests")
  add_subdirectory(test)
endif()

if(BUILD_WITH_TOOLS)
  message(STATUS "Building with tools")
  add_subdirectory(tools)
endif(
)