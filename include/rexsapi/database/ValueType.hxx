
#ifndef REXSCXX_DATABASE_VALUE_TYPES_HXX
#define REXSCXX_DATABASE_VALUE_TYPES_HXX

#include <rexsapi/Exception.hxx>
#include <rexsapi/Format.h>

namespace rexsapi::database
{
  enum class TValueType {
    FLOATING_POINT,
    BOOLEAN,
    INTEGER,
    ENUM,
    STRING,
    FILE_REFERENCE,
    BOOLEAN_ARRAY,
    FLOATING_POINT_ARRAY,
    REFERENCE_COMPONENT,
    FLOATING_POINT_MATRIX,
    INTEGER_ARRAY,
    ENUM_ARRAY,
    ARRAY_OF_INTEGER_ARRAYS
  };


  static inline TValueType typeFromString(const std::string& type)
  {
    if (type == "floating_point") {
      return TValueType::FLOATING_POINT;
    }
    if (type == "boolean") {
      return TValueType::BOOLEAN;
    }
    if (type == "integer") {
      return TValueType::INTEGER;
    }
    if (type == "enum") {
      return TValueType::ENUM;
    }
    if (type == "string") {
      return TValueType::STRING;
    }
    if (type == "file_reference") {
      return TValueType::FILE_REFERENCE;
    }
    if (type == "boolean_array") {
      return TValueType::BOOLEAN_ARRAY;
    }
    if (type == "floating_point_array") {
      return TValueType::FLOATING_POINT_ARRAY;
    }
    if (type == "reference_component") {
      return TValueType::REFERENCE_COMPONENT;
    }
    if (type == "floating_point_matrix") {
      return TValueType::FLOATING_POINT_MATRIX;
    }
    if (type == "integer_array") {
      return TValueType::INTEGER_ARRAY;
    }
    if (type == "enum_array") {
      return TValueType::ENUM_ARRAY;
    }
    if (type == "array_of_integer_arrays") {
      return TValueType::ARRAY_OF_INTEGER_ARRAYS;
    }

    throw Exception{fmt::format("unknown value type '{}'", type)};
  }
}

#endif
