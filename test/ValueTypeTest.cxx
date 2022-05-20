/*
 * Copyright Schaeffler Technologies AG & Co. KG (info.de@schaeffler.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <rexsapi/Types.hxx>
#include <rexsapi/Value.hxx>

#include <doctest.h>
#include <sstream>

namespace
{
  std::string dispatch(rexsapi::TValueType type, const rexsapi::TValue& value)
  {
    return rexsapi::dispatch<std::string>(type, value,
                                          {[](rexsapi::FloatTag, const auto& d) -> std::string {
                                             std::stringstream stream;
                                             stream << "float " << d;
                                             return stream.str();
                                           },
                                           [](rexsapi::BoolTag, const auto& b) -> std::string {
                                             std::stringstream stream;
                                             stream << "bool " << b;
                                             return stream.str();
                                           },
                                           [](rexsapi::IntTag, const auto& i) -> std::string {
                                             std::stringstream stream;
                                             stream << "int " << i;
                                             return stream.str();
                                           },
                                           [](rexsapi::EnumTag, const auto& s) -> std::string {
                                             std::stringstream stream;
                                             stream << "enum " << s;
                                             return stream.str();
                                           },
                                           [](rexsapi::StringTag, const auto& s) -> std::string {
                                             std::stringstream stream;
                                             stream << "string " << s;
                                             return stream.str();
                                           },
                                           [](rexsapi::FileReferenceTag, const auto& s) -> std::string {
                                             std::stringstream stream;
                                             stream << "file reference " << s;
                                             return stream.str();
                                           },
                                           [](rexsapi::FloatArrayTag, const auto& a) -> std::string {
                                             return "float array " + std::to_string(a.size()) + " entries";
                                           },
                                           [](rexsapi::BoolArrayTag, const auto& a) -> std::string {
                                             return "bool array " + std::to_string(a.size()) + " entries";
                                           },
                                           [](rexsapi::IntArrayTag, const auto& a) -> std::string {
                                             return "int array " + std::to_string(a.size()) + " entries";
                                           },
                                           [](rexsapi::EnumArrayTag, const auto& a) -> std::string {
                                             return "enum array " + std::to_string(a.size()) + " entries";
                                           },
                                           [](rexsapi::ReferenceComponentTag, const auto& n) -> std::string {
                                             std::stringstream stream;
                                             stream << "reference component " << n;
                                             return stream.str();
                                           },
                                           [](rexsapi::FloatMatrixTag, const auto& m) -> std::string {
                                             return "float matrix " + std::to_string(m.m_Values.size()) + " entries";
                                           },
                                           [](rexsapi::ArrayOfIntArraysTag, const auto& a) -> std::string {
                                             return "array of int arrays " + std::to_string(a.size()) + " entries";
                                           }});
  }
  std::string dispatch_empty(rexsapi::TValueType type, const rexsapi::TValue& value)
  {
    return rexsapi::dispatch<std::string>(type, value, {});
  }
}
namespace rexsapi
{
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

    SUBCASE("Type mapping")
    {
      CHECK(::dispatch(rexsapi::TValueType::FLOATING_POINT, rexsapi::TValue{47.11}) == "float 47.11");
      CHECK(::dispatch(rexsapi::TValueType::BOOLEAN, rexsapi::TValue{true}) == "bool 1");
      CHECK(::dispatch(rexsapi::TValueType::INTEGER, rexsapi::TValue{4711}) == "int 4711");
      CHECK(::dispatch(rexsapi::TValueType::STRING, rexsapi::TValue{"puschel"}) == "string puschel");
      CHECK(
        ::dispatch(rexsapi::TValueType::ENUM, rexsapi::TValue{"heat_treatable_steel_quenched_tempered_condition"}) ==
        "enum heat_treatable_steel_quenched_tempered_condition");
      CHECK(::dispatch(rexsapi::TValueType::FILE_REFERENCE, rexsapi::TValue{"./gear.gde"}) ==
            "file reference ./gear.gde");
      CHECK(::dispatch(rexsapi::TValueType::REFERENCE_COMPONENT, rexsapi::TValue{15}) == "reference component 15");
      CHECK(::dispatch(rexsapi::TValueType::FLOATING_POINT_ARRAY,
                       rexsapi::TValue{rexsapi::TFloatArrayType{1.1, 2.1, 3.1, 4.1}}) == "float array 4 entries");
      CHECK(::dispatch(rexsapi::TValueType::BOOLEAN_ARRAY,
                       rexsapi::TValue{rexsapi::TBoolArrayType{true, true, false, true}}) == "bool array 4 entries");
      CHECK(::dispatch(rexsapi::TValueType::INTEGER_ARRAY, rexsapi::TValue{rexsapi::TIntArrayType{1, 2, 3, 4, 5}}) ==
            "int array 5 entries");
      CHECK(::dispatch(rexsapi::TValueType::ENUM_ARRAY, rexsapi::TValue{rexsapi::TEnumArrayType{"1", "2", "3", "4"}}) ==
            "enum array 4 entries");
      CHECK(::dispatch(rexsapi::TValueType::FLOATING_POINT_MATRIX,
                       rexsapi::TValue{rexsapi::TFloatMatrixType{
                         {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0}}}}) == "float matrix 3 entries");
      CHECK(::dispatch(rexsapi::TValueType::ARRAY_OF_INTEGER_ARRAYS,
                       rexsapi::TValue{rexsapi::TArrayOfIntArraysType{{1, 2, 3}, {4, 5}, {6}}}) ==
            "array of int arrays 3 entries");
    }

    SUBCASE("With empty dispatcher funcs")
    {
      CHECK_THROWS(::dispatch_empty(rexsapi::TValueType::FLOATING_POINT, rexsapi::TValue{}));
      CHECK_THROWS(::dispatch_empty(rexsapi::TValueType::BOOLEAN, rexsapi::TValue{}));
      CHECK_THROWS(::dispatch_empty(rexsapi::TValueType::INTEGER, rexsapi::TValue{}));
      CHECK_THROWS(::dispatch_empty(rexsapi::TValueType::ENUM, rexsapi::TValue{}));
      CHECK_THROWS(::dispatch_empty(rexsapi::TValueType::STRING, rexsapi::TValue{}));
      CHECK_THROWS(::dispatch_empty(rexsapi::TValueType::FILE_REFERENCE, rexsapi::TValue{}));
      CHECK_THROWS(::dispatch_empty(rexsapi::TValueType::FLOATING_POINT_ARRAY, rexsapi::TValue{}));
      CHECK_THROWS(::dispatch_empty(rexsapi::TValueType::BOOLEAN_ARRAY, rexsapi::TValue{}));
      CHECK_THROWS(::dispatch_empty(rexsapi::TValueType::INTEGER_ARRAY, rexsapi::TValue{}));
      CHECK_THROWS(::dispatch_empty(rexsapi::TValueType::ENUM_ARRAY, rexsapi::TValue{}));
      CHECK_THROWS(::dispatch_empty(rexsapi::TValueType::REFERENCE_COMPONENT, rexsapi::TValue{}));
      CHECK_THROWS(::dispatch_empty(rexsapi::TValueType::FLOATING_POINT_MATRIX, rexsapi::TValue{}));
      CHECK_THROWS(::dispatch_empty(rexsapi::TValueType::ARRAY_OF_INTEGER_ARRAYS, rexsapi::TValue{}));
    }
  }
}
