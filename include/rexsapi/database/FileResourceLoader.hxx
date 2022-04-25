
#ifndef REXSCXX_DATABASE_FILE_RESOURCE_LOADER_HXX
#define REXSCXX_DATABASE_FILE_RESOURCE_LOADER_HXX

#include <rexsapi/Exception.hxx>
#include <rexsapi/database/ResourceLoader.hxx>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace rexsapi::database
{
  class TFileResourceLoader : public TResourceLoader
  {
  public:
    explicit TFileResourceLoader(std::filesystem::path path)
    : m_Path{std::move(path)}
    {
    }

  private:
    [[nodiscard]] std::vector<std::string> doFindResources() const override
    {
      if (!std::filesystem::exists(m_Path) || !std::filesystem::is_directory(m_Path)) {
        throw Exception{"Resource '" + m_Path.string() + "' does not exist or is not a directory"};
      }

      std::vector<std::string> resources;
      for (const auto& p : std::filesystem::directory_iterator(m_Path)) {
        // TODO (lcf): check file name structure with regex
        if (p.path().extension() == ".xml") {
          resources.emplace_back(p.path().string());
        }
      }

      return resources;
    }

    [[nodiscard]] std::vector<uint8_t> doLoad(TLoaderResult& result, std::string_view resource) const override
    {
      auto path = m_Path / resource;
      if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) {
        result.addError(TResourceError{"Resource '" + path.string() + "' does not exist or is not a file"});
      }

      std::ifstream file{path};
      if (!file.good()) {
        throw Exception{"Resource '" + path.string() + "' cannot be loaded"};
      }
      std::stringstream ss;
      ss << file.rdbuf();
      auto buffer = ss.str();

      return std::vector<uint8_t>{buffer.begin(), buffer.end()};
    }

    const std::filesystem::path m_Path;
  };
}

#endif
