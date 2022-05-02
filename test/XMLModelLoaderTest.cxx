
#include <rexsapi/database/FileResourceLoader.hxx>
#include <rexsapi/database/XMLModelLoader.hxx>

#include <test/TestHelper.hxx>

#include <doctest.h>
#include <set>


namespace
{
  class StringResourceLoader
  {
  public:
    explicit StringResourceLoader(std::string buffer)
    : m_Buffer{std::move(buffer)}
    {
    }

    rexsapi::database::TLoaderResult
    load(const std::function<void(rexsapi::database::TLoaderResult&, std::vector<uint8_t>&)>& callback) const
    {
      if (!callback) {
        throw rexsapi::TException{"callback not set for resource loader"};
      }

      rexsapi::database::TLoaderResult result;
      std::vector<uint8_t> buf{m_Buffer.begin(), m_Buffer.end()};
      callback(result, buf);

      return result;
    }

  private:
    std::string m_Buffer;
  };
}

TEST_CASE("XML model loader test")
{
  rexsapi::xml::TFileXsdSchemaLoader schemaLoader{projectDir() / "models" / "rexs-dbmodel.xsd"};
  std::vector<rexsapi::database::TModel> models;

  SUBCASE("Load existing models")
  {
    rexsapi::database::TFileResourceLoader loader{projectDir() / "models"};

    rexsapi::database::TXmlModelLoader modelLoader{loader, schemaLoader};
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

  SUBCASE("Load broken XML")
  {
    const auto* s =
      R"(<rexsModel version=" 1.4 " status=" RELEASED " language=" de " createdDate="2022-04-19T13:03:21.881+02:00"><units></rexsModel>)";
    StringResourceLoader loader{s};
    rexsapi::database::TXmlModelLoader modelLoader{loader, schemaLoader};
    auto result = modelLoader.load([&models](rexsapi::database::TModel model) {
      models.emplace_back(std::move(model));
    });
    CHECK_FALSE(result);
    CHECK(models.empty());
  }
}
