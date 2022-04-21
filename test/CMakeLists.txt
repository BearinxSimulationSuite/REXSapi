configure_file(TestHelper.hxx.in ${CMAKE_CURRENT_BINARY_DIR}/TestHelper.hxx)

add_executable(rexscxx_test
  main.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/TestHelper.hxx

  FileResourceLoaderTest.cxx
  ModelRegistryTest.cxx
  XMLModelLoaderTest.cxx
)

include(${PROJECT_SOURCE_DIR}/cmake/fetch_doctest.cmake)

target_link_libraries(rexscxx_test PRIVATE
  Threads::Threads
  rexscxx
  ${CMAKE_DL_LIBS}
)

include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")

message(STATUS "include ${PROJECT_SOURCE_DIR}/include")

if(COVERAGE)
  target_link_libraries(rexscxx_test PUBLIC --coverage)
  target_compile_options(rexscxx_test PUBLIC --coverage -g)
else()
  target_compile_options(rexscxx_test PRIVATE ${REXSCXX_PLATFORM_LIBS})
endif()

doctest_discover_tests(rexscxx_test)
