
#ifndef REXSCXX_DATABASE_ENUM_VALUES
#define REXSCXX_DATABASE_ENUM_VALUES

#include <string>
#include <vector>

namespace rexsapi::database
{
  struct TEnumValue {
    std::string m_Value;
    std::string m_Name;
  };

  class TEnumValues
  {
  public:
    explicit TEnumValues(std::vector<TEnumValue>&& values)
    : m_Values{std::move(values)}
    {
    }

  private:
    std::vector<TEnumValue> m_Values;
  };
}

#endif
