
#ifndef REXSCXX_DATABASE_FILE_RESOURCE_LOADER_HXX
#define REXSCXX_DATABASE_FILE_RESOURCE_LOADER_HXX

#include <rexsapi/Exception.hxx>
#include <rexsapi/Format.hxx>
#include <rexsapi/database/LoaderResult.hxx>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace rexsapi::database
{
  class TFileResourceLoader
  {
  public:
    explicit TFileResourceLoader(std::filesystem::path path)
    : m_Path{std::move(path)}
    {
    }

    TLoaderResult load(const std::function<void(TLoaderResult&, std::vector<uint8_t>&)>& callback) const
    {
      if (!callback) {
        throw Exception{"callback not set for resource loader"};
      }

      TLoaderResult result;

      auto resources = findResources(result);
      std::for_each(resources.begin(), resources.end(), [this, &callback, &result](const auto& resource) {
        auto buffer = load(result, resource);
        if (buffer.size()) {
          callback(result, buffer);
        }
      });

      return result;
    }

  private:
    [[nodiscard]] std::vector<std::filesystem::path> findResources(TLoaderResult& result) const
    {
      if (!std::filesystem::exists(m_Path) || !std::filesystem::is_directory(m_Path)) {
        throw Exception{fmt::format("Directory '{}' does not exist or is not a directory", m_Path.string())};
      }

      std::vector<std::filesystem::path> resources;
      for (const auto& p : std::filesystem::directory_iterator(m_Path)) {
        // TODO (lcf): check file name structure with regex
        if (p.path().extension() == ".xml") {
          if (!std::filesystem::is_regular_file(p.path())) {
            result.addError(TResourceError{fmt::format("Resource '{}' is not a file", p.path().string())});
            continue;
          }

          resources.emplace_back(p.path());
        }
      }
      if (resources.empty()) {
        result.addError(TResourceError{"No model database files found"});
      }

      return resources;
    }

    [[nodiscard]] std::vector<uint8_t> load(TLoaderResult& result, const std::filesystem::path& resource) const
    {
      std::ifstream file{resource};
      if (!file.good()) {
        result.addError(TResourceError{fmt::format("Resource '{}' cannot be loaded", resource.string())});
        return std::vector<uint8_t>{};
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
