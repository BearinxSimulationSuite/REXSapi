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

#include <rexsapi/JsonValueDecoder.hxx>

#include <doctest.h>
#include <vector>

namespace
{
  auto getNode(const rexsapi::json& doc, const std::string& type)
  {
    return doc[rexsapi::json::json_pointer("/" + type)];
  }
}

TEST_CASE("Json value decoder test")
{
  rexsapi::TJsonValueDecoder decoder;
  std::optional<rexsapi::database::TEnumValues> enumValue;

  std::string document = R"(
  {
    "boolean": { "boolean": true },
    "float": { "floating_point": 47.11 },
    "integer": { "integer": 42 },
    "string": { "string": "This is a string" },
    "enum": { "enum": "injection_lubrication" },
    "reference component": { "reference_component": 17 },
    "file reference": { "file_reference": "/root/my/path" },
    "float array": { "floating_point_array": [1.0, 2.0, 3.0] },
    "coded float array": { "floating_point_array_coded": { "code": "float32", "value": "MveeQZ6hM0I" } },
    "integer array": { "integer_array": [1, 2, 3] },
    "coded integer array": { "integer_array_coded": { "code": "int32", "value": "AQAAAAIAAAADAAAABAAAAAUAAAAGAAAABwAAAAgAAAA=" } },
    "boolean array": { "boolean_array": [true, false, false] },
    "string array": { "string_array": ["a", "b", "c"] },
    "enum array": { "enum_array": ["injection_lubrication", "dip_lubrication", "dip_lubrication"] },
    "float matrix": { "floating_point_matrix": [[1.1, 1.2, 1.3], [2.1, 2.2, 2.3], [3.1, 3.2, 3.3]] },
    "coded float matrix": { "floating_point_matrix_coded": { "code": "float64", "rows": 3, "columns": 3, "value": "AAAAAAAA8D8AAAAAAAAAQAAAAAAAAAhAAAAAAAAAEEAAAAAAAAAUQAAAAAAAABhAAAAAAAAAHEAAAAAAAAAgQAAAAAAAACJA" } },
    "string matrix": { "string_matrix": [["a", "b"], ["c", "d"], ["e", "f"]] },
    "array of integer arrays": { "array_of_integer_arrays": [[1, 1, 1], [2, 2], [3]] }
  }
  )";
  std::vector<uint8_t> buffer{document.begin(), document.end()};
  rexsapi::json doc = rexsapi::json::parse(buffer);

  SUBCASE("Decode empty node")
  {
    rexsapi::json node{};
    CHECK_FALSE(decoder.decode(rexsapi::TValueType::BOOLEAN, enumValue, node).second);
  }

  SUBCASE("Decode boolean")
  {
    auto result = decoder.decode(rexsapi::TValueType::BOOLEAN, enumValue, getNode(doc, "boolean"));
    CHECK(result.second);
    CHECK(result.first.getValue<rexsapi::Bool>());
  }

  SUBCASE("Decode integer")
  {
    auto result = decoder.decode(rexsapi::TValueType::INTEGER, enumValue, getNode(doc, "integer"));
    CHECK(result.second);
    CHECK(result.first.getValue<int64_t>() == 42);
  }

  SUBCASE("Decode float")
  {
    auto result = decoder.decode(rexsapi::TValueType::FLOATING_POINT, enumValue, getNode(doc, "float"));
    CHECK(result.second);
    CHECK(result.first.getValue<double>() == doctest::Approx(47.11));
  }

  SUBCASE("Decode string")
  {
    auto result = decoder.decode(rexsapi::TValueType::STRING, enumValue, getNode(doc, "string"));
    CHECK(result.second);
    CHECK(result.first.getValue<std::string>() == "This is a string");
  }

  SUBCASE("Decode reference component")
  {
    auto result =
      decoder.decode(rexsapi::TValueType::REFERENCE_COMPONENT, enumValue, getNode(doc, "reference component"));
    CHECK(result.second);
    CHECK(result.first.getValue<int64_t>() == 17);
  }

  SUBCASE("Decode file reference")
  {
    auto result = decoder.decode(rexsapi::TValueType::FILE_REFERENCE, enumValue, getNode(doc, "file reference"));
    CHECK(result.second);
    CHECK(result.first.getValue<std::string>() == "/root/my/path");
  }

  SUBCASE("Decode enumValue")
  {
    enumValue = rexsapi::database::TEnumValues{{rexsapi::database::TEnumValue{"dip_lubrication", ""},
                                                rexsapi::database::TEnumValue{"injection_lubrication", ""}}};

    auto result = decoder.decode(rexsapi::TValueType::ENUM, enumValue, getNode(doc, "enum"));
    CHECK(result.second);
    CHECK(result.first.getValue<std::string>() == "injection_lubrication");
  }

  SUBCASE("Decode integer array")
  {
    auto result = decoder.decode(rexsapi::TValueType::INTEGER_ARRAY, enumValue, getNode(doc, "integer array"));
    CHECK(result.second);
    REQUIRE(result.first.getValue<std::vector<int64_t>>().size() == 3);
    CHECK(result.first.getValue<std::vector<int64_t>>()[1] == 2);
  }

  SUBCASE("Decode coded integer array")
  {
    auto result = decoder.decode(rexsapi::TValueType::INTEGER_ARRAY, enumValue, getNode(doc, "coded integer array"));
    CHECK(result.second);
    REQUIRE(result.first.getValue<std::vector<int64_t>>().size() == 8);
    CHECK(result.first.getValue<std::vector<int64_t>>()[1] == 2);
  }

  SUBCASE("Decode float array")
  {
    auto result = decoder.decode(rexsapi::TValueType::FLOATING_POINT_ARRAY, enumValue, getNode(doc, "float array"));
    CHECK(result.second);
    CHECK(result.first.getValue<std::vector<double>>().size() == 3);
  }

  SUBCASE("Decode coded float array")
  {
    auto result =
      decoder.decode(rexsapi::TValueType::FLOATING_POINT_ARRAY, enumValue, getNode(doc, "coded float array"));
    CHECK(result.second);
    CHECK(result.first.getValue<std::vector<double>>().size() == 2);
  }

  SUBCASE("Decode boolean array")
  {
    auto result = decoder.decode(rexsapi::TValueType::BOOLEAN_ARRAY, enumValue, getNode(doc, "boolean array"));
    CHECK(result.second);
    CHECK(result.first.getValue<std::vector<rexsapi::Bool>>().size() == 3);
  }

  SUBCASE("Decode string array")
  {
    auto result = decoder.decode(rexsapi::TValueType::STRING_ARRAY, enumValue, getNode(doc, "string array"));
    CHECK(result.second);
    CHECK(result.first.getValue<std::vector<std::string>>().size() == 3);
  }

  SUBCASE("Decode enumValue array")
  {
    enumValue = rexsapi::database::TEnumValues{{rexsapi::database::TEnumValue{"dip_lubrication", ""},
                                                rexsapi::database::TEnumValue{"injection_lubrication", ""}}};

    auto result = decoder.decode(rexsapi::TValueType::ENUM_ARRAY, enumValue, getNode(doc, "enum array"));
    CHECK(result.second);
    CHECK(result.first.getValue<std::vector<std::string>>().size() == 3);
  }

  SUBCASE("Decode float matrix")
  {
    auto result = decoder.decode(rexsapi::TValueType::FLOATING_POINT_MATRIX, enumValue, getNode(doc, "float matrix"));
    CHECK(result.second);
    CHECK(result.first.getValue<rexsapi::TMatrix<double>>().m_Values.size() == 3);
    for (const auto& row : result.first.getValue<rexsapi::TMatrix<double>>().m_Values) {
      CHECK(row.size() == 3);
    }
  }

  SUBCASE("Decode coded float matrix")
  {
    auto result =
      decoder.decode(rexsapi::TValueType::FLOATING_POINT_MATRIX, enumValue, getNode(doc, "coded float matrix"));
    CHECK(result.second);
    CHECK(result.first.getValue<rexsapi::TMatrix<double>>().m_Values.size() == 3);
    for (const auto& row : result.first.getValue<rexsapi::TMatrix<double>>().m_Values) {
      CHECK(row.size() == 3);
    }
  }

  SUBCASE("Decode string matrix")
  {
    auto result = decoder.decode(rexsapi::TValueType::STRING_MATRIX, enumValue, getNode(doc, "string matrix"));
    CHECK(result.second);
    CHECK(result.first.getValue<rexsapi::TMatrix<std::string>>().m_Values.size() == 3);
    for (const auto& row : result.first.getValue<rexsapi::TMatrix<std::string>>().m_Values) {
      CHECK(row.size() == 2);
    }
  }

  SUBCASE("Decode array of integer arrays")
  {
    auto result =
      decoder.decode(rexsapi::TValueType::ARRAY_OF_INTEGER_ARRAYS, enumValue, getNode(doc, "array of integer arrays"));
    CHECK(result.second);
    REQUIRE(result.first.getValue<std::vector<std::vector<int64_t>>>().size() == 3);
    CHECK(result.first.getValue<std::vector<std::vector<int64_t>>>()[0].size() == 3);
    CHECK(result.first.getValue<std::vector<std::vector<int64_t>>>()[1].size() == 2);
    CHECK(result.first.getValue<std::vector<std::vector<int64_t>>>()[2].size() == 1);
  }
}


TEST_CASE("Json value decoder error test")
{
  rexsapi::TJsonValueDecoder decoder;
  std::optional<rexsapi::database::TEnumValues> enumValue;

  std::string document = R"(
  {
    "boolean": { "boolean": "derk" },
    "float": { "floating_point": "puschel" },
    "integer": { "integer": "pi" },
    "string": { "string": "" },
    "enum": { "enum": "unknown enum" },
    "reference component": { "reference_component": "PR" },
    "file reference": { "file_reference": 4711 },
    "float array": { "floating_point_array": ["a", "b", "c"] },
    "integer array": { "integer_array": ["a", "b", "c"] },
    "boolean array": { "boolean_array": ["true", "false", "false"] },
    "string array": { "string_array": [1, 2, 3] },
    "enum array": { "enum_array": [1, 2, 3] },
    "float matrix": { "floating_point_matrix": [[1.1, 1.2, 1.3], [2.1, 2.2], [3.1, 3.2, 3.3]] },
    "coded float matrix": { "floating_point_matrix_coded": { "code": "float64", "rows": 5, "columns": 3, "value": "AAAAAAAA8D8AAAAAAAAAQAAAAAAAAAhAAAAAAAAAEEAAAAAAAAAUQAAAAAAAABhAAAAAAAAAHEAAAAAAAAAgQAAAAAAAACJA" } },
    "string matrix": { "string_matrix": [["a", "b"], ["c"], ["e", "f"]] },
    "array of integer arrays": { "array_of_integer_arrays": [["a", "b", "c"], ["d", "e"], ["f"]] }
  }
  )";
  std::vector<uint8_t> buffer{document.begin(), document.end()};
  rexsapi::json doc = rexsapi::json::parse(buffer);

  SUBCASE("Decode boolean")
  {
    CHECK_FALSE(decoder.decode(rexsapi::TValueType::BOOLEAN, enumValue, getNode(doc, "boolean")).second);
  }

  SUBCASE("Decode integer")
  {
    CHECK_FALSE(decoder.decode(rexsapi::TValueType::INTEGER, enumValue, getNode(doc, "integer")).second);
  }

  SUBCASE("Decode float")
  {
    CHECK_FALSE(decoder.decode(rexsapi::TValueType::FLOATING_POINT, enumValue, getNode(doc, "float")).second);
  }

  SUBCASE("Decode string")
  {
    CHECK_FALSE(decoder.decode(rexsapi::TValueType::STRING, enumValue, getNode(doc, "string")).second);
  }

  SUBCASE("Decode file reference")
  {
    CHECK_FALSE(decoder.decode(rexsapi::TValueType::STRING, enumValue, getNode(doc, "file reference")).second);
  }

  SUBCASE("Decode reference component")
  {
    CHECK_FALSE(
      decoder.decode(rexsapi::TValueType::REFERENCE_COMPONENT, enumValue, getNode(doc, "reference component")).second);
  }

  SUBCASE("Decode enumValue")
  {
    enumValue = rexsapi::database::TEnumValues{{rexsapi::database::TEnumValue{"dip_lubrication", ""},
                                                rexsapi::database::TEnumValue{"injection_lubrication", ""}}};

    CHECK_FALSE(decoder.decode(rexsapi::TValueType::ENUM, enumValue, getNode(doc, "enum")).second);
    CHECK(decoder.decode(rexsapi::TValueType::ENUM, {}, getNode(doc, "enum")).second);
  }

  SUBCASE("Decode integer array")
  {
    CHECK_FALSE(decoder.decode(rexsapi::TValueType::INTEGER_ARRAY, enumValue, getNode(doc, "integer array")).second);
  }

  SUBCASE("Decode float array")
  {
    CHECK_FALSE(
      decoder.decode(rexsapi::TValueType::FLOATING_POINT_ARRAY, enumValue, getNode(doc, "float array")).second);
  }

  SUBCASE("Decode boolean array")
  {
    CHECK_FALSE(decoder.decode(rexsapi::TValueType::INTEGER_ARRAY, enumValue, getNode(doc, "boolean array")).second);
  }

  SUBCASE("Decode enum array")
  {
    CHECK_FALSE(decoder.decode(rexsapi::TValueType::ENUM_ARRAY, enumValue, getNode(doc, "enum array")).second);
  }

  SUBCASE("Decode string array")
  {
    CHECK_FALSE(decoder.decode(rexsapi::TValueType::STRING_ARRAY, enumValue, getNode(doc, "string array")).second);
  }

  SUBCASE("Decode float matrix")
  {
    CHECK_FALSE(
      decoder.decode(rexsapi::TValueType::FLOATING_POINT_MATRIX, enumValue, getNode(doc, "float matrix")).second);
  }

  SUBCASE("Decode coded float matrix")
  {
    CHECK_FALSE(
      decoder.decode(rexsapi::TValueType::FLOATING_POINT_MATRIX, enumValue, getNode(doc, "coded float matrix")).second);
  }

  SUBCASE("Decode string matrix")
  {
    CHECK_FALSE(decoder.decode(rexsapi::TValueType::STRING_MATRIX, enumValue, getNode(doc, "string matrix")).second);
  }

  SUBCASE("Decode array of integer arrays")
  {
    CHECK_FALSE(
      decoder.decode(rexsapi::TValueType::ARRAY_OF_INTEGER_ARRAYS, enumValue, getNode(doc, "array of integer arrays"))
        .second);
  }
}
