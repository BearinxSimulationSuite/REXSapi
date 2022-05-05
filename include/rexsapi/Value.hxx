
#ifndef REXSAPI_VALUE_HXX
#define REXSAPI_VALUE_HXX

#include <string>

namespace rexsapi
{
  class TValue
  {
  public:
    explicit TValue(std::string value)
    : m_Value{std::move(value)}
    {
    }

    const std::string& getValue() const
    {
      return m_Value;
    }

  private:
    std::string m_Value;
  };
}

#endif
