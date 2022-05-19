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
      rexsapi::TValue v{"47.11"};

      std::string res =
        rexsapi::dispatch<std::string>(rexsapi::TValueType::STRING, v,
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
      CHECK(res == "string 47.11");
    }
  }
}
