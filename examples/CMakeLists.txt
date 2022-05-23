
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
