
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
    <attribute id="enum">injection_lubrication</attribute>
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
}
