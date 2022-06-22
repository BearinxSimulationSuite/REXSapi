
add_executable(example
  example.cxx
)
target_link_libraries(example PRIVATE
  rexsapi
)

add_executable(builder_example
  BuilderExample.cxx
)
target_link_libraries(builder_example PRIVATE
  rexsapi
)

add_executable(model_checker
  ModelChecker.cxx
)
target_link_libraries(model_checker PRIVATE
  rexsapi
)

add_executable(model_converter
  ModelConverter.cxx
)
target_link_libraries(model_converter PRIVATE
  rexsapi
)
