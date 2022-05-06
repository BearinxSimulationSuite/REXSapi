
#include <rexsapi/Rexsapi.hxx>

#include <test/TestHelper.hxx>

#include <doctest.h>

TEST_CASE("Test rexs model registry")
{
  rexsapi::xml::TFileXsdSchemaLoader schemaLoader{projectDir() / "models" / "rexs-dbmodel.xsd"};
  rexsapi::database::TFileResourceLoader resourceLoader{projectDir() / "models"};
  rexsapi::database::TXmlModelLoader modelLoader{resourceLoader, schemaLoader};
  auto result = rexsapi::database::TModelRegistry::createModelRegistry(modelLoader);
  const auto& registry = result.first;
  REQUIRE(result.second);

  SUBCASE("Get existing models")
  {
    const auto& model = registry.getModel("1.4", "de");
    CHECK(model.getVersion() == "1.4");
    CHECK(model.getLanguage() == "de");

    const auto& component = model.findComponentById("side_plate");
    CHECK(component.getId() == "side_plate");
    CHECK(component.getName() == "Wange");
    CHECK(component.getAttributes().size() == 9);

    CHECK(model.findUnitByName("mm").getId() == 2);
    CHECK(model.findUnitById(2).getName() == "mm");
  }

  SUBCASE("Get non existing models")
  {
    CHECK_THROWS_WITH((void)registry.getModel("1.4", "es"), "cannot find a model for version '1.4' and locale 'es'");
    CHECK_THROWS_WITH((void)registry.getModel("1.99", "en"), "cannot find a model for version '1.99' and locale 'en'");
  }
}
