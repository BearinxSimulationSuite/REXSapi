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

#include <rexsapi/XMLValueDecoder.hxx>

#include <vector>

#include <doctest.h>


namespace
{
  auto getNode(const pugi::xml_document& doc, const std::string& type)
  {
    return doc.select_node(fmt::format("/component[@id='1']/attribute[@id='{}']", type).c_str()).node();
  }
}


TEST_CASE("XML value decoder test")
{
  rexsapi::TXMLValueDecoder decoder;
  std::optional<rexsapi::database::TEnumValues> enumValue;

  std::string document = R"(
  <component id="1" type="test">
    <attribute id="boolean">true</attribute>
    <attribute id="float">47.11</attribute>
    <attribute id="integer">42</attribute>
    <attribute id="string">This is a string</attribute>
    <attribute id="enum">injection_lubrication</attribute>
    <attribute id="reference component">17</attribute>
    <attribute id="file reference">/root/my/path</attribute>
    <attribute id="coded float32 array">
      <array code="float32">MveeQZ6hM0I=</array>
    </attribute>
    <attribute id="coded float64 array">
      <array code="float64">AAAAAAAAHEAAAAAAAAAgQAAAAAAAACJA</array>
    </attribute>
    <attribute id="float array">
      <array>
        <c>1.0</c>
        <c>2.0</c>
        <c>3.0</c>
      </array>
    </attribute>
    <attribute id="coded integer array">
      <array code="int32">AQAAAAIAAAADAAAABAAAAAUAAAAGAAAABwAAAAgAAAA=</array>
    </attribute>
    <attribute id="integer array">
      <array>
        <c>1</c>
        <c>2</c>
        <c>3</c>
      </array>
    </attribute>
    <attribute id="boolean array">
      <array>
        <c>true</c>
        <c>false</c>
        <c>false</c>
      </array>
    </attribute>
    <attribute id="enum array">
      <array>
        <c>injection_lubrication</c>
        <c>dip_lubrication</c>
        <c>dip_lubrication</c>
      </array>
    </attribute>
    <attribute id="coded float matrix">
      <matrix code="float64" rows="3" columns="3">AAAAAAAA8D8AAAAAAAAAQAAAAAAAAAhAAAAAAAAAEEAAAAAAAAAUQAAAAAAAABhAAAAAAAAAHEAAAAAAAAAgQAAAAAAAACJA</matrix>
    </attribute>
    <attribute id="float matrix">
      <matrix>
        <r>
          <c>1.1</c>
          <c>1.2</c>
          <c>1.3</c>
        </r>
        <r>
          <c>2.1</c>
          <c>2.2</c>
          <c>2.3</c>
        </r>
        <r>
          <c>3.1</c>
          <c>3.2</c>
          <c>3.3</c>
        </r>
      </matrix>
    </attribute>
    <attribute id="array of integer arrays">
      <array_of_arrays>
        <array>
          <c>1</c>
          <c>1</c>
          <c>1</c>
        </array>
        <array>
          <c>2</c>
          <c>2</c>
        </array>
        <array>
          <c>3</c>
        </array>
      </array_of_arrays>
    </attribute>
  </component>
  )";
  std::vector<uint8_t> buffer{document.begin(), document.end()};
  pugi::xml_document doc;
  if (pugi::xml_parse_result parseResult = doc.load_buffer_inplace(buffer.data(), buffer.size()); !parseResult) {
    throw std::runtime_error{"cannot parse test xml"};
  }

  SUBCASE("Decode empty node")
  {
    pugi::xml_node node{};
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

  SUBCASE("Decode coded float32 array")
  {
    auto result =
      decoder.decode(rexsapi::TValueType::FLOATING_POINT_ARRAY, enumValue, getNode(doc, "coded float32 array"));
    CHECK(result.second);
    CHECK(result.first.getValue<std::vector<double>>().size() == 2);

    result = decoder.decode(rexsapi::TValueType::FLOATING_POINT_ARRAY, enumValue, getNode(doc, "coded float64 array"));
    CHECK(result.second);
    CHECK(result.first.getValue<std::vector<double>>().size() == 3);
  }

  SUBCASE("Decode boolean array")
  {
    auto result = decoder.decode(rexsapi::TValueType::BOOLEAN_ARRAY, enumValue, getNode(doc, "boolean array"));
    CHECK(result.second);
    CHECK(result.first.getValue<std::vector<rexsapi::Bool>>().size() == 3);
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

TEST_CASE("XML value decoder error test")
{
  rexsapi::TXMLValueDecoder decoder;
  std::optional<rexsapi::database::TEnumValues> enumValue;

  std::string document = R"(
  <component id="1" type="test">
    <attribute id="boolean">derk</attribute>
    <attribute id="float">puschel</attribute>
    <attribute id="integer">42.11</attribute>
    <attribute id="string"></attribute>
    <attribute id="enum">unknown enum</attribute>
    <attribute id="reference component">PR</attribute>
    <attribute id="integer array">
      <array>
        <c>1.1</c>
        <c>2.2</c>
        <c>3.3</c>
      </array>
    </attribute>
    <attribute id="float matrix">
      <matrix>
        <r>
          <c>1.1</c>
          <c>1.2</c>
          <c>1.3</c>
        </r>
        <r>
          <c>2.1</c>
          <c>2.2</c>
        </r>
        <r>
          <c>3.1</c>
          <c>3.2</c>
          <c>3.3</c>
        </r>
      </matrix>
    </attribute>
    <attribute id="coded float matrix 1">
      <matrix code="float64" rows="6" columns="3">AAAAAAAA8D8AAAAAAAAAQAAAAAAAAAhAAAAAAAAAEEAAAAAAAAAUQAAAAAAAABhAAAAAAAAAHEAAAAAAAAAgQAAAAAAAACJA</matrix>
    </attribute>
    <attribute id="coded float matrix 2">
      <matrix code="float32" rows="3" columns="3">AAAAAAAA8D8AAAAAAAAAQAAAAAAAAAhAAAAAAAAAEEAAAAAAAAAUQAAAAAAAABhAAAAAAAAAHEAAAAAAAAAgQAAAAAAAACJA</matrix>
    </attribute>
    <attribute id="coded integer array">
      <array code="int32">AQAAAAIAAAADAAAABAAAAAUAAAAGAAAABwAAAAgAAAA=</array>
    </attribute>
  </component>
  )";
  std::vector<uint8_t> buffer{document.begin(), document.end()};
  pugi::xml_document doc;
  if (pugi::xml_parse_result parseResult = doc.load_buffer_inplace(buffer.data(), buffer.size()); !parseResult) {
    throw std::runtime_error{"cannot parse test xml"};
  }

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
    CHECK_FALSE(decoder.decode(rexsapi::TValueType::ENUM, {}, getNode(doc, "enum")).second);
  }

  SUBCASE("Decode integer array")
  {
    CHECK_FALSE(decoder.decode(rexsapi::TValueType::INTEGER_ARRAY, enumValue, getNode(doc, "integer array")).second);
  }

  SUBCASE("Decode coded integer array")
  {
    CHECK_FALSE(decoder.decode(rexsapi::TValueType::FLOATING_POINT, enumValue, getNode(doc, "integer array")).second);
  }

  SUBCASE("Decode float matrix")
  {
    CHECK_FALSE(
      decoder.decode(rexsapi::TValueType::FLOATING_POINT_MATRIX, enumValue, getNode(doc, "float matrix")).second);
  }

  SUBCASE("Decode coded float matrix wrong rows")
  {
    CHECK_FALSE(
      decoder.decode(rexsapi::TValueType::FLOATING_POINT_MATRIX, enumValue, getNode(doc, "coded float matrix 1"))
        .second);
  }

  SUBCASE("Decode coded float matrix wrong type size")
  {
    CHECK_FALSE(
      decoder.decode(rexsapi::TValueType::FLOATING_POINT_MATRIX, enumValue, getNode(doc, "coded float matrix 2"))
        .second);
  }
}

TEST_CASE("XML unknown value decoder test")
{
  rexsapi::TXMLValueDecoder decoder;
  pugi::xml_document doc;

  SUBCASE("Decode string")
  {
    auto node = doc.append_child("attribute");
    node.append_child(pugi::node_pcdata).set_value("test");
    auto res = decoder.decodeUnknown(node);
    CHECK(res.second == rexsapi::TValueType::STRING);
    CHECK(res.first == rexsapi::TValue{"test"});
  }

  SUBCASE("Decode array")
  {
    auto node = doc.append_child("attribute");
    auto arrayNode = node.append_child("array");
    auto child = arrayNode.append_child("c");
    child.append_child(pugi::node_pcdata).set_value("1");
    child = arrayNode.append_child("c");
    child.append_child(pugi::node_pcdata).set_value("2");
    child = arrayNode.append_child("c");
    child.append_child(pugi::node_pcdata).set_value("3");

    auto res = decoder.decodeUnknown(node);
    CHECK(res.second == rexsapi::TValueType::STRING_ARRAY);
    auto val = res.first.getValue<rexsapi::TStringArrayType>();
    CHECK(val.size() == 3);
  }

  SUBCASE("Decode matrix")
  {
    auto node = doc.append_child("attribute");
    auto matrixNode = node.append_child("matrix");
    auto rowNode = matrixNode.append_child("r");
    auto child = rowNode.append_child("c");
    child.append_child(pugi::node_pcdata).set_value("a");
    child = rowNode.append_child("c");
    child.append_child(pugi::node_pcdata).set_value("b");
    rowNode = matrixNode.append_child("r");
    child = rowNode.append_child("c");
    child.append_child(pugi::node_pcdata).set_value("c");
    child = rowNode.append_child("c");
    child.append_child(pugi::node_pcdata).set_value("d");

    auto res = decoder.decodeUnknown(node);
    CHECK(res.second == rexsapi::TValueType::STRING_MATRIX);
    auto val = res.first.getValue<rexsapi::TStringMatrixType>();
    CHECK(val.m_Values.size() == 2);
    CHECK(val.validate());
  }

  SUBCASE("Decode array of integer arrays")
  {
    auto node = doc.append_child("attribute");
    auto arraysNode = node.append_child("array_of_arrays");
    auto aNode = arraysNode.append_child("array");
    auto child = aNode.append_child("c");
    child.append_child(pugi::node_pcdata).set_value("1");
    aNode = arraysNode.append_child("array");
    child = aNode.append_child("c");
    child.append_child(pugi::node_pcdata).set_value("1");
    child.append_child(pugi::node_pcdata).set_value("2");
    aNode = arraysNode.append_child("array");
    child = aNode.append_child("c");
    child.append_child(pugi::node_pcdata).set_value("1");
    child.append_child(pugi::node_pcdata).set_value("2");
    child.append_child(pugi::node_pcdata).set_value("3");

    auto res = decoder.decodeUnknown(node);
    CHECK(res.second == rexsapi::TValueType::ARRAY_OF_INTEGER_ARRAYS);
    auto val = res.first.getValue<rexsapi::TArrayOfIntArraysType>();
    CHECK(val.size() == 3);
  }
}
