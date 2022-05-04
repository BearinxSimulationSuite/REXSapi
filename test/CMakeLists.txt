configure_file(TestHelper.hxx.in ${CMAKE_CURRENT_BINARY_DIR}/TestHelper.hxx)

add_executable(rexsapi_test
  main.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/TestHelper.hxx
  TestModelLoader.hxx

  XSDSchemaValidatorTest.cxx
  ConversionHelperTest.cxx

  database/AttributeTest.cxx
  database/ComponentAttributeMapperTest.cxx
  database/ComponentTest.cxx
  database/EnumValuesTest.cxx
  database/FileResourceLoaderTest.cxx
  database/IntervalTest.cxx
  database/LoaderResultTest.cxx
  database/ModelRegistryTest.cxx
  database/ModelTest.cxx
  database/XMLModelLoaderTest.cxx
  database/ValueTypeTest.cxx
)

include(${PROJECT_SOURCE_DIR}/cmake/fetch_doctest.cmake)
include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")

target_include_directories(rexsapi_test SYSTEM PRIVATE "${doctest_SOURCE_DIR}/doctest")
target_include_directories(rexsapi_test PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(rexsapi_test PRIVATE
  Threads::Threads
  rexsapi
  ${CMAKE_DL_LIBS}
  ${REXSAPI_PLATFORM_LIBS}
)

if(COVERAGE)
  target_link_libraries(rexsapi_test PUBLIC --coverage)
  target_compile_options(rexsapi_test PUBLIC --coverage -g)
else()
  target_compile_options(rexsapi_test PRIVATE ${REXSAPI_COMPILE_OPTIONS})
endif()

doctest_discover_tests(rexsapi_test)
