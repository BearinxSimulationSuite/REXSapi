
#ifndef REXSCXX_DATABASE_MODEL_REGISTRY_HXX
#define REXSCXX_DATABASE_MODEL_REGISTRY_HXX

#include <rexsapi/database/Model.hxx>

namespace rexsapi::database
{
  class TModelRegistry
  {
  public:
    explicit TModelRegistry(std::vector<TModel>&& models)
    : m_Models{std::move(models)}
    {
    }

    [[nodiscard]] const TModel& getModel(const std::string& version, const std::string& language) const
    {
      auto it = std::find_if(m_Models.begin(), m_Models.end(), [&version, &language](const auto& model) {
        return model.getVersion() == version && model.getLanguage() == language;
      });

      if (it == m_Models.end()) {
        throw Exception{"cannot find a model for version '" + version + "' and locale '" + language + "'"};
      }

      return *it;
    }

  private:
    const std::vector<TModel> m_Models;
  };

  template<typename ResourceLoader, typename ModelLoader>
  static inline TModelRegistry createModelRegistry(const std::filesystem::path& directory)
  {
    ResourceLoader loader{directory};
    std::vector<rexsapi::database::TModel> models;
    ModelLoader modelLoader{loader};
    auto result = modelLoader.load([&models](rexsapi::database::TModel model) {
      models.emplace_back(std::move(model));
    });

    return rexsapi::database::TModelRegistry{std::move(models)};
  }
}

#endif
