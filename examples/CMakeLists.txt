
add_executable(example
  example.cxx
)
if(MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
  target_compile_options(example PRIVATE /bigobj)
endif()
target_link_libraries(example PRIVATE
  rexsapi
)

add_executable(builder_example
  BuilderExample.cxx
)
if(MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
  target_compile_options(builder_example PRIVATE /bigobj)
endif()
target_link_libraries(builder_example PRIVATE
  rexsapi
)

add_executable(model_checker
  ModelChecker.cxx
)
if(MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
  target_compile_options(model_checker PRIVATE /bigobj)
endif()
target_link_libraries(model_checker PRIVATE
  rexsapi
)

add_executable(model_converter
  ModelConverter.cxx
)
if(MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
  target_compile_options(model_converter PRIVATE /bigobj)
endif()
target_link_libraries(model_converter PRIVATE
  rexsapi
)
