
#ifndef REXSAPI_VALUE_HXX
#define REXSAPI_VALUE_HXX

#include <rexsapi/database/EnumValues.hxx>

#include <variant>

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


  using Variant = std::variant<double, bool, int64_t, std::string, std::vector<double>, std::vector<bool>,
                               std::vector<int64_t>, std::vector<std::string>, std::vector<std::vector<int64_t>>>;

}

#endif
