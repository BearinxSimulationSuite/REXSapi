
#include <rexsapi/database/FileResourceLoader.hxx>
#include <rexsapi/database/ModelRegistry.hxx>
#include <rexsapi/database/XMLModelLoader.hxx>

#include <test/TestHelper.hxx>

#include <doctest.h>

TEST_CASE("Test rexs model registry")
{
  SUBCASE("Get existing models")
  {
    rexsapi::database::TModelRegistry registry =
      rexsapi::database::createModelRegistry<rexsapi::database::TFileResourceLoader, rexsapi::database::TXmlModelLoader>(
        projectDir() / "models");
    const auto& model = registry.getModel("1.4", "de");
    CHECK(model.getVersion() == "1.4");
    CHECK(model.getLanguage() == "de");
  }
}
