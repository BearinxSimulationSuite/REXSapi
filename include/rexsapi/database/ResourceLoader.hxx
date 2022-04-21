
#ifndef REXSCXX_DATABASE_RESOURCE_LOADER_HXX
#define REXSCXX_DATABASE_RESOURCE_LOADER_HXX

#include <rexsapi/Exception.hxx>

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace rexsapi::database
{
  struct TResourceError {
    explicit TResourceError(std::string message, ssize_t position = -1)
    : m_Message{std::move(message)}
    , m_Position{position}
    {
    }

    std::string m_Message;
    ssize_t m_Position;
  };

  class TLoaderResult
  {
  public:
    void addError(TResourceError error)
    {
      m_Errors.emplace_back(std::move(error));
    }

    explicit operator bool() const
    {
      return m_Errors.empty();
    }

  private:
    std::vector<TResourceError> m_Errors;
  };

  class TResourceLoader
  {
  public:
    TResourceLoader() = default;
    virtual ~TResourceLoader() = default;

    TResourceLoader(const TResourceLoader&) = delete;
    TResourceLoader(TResourceLoader&&) = default;
    TResourceLoader& operator=(const TResourceLoader&) = delete;
    TResourceLoader& operator=(TResourceLoader&&) = default;

    TLoaderResult load(const std::function<void(TLoaderResult&, std::vector<uint8_t>&)>& callback) const
    {
      if (!callback) {
        throw Exception{"callback not set for resource loader"};
      }

      TLoaderResult result;

      auto resources = doFindResources();
      std::for_each(resources.begin(), resources.end(), [this, &callback, &result](std::string_view resource) {
        auto buffer = doLoad(result, resource);
        callback(result, buffer);
      });

      return result;
    }

  private:
    [[nodiscard]] virtual std::vector<std::string> doFindResources() const = 0;
    [[nodiscard]] virtual std::vector<uint8_t> doLoad(TLoaderResult& result, std::string_view resource) const = 0;
  };
}

#endif
