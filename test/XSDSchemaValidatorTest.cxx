
#include <rexsapi/xml/XSDSchemaValidator.hxx>

#include <test/TestHelper.hxx>

#include <doctest.h>

TEST_CASE("XSD schema validator test")
{
  SUBCASE("Load schema")
  {
    pugi::xml_document doc;
    if (pugi::xml_parse_result parseResult = doc.load_file((projectDir() / "models" / "rexs_model_1.4_en.xml").string().c_str());
        !parseResult) {
      throw rexsapi::Exception{
        fmt::format("cannot open xml file '{}'", (projectDir() / "models" / "rexs_model_1.4_en.xml").string())};
    }

    rexsapi::xml::TXSDSchemaValidator val{projectDir() / "models" / "rexs-dbmodel.xsd"};
    val.dump(std::cout);

    std::vector<std::string> errors;
    if (!val.validate(doc, errors)) {
      std::cout << "invalid document" << std::endl;
      for (const auto& error : errors) {
        std::cout << error << std::endl;
      }
    } else {
      std::cout << "document is valid!" << std::endl;
    }
  }
}
