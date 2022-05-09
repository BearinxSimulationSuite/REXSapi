
#include <rexsapi/Types.hxx>

#include <doctest.h>


TEST_CASE("Value type test")
{
  SUBCASE("Type from string")
  {
    CHECK(rexsapi::typeFromString("floating_point") == rexsapi::TValueType::FLOATING_POINT);
    CHECK(rexsapi::typeFromString("boolean") == rexsapi::TValueType::BOOLEAN);
    CHECK(rexsapi::typeFromString("integer") == rexsapi::TValueType::INTEGER);
    CHECK(rexsapi::typeFromString("enum") == rexsapi::TValueType::ENUM);
    CHECK(rexsapi::typeFromString("string") == rexsapi::TValueType::STRING);
    CHECK(rexsapi::typeFromString("file_reference") == rexsapi::TValueType::FILE_REFERENCE);
    CHECK(rexsapi::typeFromString("boolean_array") == rexsapi::TValueType::BOOLEAN_ARRAY);
    CHECK(rexsapi::typeFromString("floating_point_array") == rexsapi::TValueType::FLOATING_POINT_ARRAY);
    CHECK(rexsapi::typeFromString("reference_component") == rexsapi::TValueType::REFERENCE_COMPONENT);
    CHECK(rexsapi::typeFromString("floating_point_matrix") == rexsapi::TValueType::FLOATING_POINT_MATRIX);
    CHECK(rexsapi::typeFromString("integer_array") == rexsapi::TValueType::INTEGER_ARRAY);
    CHECK(rexsapi::typeFromString("enum_array") == rexsapi::TValueType::ENUM_ARRAY);
    CHECK(rexsapi::typeFromString("array_of_integer_arrays") == rexsapi::TValueType::ARRAY_OF_INTEGER_ARRAYS);
  }

  SUBCASE("Type from string")
  {
    CHECK_THROWS_WITH(rexsapi::typeFromString("not existing type"), "unknown value type 'not existing type'");
  }
}
