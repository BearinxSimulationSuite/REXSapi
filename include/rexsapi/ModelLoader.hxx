
#ifndef REXSAPI_MODEL_LOADER_FACTORY_HXX
#define REXSAPI_MODEL_LOADER_FACTORY_HXX

#include <rexsapi/LoaderResult.hxx>
#include <rexsapi/Model.hxx>
#include <rexsapi/database/ModelRegistry.hxx>

#include <filesystem>

namespace rexsapi
{

  template<typename T>
  class TFileLoader
  {
  public:
    explicit TFileLoader(std::filesystem::path path)
    : m_Path{std::move(path)}
    {
    }

    [[nodiscard]] std::optional<TModel> load(TLoaderResult& result, const rexsapi::database::TModelRegistry& registry)
    {
      // TODO (lcf): check path, extension, etc.
      // TODO (lcf): load file contents into buffer
      // TODO (lcf): use T to load buffer as model

      T loader;
      return loader.load(result, registry);
    }

  private:
    std::filesystem::path m_Path;
  };


  template<typename T>
  class TBufferLoader
  {
  public:
    explicit TBufferLoader(const std::string& buffer)
    : m_Buffer{buffer.begin(), buffer.end()}
    {
    }

    explicit TBufferLoader(std::vector<uint8_t> buffer)
    : m_Buffer{std::move(buffer)}
    {
    }

    [[nodiscard]] std::optional<TModel> load(TLoaderResult& result, const rexsapi::database::TModelRegistry& registry)
    {
      T loader;
      return loader.load(result, registry, m_Buffer);
    }

  private:
    std::vector<uint8_t> m_Buffer;
  };
}

#endif
