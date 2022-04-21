
#include <rexsapi/database/FileResourceLoader.hxx>
#include <rexsapi/database/XMLModelLoader.hxx>

#include <test/TestHelper.hxx>

#include <doctest.h>
#include <set>

TEST_CASE("XML model loader test")
{
  rexsapi::database::TFileResourceLoader loader{projectDir() / "models"};

  SUBCASE("Load existing models")
  {
    std::vector<rexsapi::database::TModel> models;
    rexsapi::database::TXmlModelLoader modelLoader{loader};
    auto result = modelLoader.load([&models](rexsapi::database::TModel model) {
      models.emplace_back(std::move(model));
    });

    CHECK(result);
    CHECK(models.size() == 10);
    std::set<std::string, std::less<>> languages;
    for (const auto& model : models) {
      languages.insert(model.getLanguage());
    }
    CHECK(languages.size() == 2);
    CHECK(languages.find("de") != languages.end());
    CHECK(languages.find("en") != languages.end());
  }
}
