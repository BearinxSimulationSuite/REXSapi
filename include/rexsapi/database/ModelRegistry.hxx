
#ifndef REXSAPI_DATABASE_MODEL_REGISTRY_HXX
#define REXSAPI_DATABASE_MODEL_REGISTRY_HXX

#include <rexsapi/database/Model.hxx>

namespace rexsapi::database
{
  class TModelRegistry
  {
  public:
    ~TModelRegistry() = default;

    TModelRegistry(const TModelRegistry&) = delete;
    TModelRegistry(TModelRegistry&&) noexcept = default;
    TModelRegistry& operator=(const TModelRegistry&) = delete;
    TModelRegistry& operator=(TModelRegistry&&) = delete;

    [[nodiscard]] const TModel& getModel(const TRexsVersion& version, const std::string& language) const;

    template<typename TModelLoader>
    static std::pair<TModelRegistry, TLoaderResult> createModelRegistry(const TModelLoader& loader);

  private:
    explicit TModelRegistry(std::vector<TModel>&& models)
    : m_Models{std::move(models)}
    {
    }

    std::vector<TModel> m_Models;
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline const TModel& TModelRegistry::getModel(const TRexsVersion& version, const std::string& language) const
  {
    auto it = std::find_if(m_Models.begin(), m_Models.end(), [&version, &language](const auto& model) {
      return model.getVersion() == version && model.getLanguage() == language;
    });

    if (it == m_Models.end()) {
      throw TException{fmt::format("cannot find a model for version '{}' and locale '{}'", version.string(), language)};
    }

    return *it;
  }

  template<typename TModelLoader>
  std::pair<TModelRegistry, TLoaderResult> TModelRegistry::createModelRegistry(const TModelLoader& loader)
  {
    std::vector<TModel> models;
    auto result = loader.load([&models](TModel model) {
      models.emplace_back(std::move(model));
    });

    return std::make_pair(TModelRegistry{std::move(models)}, result);
  }
}

#endif
