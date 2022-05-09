
#include <rexsapi/XMLParser.hxx>
#include <rexsapi/XMLValueDecoder.hxx>

#include <doctest.h>
#include <vector>

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
    <attribute id="reference component">42</attribute>
    <attribute id="file reference">/root/my/path</attribute>
    <attribute id="float array">
      <array>
        <c>1.0</c>
        <c>2.0</c>
        <c>3.0</c>
      </array>
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
    CHECK_FALSE(decoder.decode(rexsapi::database::TValueType::BOOLEAN, enumValue, node));
  }

  SUBCASE("Decode boolean")
  {
    auto node = doc.select_node("/component[@id='1']/attribute[@id='boolean']").node();
    CHECK(decoder.decode(rexsapi::database::TValueType::BOOLEAN, enumValue, node));
  }

  SUBCASE("Decode integer")
  {
    auto node = doc.select_node("/component[@id='1']/attribute[@id='integer']").node();
    CHECK(decoder.decode(rexsapi::database::TValueType::INTEGER, enumValue, node));
  }

  SUBCASE("Decode float")
  {
    auto node = doc.select_node("/component[@id='1']/attribute[@id='float']").node();
    CHECK(decoder.decode(rexsapi::database::TValueType::FLOATING_POINT, enumValue, node));
  }

  SUBCASE("Decode string")
  {
    auto node = doc.select_node("/component[@id='1']/attribute[@id='string']").node();
    CHECK(decoder.decode(rexsapi::database::TValueType::STRING, enumValue, node));
  }

  SUBCASE("Decode reference component")
  {
    auto node = doc.select_node("/component[@id='1']/attribute[@id='reference component']").node();
    CHECK(decoder.decode(rexsapi::database::TValueType::REFERENCE_COMPONENT, enumValue, node));
  }

  SUBCASE("Decode file reference")
  {
    auto node = doc.select_node("/component[@id='1']/attribute[@id='file reference']").node();
    CHECK(decoder.decode(rexsapi::database::TValueType::FILE_REFERENCE, enumValue, node));
  }

  SUBCASE("Decode enumValue")
  {
    enumValue = rexsapi::database::TEnumValues{{rexsapi::database::TEnumValue{"dip_lubrication", ""},
                                                rexsapi::database::TEnumValue{"injection_lubrication", ""}}};

    auto node = doc.select_node("/component[@id='1']/attribute[@id='enum']").node();
    CHECK(decoder.decode(rexsapi::database::TValueType::ENUM, enumValue, node));
  }

  SUBCASE("Decode integer array")
  {
    auto node = doc.select_node("/component[@id='1']/attribute[@id='integer array']").node();
    CHECK(decoder.decode(rexsapi::database::TValueType::INTEGER_ARRAY, enumValue, node));
  }

  SUBCASE("Decode float array")
  {
    auto node = doc.select_node("/component[@id='1']/attribute[@id='float array']").node();
    CHECK(decoder.decode(rexsapi::database::TValueType::FLOATING_POINT_ARRAY, enumValue, node));
  }

  SUBCASE("Decode boolean array")
  {
    auto node = doc.select_node("/component[@id='1']/attribute[@id='boolean array']").node();
    CHECK(decoder.decode(rexsapi::database::TValueType::BOOLEAN_ARRAY, enumValue, node));
  }

  SUBCASE("Decode enumValue array")
  {
    enumValue = rexsapi::database::TEnumValues{{rexsapi::database::TEnumValue{"dip_lubrication", ""},
                                                rexsapi::database::TEnumValue{"injection_lubrication", ""}}};

    auto node = doc.select_node("/component[@id='1']/attribute[@id='enum array']").node();
    CHECK(decoder.decode(rexsapi::database::TValueType::ENUM_ARRAY, enumValue, node));
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
  </component>
  )";
  std::vector<uint8_t> buffer{document.begin(), document.end()};
  pugi::xml_document doc;
  if (pugi::xml_parse_result parseResult = doc.load_buffer_inplace(buffer.data(), buffer.size()); !parseResult) {
    throw std::runtime_error{"cannot parse test xml"};
  }

  SUBCASE("Decode boolean")
  {
    auto node = doc.select_node("/component[@id='1']/attribute[@id='boolean']").node();
    CHECK_FALSE(decoder.decode(rexsapi::database::TValueType::BOOLEAN, enumValue, node));
  }

  SUBCASE("Decode integer")
  {
    auto node = doc.select_node("/component[@id='1']/attribute[@id='integer']").node();
    CHECK_FALSE(decoder.decode(rexsapi::database::TValueType::INTEGER, enumValue, node));
  }

  SUBCASE("Decode float")
  {
    auto node = doc.select_node("/component[@id='1']/attribute[@id='float']").node();
    CHECK_FALSE(decoder.decode(rexsapi::database::TValueType::FLOATING_POINT, enumValue, node));
  }

  SUBCASE("Decode string")
  {
    auto node = doc.select_node("/component[@id='1']/attribute[@id='string']").node();
    CHECK_FALSE(decoder.decode(rexsapi::database::TValueType::STRING, enumValue, node));
  }

  SUBCASE("Decode reference component")
  {
    auto node = doc.select_node("/component[@id='1']/attribute[@id='reference component']").node();
    CHECK_FALSE(decoder.decode(rexsapi::database::TValueType::REFERENCE_COMPONENT, enumValue, node));
  }

  SUBCASE("Decode enumValue")
  {
    enumValue = rexsapi::database::TEnumValues{{rexsapi::database::TEnumValue{"dip_lubrication", ""},
                                                rexsapi::database::TEnumValue{"injection_lubrication", ""}}};

    auto node = doc.select_node("/component[@id='1']/attribute[@id='enum']").node();
    CHECK_FALSE(decoder.decode(rexsapi::database::TValueType::ENUM, enumValue, node));
    CHECK_FALSE(decoder.decode(rexsapi::database::TValueType::ENUM, {}, node));
  }

  SUBCASE("Decode integer array")
  {
    auto node = doc.select_node("/component[@id='1']/attribute[@id='integer array']").node();
    CHECK_FALSE(decoder.decode(rexsapi::database::TValueType::INTEGER_ARRAY, enumValue, node));
  }
}
