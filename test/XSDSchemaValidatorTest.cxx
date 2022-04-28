
#include <rexsapi/xml/XSDSchemaValidator.hxx>

#include <test/TestHelper.hxx>

#include <doctest.h>

TEST_CASE("XSD schema validator test")
{
  SUBCASE("Load schema")
  {
    pugi::xml_document doc;
    pugi::xml_parse_result parseResult = doc.load_file((projectDir() / "models" / "rexs_model_1.4_en.xml").string().c_str());
    CHECK(parseResult);

    rexsapi::xml::TXSDSchemaValidator val{projectDir() / "models" / "rexs-dbmodel.xsd"};

    std::vector<std::string> errors;
    CHECK(val.validate(doc, errors));
    CHECK(errors.empty());
  }
}
