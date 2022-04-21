configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/Version.hxx.in"
  "${PROJECT_BINARY_DIR}/rexscxx/Version.hxx"
)

add_library(rexscxx INTERFACE)

target_sources(rexscxx INTERFACE
  ${PROJECT_SOURCE_DIR}/include/rexscxx/database/Attribute.hxx
  ${PROJECT_SOURCE_DIR}/include/rexscxx/database/Component.hxx
  ${PROJECT_SOURCE_DIR}/include/rexscxx/database/Interval.hxx
  ${PROJECT_SOURCE_DIR}/include/rexscxx/database/Model.hxx
  ${PROJECT_SOURCE_DIR}/include/rexscxx/database/ModelLoader.hxx
  ${PROJECT_SOURCE_DIR}/include/rexscxx/database/ModelRegistry.hxx
  ${PROJECT_SOURCE_DIR}/include/rexscxx/database/Unit.hxx
  ${PROJECT_SOURCE_DIR}/include/rexscxx/database/ValueType.hxx
  ${PROJECT_SOURCE_DIR}/include/rexscxx/database/XMLModelLoader.hxx

  ${PROJECT_SOURCE_DIR}/include/rexscxx/ConversionHelper.hxx
  ${PROJECT_SOURCE_DIR}/include/rexscxx/Exception.hxx
  ${PROJECT_SOURCE_DIR}/include/rexscxx/FileResourceLoader.hxx
  ${PROJECT_SOURCE_DIR}/include/rexscxx/ResourceLoader.hxx
  ${PROJECT_SOURCE_DIR}/include/rexscxx/XMLParser.hxx
)

target_include_directories(rexscxx INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>)
target_include_directories(rexscxx SYSTEM INTERFACE "${doctest_SOURCE_DIR}/doctest")
target_include_directories(rexscxx SYSTEM INTERFACE "${pugixml_SOURCE_DIR}/src")
target_include_directories(rexscxx INTERFACE ${PROJECT_BINARY_DIR})
