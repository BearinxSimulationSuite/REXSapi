
#include <rexsapi/database/FileResourceLoader.hxx>
#include <rexsapi/database/XMLModelLoader.hxx>

#include <test/TestHelper.hxx>


static inline std::vector<rexsapi::database::TModel> loadModels()
{
  std::vector<rexsapi::database::TModel> models;
  rexsapi::database::TFileResourceLoader loader{projectDir() / "models"};
  rexsapi::database::TXmlModelLoader modelLoader{loader};
  auto result = modelLoader.load([&models](rexsapi::database::TModel model) {
    models.emplace_back(std::move(model));
  });

  return models;
}
