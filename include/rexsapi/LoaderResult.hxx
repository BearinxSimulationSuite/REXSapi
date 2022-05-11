
#ifndef REXSAPI_RESOURCE_LOADER_HXX
#define REXSAPI_RESOURCE_LOADER_HXX

#include <rexsapi/Defines.hxx>

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace rexsapi
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

    const std::vector<TResourceError>& getErrors() const
    {
      return m_Errors;
    }

  private:
    std::vector<TResourceError> m_Errors;
  };
}

#endif
