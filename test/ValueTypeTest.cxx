
#include <rexsapi/database/ValueType.hxx>

#include <doctest.h>


TEST_CASE("Value type test")
{
  SUBCASE("Type from string")
  {
    CHECK(rexsapi::database::typeFromString("floating_point") == rexsapi::database::TValueType::FLOATING_POINT);
    CHECK(rexsapi::database::typeFromString("boolean") == rexsapi::database::TValueType::BOOLEAN);
    CHECK(rexsapi::database::typeFromString("integer") == rexsapi::database::TValueType::INTEGER);
    CHECK(rexsapi::database::typeFromString("enum") == rexsapi::database::TValueType::ENUM);
    CHECK(rexsapi::database::typeFromString("string") == rexsapi::database::TValueType::STRING);
    CHECK(rexsapi::database::typeFromString("file_reference") == rexsapi::database::TValueType::FILE_REFERENCE);
    CHECK(rexsapi::database::typeFromString("boolean_array") == rexsapi::database::TValueType::BOOLEAN_ARRAY);
    CHECK(rexsapi::database::typeFromString("floating_point_array") == rexsapi::database::TValueType::FLOATING_POINT_ARRAY);
    CHECK(rexsapi::database::typeFromString("reference_component") == rexsapi::database::TValueType::REFERENCE_COMPONENT);
    CHECK(rexsapi::database::typeFromString("floating_point_matrix") == rexsapi::database::TValueType::FLOATING_POINT_MATRIX);
    CHECK(rexsapi::database::typeFromString("integer_array") == rexsapi::database::TValueType::INTEGER_ARRAY);
    CHECK(rexsapi::database::typeFromString("enum_array") == rexsapi::database::TValueType::ENUM_ARRAY);
    CHECK(rexsapi::database::typeFromString("array_of_integer_arrays") == rexsapi::database::TValueType::ARRAY_OF_INTEGER_ARRAYS);
  }

  SUBCASE("Type from string")
  {
    CHECK_THROWS_WITH(rexsapi::database::typeFromString("not existing type"), "unknown value type 'not existing type'");
  }
}
