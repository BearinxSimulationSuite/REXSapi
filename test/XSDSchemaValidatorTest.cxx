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

#include <rexsapi/XSDSchemaValidator.hxx>

#include <test/TestHelper.hxx>

#include <doctest.h>

namespace
{
  const auto* const schema = R"(
      <?xml version="1.0" encoding="UTF-8"?>
      <xsd:schema xmlns:xsd="http://www.w3.org/2001/XMLSchema">
        <xsd:element name="TestCases">
          <xsd:complexType>
            <xsd:sequence>
              <xsd:element name="Annotation" type="xsd:string" maxOccurs="1" minOccurs="0"/>
              <xsd:element ref="Suites" maxOccurs="1" minOccurs="1"/>
              <xsd:element ref="Tests" maxOccurs="1" minOccurs="0"/>
            </xsd:sequence>
            <xsd:attribute name="version" type="xsd:integer" use="required"/>
            <xsd:attribute name="date" type="xsd:string" use="optional"/>
            <xsd:attribute name="status" type="Status" use="optional"/>
          </xsd:complexType>
        </xsd:element>
        <xsd:element name="Suites">
          <xsd:complexType>
            <xsd:sequence>
              <xsd:element ref="Suite" maxOccurs="unbounded" minOccurs="1"/>
            </xsd:sequence>
          </xsd:complexType>
        </xsd:element>
        <xsd:element name="Suite">
          <xsd:complexType>
            <xsd:sequence>
              <xsd:element ref="Tests" maxOccurs="1" minOccurs="1"/>
            </xsd:sequence>
            <xsd:attribute name="name" type="xsd:string" use="required"/>
          </xsd:complexType>
        </xsd:element>        
        <xsd:element name="Tests">
          <xsd:complexType>
            <xsd:sequence>
              <xsd:element ref="Test" maxOccurs="unbounded" minOccurs="2"/>
              <xsd:element ref="Revision" maxOccurs="1" minOccurs="0"/>
            </xsd:sequence>
          </xsd:complexType>
        </xsd:element>        
        <xsd:element name="Test">
          <xsd:complexType>
            <xsd:attribute name="name" type="xsd:string" use="required"/>
            <xsd:attribute name="number" type="Number" use="optional"/>
            <xsd:anyAttribute processContents="skip"/>
          </xsd:complexType>
        </xsd:element>
        <xsd:simpleType name="Status">
          <xsd:restriction base="xsd:string">
            <xsd:enumeration value="COOL"/>
            <xsd:enumeration value="OH NO"/>
          </xsd:restriction>
        </xsd:simpleType>
        <xsd:element name="Revision">
          <xsd:complexType>
            <xsd:simpleContent>
              <xsd:extension base="Number"></xsd:extension>
            </xsd:simpleContent>
          </xsd:complexType>
        </xsd:element>
        <xsd:simpleType name="Number">
          <xsd:restriction base="xsd:integer"/>
        </xsd:simpleType>
      </xsd:schema>
    )";

  bool validate(const char* document, std::vector<std::string>& errors)
  {
    pugi::xml_document doc;
    pugi::xml_parse_result parseResult = doc.load_string(document);
    CHECK(parseResult);

    rexsapi::xml::TBufferXsdSchemaLoader loader{schema};
    rexsapi::xml::TXSDSchemaValidator val{loader};
    return val.validate(doc, errors);
  }
}

TEST_CASE("XSD file schema loader test")
{
  SUBCASE("Load existing schema")
  {
    rexsapi::xml::TFileXsdSchemaLoader loader{projectDir() / "models" / "rexs-dbmodel.xsd"};
    auto doc = loader.load();
    CHECK_FALSE(doc.empty());
  }

  SUBCASE("Load non-existing schema")
  {
    CHECK_THROWS(rexsapi::xml::TFileXsdSchemaLoader{projectDir() / "models" / "non-existing-schema.xsd"});
    CHECK_THROWS(rexsapi::xml::TFileXsdSchemaLoader{projectDir() / "models"});
  }
}

TEST_CASE("XSD schema validator test")
{
  SUBCASE("Validate db model schema from file")
  {
    pugi::xml_document doc;
    pugi::xml_parse_result parseResult =
      doc.load_file((projectDir() / "models" / "rexs_model_1.4_en.xml").string().c_str());
    CHECK(parseResult);

    rexsapi::xml::TFileXsdSchemaLoader loader{projectDir() / "models" / "rexs-dbmodel.xsd"};
    rexsapi::xml::TXSDSchemaValidator val{loader};

    std::vector<std::string> errors;
    CHECK(val.validate(doc, errors));
    CHECK(errors.empty());
  }

  SUBCASE("Validate model from file")
  {
    pugi::xml_document doc;
    pugi::xml_parse_result parseResult =
      doc.load_file((projectDir() / "test" / "example_models" / "FVA_worm_stage_1-4.rexs").string().c_str());
    CHECK(parseResult);

    rexsapi::xml::TFileXsdSchemaLoader loader{projectDir() / "models" / "rexs-schema.xsd"};
    rexsapi::xml::TXSDSchemaValidator val{loader};

    std::vector<std::string> errors;
    CHECK(val.validate(doc, errors));
    CHECK(errors.empty());
  }

  SUBCASE("Validate schema from string")
  {
    const auto* xml = R"(
      <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
      <TestCases version="42" status="COOL" date="2022-04-28T11:10">
        <Annotation>Puschelb??r</Annotation>
        <Suites>
          <Suite name="suite 1">
            <Tests>
              <Test name="1.1" />
              <Test name="1.2" />
            </Tests>
          </Suite>
          <Suite name="suite 2">
            <Tests>
              <Test name="2.1" />
              <Test name="2.2" />
              <Test name="2.3" />
            </Tests>
          </Suite>
        </Suites>
        <Tests>
          <!-- tests anyAttribute feature -->
          <Test name="1" puschel="check" />
          <Test name="2" />
          <Test name="3" />
        </Tests>
      </TestCases>
    )";

    std::vector<std::string> errors;
    CHECK(validate(xml, errors));
    CHECK(errors.empty());
  }

  SUBCASE("Simple content type")
  {
    const auto* xml = R"(
      <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
      <TestCases version="42">
        <Suites>
          <Suite name="suite 1">
            <Tests>
              <Test name="1.1" />
              <Test name="1.2" />
              <Revision>17</Revision>
            </Tests>
          </Suite>
        </Suites>
      </TestCases>
    )";

    std::vector<std::string> errors;
    CHECK(validate(xml, errors));
    CHECK(errors.empty());
  }
}

TEST_CASE("XSD schema validator failure test")
{
  SUBCASE("Missing required attribute")
  {
    const auto* xml = R"(
      <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
      <TestCases date="2022-04-28T11:10">
        <Suites />
      </TestCases>
    )";

    std::vector<std::string> errors;
    CHECK_FALSE(validate(xml, errors));
    REQUIRE(errors.size() == 2);
    CHECK(errors[0] == "[/TestCases/] missing required attribute 'version'");
    CHECK(errors[1] == "[/TestCases/Suites/] too few 'Suite' elements, found 0 instead of at least 1");
  }

  SUBCASE("Missing required elements")
  {
    const auto* xml = R"(
      <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
      <TestCases version="42">
        <Tests />
      </TestCases>
    )";

    std::vector<std::string> errors;
    CHECK_FALSE(validate(xml, errors));
    REQUIRE(errors.size() == 2);
    CHECK(errors[0] == "[/TestCases/] too few 'Suites' elements, found 0 instead of at least 1");
    CHECK(errors[1] == "[/TestCases/Tests/] too few 'Test' elements, found 0 instead of at least 2");
  }

  SUBCASE("Wrong element")
  {
    const auto* xml = R"(
      <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
      <TestCases version="42">
        <Suites>
          <Tests />
          <Suite name="suite 1">
            <Tests>
              <Test name="1.1" />
              <Test name="1.2" />
            </Tests>
            <P??schel />
          </Suite>
        </Suites>
      </TestCases>
    )";

    std::vector<std::string> errors;
    CHECK_FALSE(validate(xml, errors));
    REQUIRE(errors.size() == 2);
    CHECK(errors[0] == "[/TestCases/Suites/] element 'Tests' is not allowed here");
    CHECK(errors[1] == "[/TestCases/Suites/Suite/] unkown element 'P??schel'");
  }

  SUBCASE("Wrong attribute")
  {
    const auto* xml = R"(
      <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
      <TestCases version="42" timestamp="2022-04-28T12:28">
        <Suites>
          <Suite name="suite 1">
            <Tests>
              <Test name="1.1" />
              <Test name="1.2" />
            </Tests>
          </Suite>
        </Suites>
      </TestCases>
    )";

    std::vector<std::string> errors;
    CHECK_FALSE(validate(xml, errors));
    REQUIRE(errors.size() == 1);
    CHECK(errors[0] == "[/TestCases/] unknown attribute 'timestamp'");
  }

  SUBCASE("Too much elements")
  {
    const auto* xml = R"(
      <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
      <TestCases version="42">
        <Suites>
          <Suite name="suite 1">
            <Tests>
              <Test name="1.1" />
              <Test name="1.2" />
            </Tests>
          </Suite>
        </Suites>
        <Suites>
          <Suite name="suite 1">
            <Tests>
              <Test name="1.1" />
              <Test name="1.2" />
            </Tests>
          </Suite>
        </Suites>
      </TestCases>
    )";

    std::vector<std::string> errors;
    CHECK_FALSE(validate(xml, errors));
    REQUIRE(errors.size() == 1);
    CHECK(errors[0] == "[/TestCases/] too many 'Suites' elements, found 2 instead of at most 1");
  }

  SUBCASE("Wrong attribute type")
  {
    const auto* xml = R"(
      <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
      <TestCases version="p??schel">
        <Suites>
          <Suite name="suite 1">
            <Tests>
              <Test name="1.1" />
              <Test name="1.2" />
            </Tests>
          </Suite>
        </Suites>
      </TestCases>
    )";

    std::vector<std::string> errors;
    CHECK_FALSE(validate(xml, errors));
    REQUIRE(errors.size() == 1);
    CHECK(errors[0] == "[/TestCases/version/] cannot convert 'p??schel' to integer");
  }

  SUBCASE("Unkown enum value")
  {
    const auto* xml = R"(
      <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
      <TestCases version="41" status="BAD">
        <Suites>
          <Suite name="suite 1">
            <Tests>
              <Test name="1.1" />
              <Test name="1.2" />
            </Tests>
          </Suite>
        </Suites>
      </TestCases>
    )";

    std::vector<std::string> errors;
    CHECK_FALSE(validate(xml, errors));
    REQUIRE(errors.size() == 1);
    CHECK(errors[0] == "[/TestCases/status/] unknown enum value 'BAD' for type 'Status'");
  }

  SUBCASE("Wrong restriction value")
  {
    const auto* xml = R"(
      <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
      <TestCases version="41">
        <Suites>
          <Suite name="suite 1">
            <Tests>
              <Test name="1.1" number="4711" />
              <Test name="1.2" number="no number" />
            </Tests>
          </Suite>
        </Suites>
      </TestCases>
    )";

    std::vector<std::string> errors;
    CHECK_FALSE(validate(xml, errors));
    REQUIRE(errors.size() == 1);
    CHECK(errors[0] == "[/TestCases/Suites/Suite/Tests/Test/number/] cannot convert 'no number' to integer");
  }

  SUBCASE("Simple content type without value")
  {
    const auto* xml = R"(
      <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
      <TestCases version="42">
        <Suites>
          <Suite name="suite 1">
            <Tests>
              <Test name="1.1" />
              <Test name="1.2" />
              <Revision />
            </Tests>
          </Suite>
        </Suites>
      </TestCases>
    )";

    std::vector<std::string> errors;
    CHECK_FALSE(validate(xml, errors));
    REQUIRE(errors.size() == 1);
    CHECK(errors[0] == "[/TestCases/Suites/Suite/Tests/Revision/] element 'Revision' does not have a value");
  }

  SUBCASE("Value on non-mixed type element")
  {
    const auto* xml = R"(
      <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
      <TestCases version="42">
        <Suites>
          <Suite name="suite 1">
            <Tests>
              <Test name="1.1" />
              <Test name="1.2">hutzli</Test>
            </Tests>
          </Suite>
        </Suites>
      </TestCases>
    )";

    std::vector<std::string> errors;
    CHECK_FALSE(validate(xml, errors));
    REQUIRE(errors.size() == 1);
    CHECK(errors[0] == "[/TestCases/Suites/Suite/Tests/Test/] element has value but is not of mixed type");
  }
}

namespace
{
  template<typename Type>
  bool check(const std::string& value)
  {
    rexsapi::xml::TElements elements;
    rexsapi::xml::TValidationContext context{elements};

    Type t;
    t.validate(value, context);
    return !context.hasErrors();
  }

  template<typename Type>
  bool check(const Type& t, const std::string& value)
  {
    rexsapi::xml::TElements elements;
    rexsapi::xml::TValidationContext context{elements};

    t.validate(value, context);
    return !context.hasErrors();
  }
}


TEST_CASE("XSD schema validator types test")
{
  SUBCASE("Test string type")
  {
    CHECK(check<rexsapi::xml::TStringType>("Puschel"));
    CHECK(check<rexsapi::xml::TStringType>("4711 und 0815"));
  }

  SUBCASE("Test boolean type")
  {
    CHECK(check<rexsapi::xml::TBooleanType>("0"));
    CHECK(check<rexsapi::xml::TBooleanType>("1"));
    CHECK(check<rexsapi::xml::TBooleanType>("true"));
    CHECK(check<rexsapi::xml::TBooleanType>("false"));
    CHECK_FALSE(check<rexsapi::xml::TBooleanType>("puschel"));
    CHECK_FALSE(check<rexsapi::xml::TBooleanType>("t"));
    CHECK_FALSE(check<rexsapi::xml::TBooleanType>("f"));
    CHECK_FALSE(check<rexsapi::xml::TBooleanType>("TRUE"));
    CHECK_FALSE(check<rexsapi::xml::TBooleanType>("FALSE"));
    CHECK_FALSE(check<rexsapi::xml::TBooleanType>(""));
  }

  SUBCASE("Test integer type")
  {
    CHECK(check<rexsapi::xml::TIntegerType>("0"));
    CHECK(check<rexsapi::xml::TIntegerType>("4711"));
    CHECK(check<rexsapi::xml::TIntegerType>("-4711"));
    CHECK(check<rexsapi::xml::TIntegerType>(std::to_string(std::numeric_limits<int64_t>::max())));
    CHECK(check<rexsapi::xml::TIntegerType>(std::to_string(std::numeric_limits<int64_t>::min())));
    CHECK_FALSE(check<rexsapi::xml::TIntegerType>(std::to_string(std::numeric_limits<int64_t>::max()) + "1"));
    CHECK_FALSE(check<rexsapi::xml::TIntegerType>(""));
    CHECK_FALSE(check<rexsapi::xml::TIntegerType>("47LL"));
    CHECK_FALSE(check<rexsapi::xml::TIntegerType>("test"));
    CHECK_FALSE(check<rexsapi::xml::TIntegerType>("47.11"));
  }

  SUBCASE("Test non negative integer type")
  {
    CHECK(check<rexsapi::xml::TNonNegativeIntegerType>("0"));
    CHECK(check<rexsapi::xml::TNonNegativeIntegerType>("4711"));
    CHECK(check<rexsapi::xml::TNonNegativeIntegerType>(std::to_string(std::numeric_limits<uint64_t>::max())));
    CHECK(check<rexsapi::xml::TNonNegativeIntegerType>(std::to_string(std::numeric_limits<uint64_t>::min())));
    CHECK_FALSE(
      check<rexsapi::xml::TNonNegativeIntegerType>(std::to_string(std::numeric_limits<uint64_t>::max()) + "1"));
    CHECK_FALSE(check<rexsapi::xml::TNonNegativeIntegerType>(""));
    CHECK_FALSE(check<rexsapi::xml::TNonNegativeIntegerType>("-4711"));
    CHECK_FALSE(check<rexsapi::xml::TNonNegativeIntegerType>("47LL"));
    CHECK_FALSE(check<rexsapi::xml::TNonNegativeIntegerType>("test"));
    CHECK_FALSE(check<rexsapi::xml::TNonNegativeIntegerType>("47.11"));
  }

  SUBCASE("Test decimal type")
  {
    CHECK(check<rexsapi::xml::TDecimalType>("0"));
    CHECK(check<rexsapi::xml::TDecimalType>("4590845908"));
    CHECK(check<rexsapi::xml::TDecimalType>("-4590845908"));
    CHECK(check<rexsapi::xml::TDecimalType>("47.11"));
    CHECK(check<rexsapi::xml::TDecimalType>("-47.11"));
    CHECK(check<rexsapi::xml::TDecimalType>(std::to_string(std::numeric_limits<double>::max())));
    CHECK(check<rexsapi::xml::TDecimalType>(std::to_string(std::numeric_limits<double>::min())));
    CHECK_FALSE(check<rexsapi::xml::TDecimalType>(""));
    CHECK_FALSE(check<rexsapi::xml::TDecimalType>("47LL"));
    CHECK_FALSE(check<rexsapi::xml::TDecimalType>("test"));
  }
}

TEST_CASE("XSD schema validator simple type test")
{
  std::vector<std::string> enumValues{"one", "two", "three"};
  rexsapi::xml::TEnumeration enumeration{"numbers", enumValues};

  SUBCASE("Test enum valid")
  {
    CHECK(check(enumeration, "one"));
    CHECK(check(enumeration, "two"));
    CHECK(check(enumeration, "three"));
  }

  SUBCASE("Test enum not valid")
  {
    CHECK_FALSE(check(enumeration, "nine"));
    CHECK_FALSE(check(enumeration, ""));
  }
}
