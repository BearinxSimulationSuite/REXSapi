
#ifndef REXSCXX_DATABASE_ATTRIBUTE_HXX
#define REXSCXX_DATABASE_ATTRIBUTE_HXX

#include <rexsapi/Format.h>
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
    TAttribute(std::string attributeId, std::string name, TValueType type, const Unit& unit, std::string symbol,
               std::optional<const TInterval> interval, std::optional<const TEnumValues> enumValues)
    : m_AttributeId{std::move(attributeId)}
    , m_Name{std::move(name)}
    , m_Type{type}
    , m_Unit{unit}
    , m_Symbol{std::move(symbol)}
    , m_Interval{std::move(interval)}
    , m_EnumValues{std::move(enumValues)}
    {
      if (m_Type == TValueType::ENUM && !m_EnumValues) {
        throw Exception{fmt::format("enum with id '{}' does not have any enum values", m_AttributeId)};
      }
    }

    ~TAttribute() = default;

    TAttribute(const TAttribute&) = delete;
    TAttribute(TAttribute&&) = default;
    TAttribute& operator=(const TAttribute&) = delete;
    TAttribute& operator=(TAttribute&&) = delete;

    [[nodiscard]] const std::string& getAttributeId() const
    {
      return m_AttributeId;
    }

    [[nodiscard]] const std::string& getName() const
    {
      return m_Name;
    }

    [[nodiscard]] TValueType getValueType() const
    {
      return m_Type;
    }

    [[nodiscard]] const Unit& getUnit() const
    {
      return m_Unit;
    }

    [[nodiscard]] const std::string& getSymbol() const
    {
      return m_Symbol;
    }

    [[nodiscard]] const std::optional<const TInterval>& getInterval() const
    {
      return m_Interval;
    }

    [[nodiscard]] const std::optional<const TEnumValues>& getEnums() const
    {
      return m_EnumValues;
    }

  private:
    const std::string m_AttributeId;
    const std::string m_Name;
    const TValueType m_Type;
    const Unit& m_Unit;
    const std::string m_Symbol;
    const std::optional<const TInterval> m_Interval;
    const std::optional<const TEnumValues> m_EnumValues;
  };
}

#endif
