
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
              <xsd:element ref="Suites" maxOccurs="1" minOccurs="0"/>
              <xsd:element ref="Tests" maxOccurs="1" minOccurs="0"/>
            </xsd:sequence>
            <xsd:attribute name="version" type="xsd:string" use="required"/>
            <xsd:attribute name="date" type="xsd:string" use="optional"/>
          </xsd:complexType>
        </xsd:element>
        <xsd:element name="Suites">
          <xsd:complexType>
            <xsd:sequence>
              <xsd:element ref="Suite" maxOccurs="unbounded" minOccurs="0"/>
            </xsd:sequence>
          </xsd:complexType>
        </xsd:element>
        <xsd:element name="Suite">
          <xsd:complexType>
            <xsd:sequence>
              <xsd:element ref="Tests" maxOccurs="1" minOccurs="0"/>
            </xsd:sequence>
            <xsd:attribute name="name" type="xsd:string" use="required"/>
          </xsd:complexType>
        </xsd:element>        
        <xsd:element name="Tests">
          <xsd:complexType>
            <xsd:sequence>
              <xsd:element ref="Test" maxOccurs="unbounded" minOccurs="0"/>
            </xsd:sequence>
          </xsd:complexType>
        </xsd:element>        
        <xsd:element name="Test">
          <xsd:complexType>
            <xsd:attribute name="name" type="xsd:string" use="required"/>
          </xsd:complexType>
        </xsd:element>        
      </xsd:schema>
    )";
}

TEST_CASE("XSD schema validator test")
{
  SUBCASE("Validate schema from file")
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
      <TestCases version="42" date="2022-04-28T11:10">
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
            <Tests>
              <Test name="1" />
              <Test name="2" />
              <Test name="3" />
            </Tests>
        </Tests>
      </TestCases>
    )";

    pugi::xml_document doc;
    pugi::xml_parse_result parseResult = doc.load_string(xml);
    CHECK(parseResult);

    rexsapi::xml::TBufferXsdSchemaLoader loader{schema};
    rexsapi::xml::TXSDSchemaValidator val{loader};

    std::vector<std::string> errors;
    CHECK(val.validate(doc, errors));
    CHECK(errors.empty());
  }
}
