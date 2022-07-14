
add_executable(model_checker
  ModelChecker.cxx
)

if(MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
  target_compile_options(model_checker PRIVATE /bigobj)
endif()

target_link_libraries(model_checker PRIVATE
  rexsapi CLI11::CLI11
)

add_executable(model_converter
  ModelConverter.cxx
)

if(MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
  target_compile_options(model_converter PRIVATE /bigobj)
endif()

target_link_libraries(model_converter PRIVATE
  rexsapi CLI11::CLI11
)
