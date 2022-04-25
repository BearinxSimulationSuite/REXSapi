
#include <rexsapi/database/FileResourceLoader.hxx>
#include <rexsapi/database/ModelRegistry.hxx>
#include <rexsapi/database/XMLModelLoader.hxx>

#include <test/TestHelper.hxx>

#include <doctest.h>

TEST_CASE("Test rexs model registry")
{
  rexsapi::database::TFileResourceLoader resourceLoader{projectDir() / "models"};
  rexsapi::database::TXmlModelLoader modelLoader{resourceLoader};
  auto createResult = rexsapi::database::TModelRegistry::createModelRegistry(modelLoader);
  const auto& registry = createResult.first;
  REQUIRE(createResult.second);

  SUBCASE("Get existing models")
  {
    const auto& model = registry.getModel("1.4", "de");
    CHECK(model.getVersion() == "1.4");
    CHECK(model.getLanguage() == "de");

    const auto& component = model.findComponentById("side_plate");
    CHECK(component.getId() == "side_plate");
    CHECK(component.getName() == "Wange");
    CHECK(component.getAttributes().size() == 9);
  }

  SUBCASE("Get non existing models")
  {
    CHECK_THROWS_WITH((void)registry.getModel("1.4", "es"), "cannot find a model for version '1.4' and locale 'es'");
    CHECK_THROWS_WITH((void)registry.getModel("1.99", "en"), "cannot find a model for version '1.99' and locale 'en'");
  }
}
