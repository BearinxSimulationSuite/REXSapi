
#include <rexsapi/xml/XSDSchemaValidator.hxx>

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
            </xsd:sequence>
          </xsd:complexType>
        </xsd:element>        
        <xsd:element name="Test">
          <xsd:complexType>
            <xsd:attribute name="name" type="xsd:string" use="required"/>
          </xsd:complexType>
        </xsd:element>
        <xsd:simpleType name="Status">
          <xsd:restriction base="xsd:string">
            <xsd:enumeration value="COOL"/>
            <xsd:enumeration value="OH NO"/>
          </xsd:restriction>
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

TEST_CASE("XSD schema validator test")
{
  SUBCASE("Validate db model schema from file")
  {
    pugi::xml_document doc;
    pugi::xml_parse_result parseResult = doc.load_file((projectDir() / "models" / "rexs_model_1.4_en.xml").string().c_str());
    CHECK(parseResult);

    rexsapi::xml::TFileXsdSchemaLoader loader{projectDir() / "models" / "rexs-dbmodel.xsd"};
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
          <Test name="1" />
          <Test name="2" />
          <Test name="3" />
        </Tests>
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
            <Püschel />
          </Suite>
        </Suites>
      </TestCases>
    )";

    std::vector<std::string> errors;
    CHECK_FALSE(validate(xml, errors));
    REQUIRE(errors.size() == 2);
    CHECK(errors[0] == "[/TestCases/Suites/] element 'Tests' is not allowed here");
    CHECK(errors[1] == "[/TestCases/Suites/Suite/] unkown element 'Püschel'");
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
      <TestCases version="püschel">
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
    CHECK(errors[0] == "[/TestCases/version/] cannot convert 'püschel' to integer");
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
}
