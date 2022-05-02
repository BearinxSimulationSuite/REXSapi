
#include <rexsapi/database/FileResourceLoader.hxx>
#include <rexsapi/database/XMLModelLoader.hxx>

#include <test/TestHelper.hxx>


static inline std::vector<rexsapi::database::TModel> loadModels()
{
  std::vector<rexsapi::database::TModel> models;
  rexsapi::xml::TFileXsdSchemaLoader schemaLoader{projectDir() / "models" / "rexs-dbmodel.xsd"};
  rexsapi::database::TFileResourceLoader loader{projectDir() / "models"};
  rexsapi::database::TXmlModelLoader modelLoader{loader, schemaLoader};
  auto result = modelLoader.load([&models](rexsapi::database::TModel model) {
    models.emplace_back(std::move(model));
  });

  return models;
}

static inline rexsapi::database::TModel loadModel(const std::string& version)
{
  auto models = loadModels();
  auto it = std::find_if(models.begin(), models.end(), [&version](const auto& model) {
    return version == model.getVersion();
  });

  if (it == models.end()) {
    throw rexsapi::TException{"no model with version '" + version + "' found"};
  }
  return std::move(*it);
}
