
#ifndef REXSCXX_DATABASE_ENUM_VALUES
#define REXSCXX_DATABASE_ENUM_VALUES

#include <algorithm>
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

    [[nodiscard]] bool check(const std::string& value) const
    {
      auto it = std::find_if(m_Values.begin(), m_Values.end(), [&value](const auto& entry) {
        return entry.m_Value == value;
      });

      return it != m_Values.end();
    }

  private:
    std::vector<TEnumValue> m_Values;
  };
}

#endif
