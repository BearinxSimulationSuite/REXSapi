
#ifndef REXSCXX_DATABASE_ATTRIBUTE_HXX
#define REXSCXX_DATABASE_ATTRIBUTE_HXX

#include <rexsapi/database/EnumValues.hxx>
#include <rexsapi/database/Interval.hxx>
#include <rexsapi/database/Unit.hxx>
#include <rexsapi/database/ValueType.hxx>

#include <optional>

namespace rexsapi::database
{
  class TAttribute
  {
  public:
    TAttribute(uint64_t id, std::string attributeId, std::string name, TValueType type, const Unit& unit,
               std::optional<std::string> symbol, const std::optional<TInterval>& interval, std::optional<TEnumValues> enumValues)
    : m_Id{id}
    , m_AttributeId{std::move(attributeId)}
    , m_Name{std::move(name)}
    , m_Type{type}
    , m_Unit{unit}
    , m_Symbol{std::move(symbol)}
    , m_Interval{interval}
    , m_EnumValues{std::move(enumValues)}
    {
    }

    [[nodiscard]] const std::string& getAttributeId() const
    {
      return m_AttributeId;
    }

  private:
    uint64_t m_Id;
    std::string m_AttributeId;
    std::string m_Name;
    TValueType m_Type;
    const Unit& m_Unit;
    std::optional<std::string> m_Symbol;
    std::optional<TInterval> m_Interval;
    std::optional<TEnumValues> m_EnumValues;
  };
}

#endif
