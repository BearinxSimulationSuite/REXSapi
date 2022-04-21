configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/Version.hxx.in"
  "${PROJECT_BINARY_DIR}/rexsapi/Version.hxx"
)

add_library(rexsapi INTERFACE)

target_sources(rexsapi INTERFACE
  ${PROJECT_SOURCE_DIR}/include/rexsapi/database/Attribute.hxx
  ${PROJECT_SOURCE_DIR}/include/rexsapi/database/Component.hxx
  ${PROJECT_SOURCE_DIR}/include/rexsapi/database/FileResourceLoader.hxx
  ${PROJECT_SOURCE_DIR}/include/rexsapi/database/Interval.hxx
  ${PROJECT_SOURCE_DIR}/include/rexsapi/database/Model.hxx
  ${PROJECT_SOURCE_DIR}/include/rexsapi/database/ModelLoader.hxx
  ${PROJECT_SOURCE_DIR}/include/rexsapi/database/ModelRegistry.hxx
  ${PROJECT_SOURCE_DIR}/include/rexsapi/database/ResourceLoader.hxx
  ${PROJECT_SOURCE_DIR}/include/rexsapi/database/Unit.hxx
  ${PROJECT_SOURCE_DIR}/include/rexsapi/database/ValueType.hxx
  ${PROJECT_SOURCE_DIR}/include/rexsapi/database/XMLModelLoader.hxx

  ${PROJECT_SOURCE_DIR}/include/rexsapi/ConversionHelper.hxx
  ${PROJECT_SOURCE_DIR}/include/rexsapi/Exception.hxx
  ${PROJECT_SOURCE_DIR}/include/rexsapi/XMLParser.hxx
)

target_include_directories(rexsapi INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>)
target_include_directories(rexsapi SYSTEM INTERFACE "${pugixml_SOURCE_DIR}/src")
target_include_directories(rexsapi INTERFACE ${PROJECT_BINARY_DIR})
target_compile_options(rexsapi INTERFACE ${REXSAPI_COMPILE_OPTIONS})
