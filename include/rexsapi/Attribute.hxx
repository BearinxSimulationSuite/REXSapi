
#ifndef REXSAPI_ATTRIBUTE_HXX
#define REXSAPI_ATTRIBUTE_HXX

#include <rexsapi/Unit.hxx>
#include <rexsapi/Value.hxx>
#include <rexsapi/database/Attribute.hxx>

namespace rexsapi
{
  class TAttribute
  {
  public:
    TAttribute(const database::TAttribute& attribute, TUnit unit, TValue value)
    : m_Attribute{attribute}
    , m_Unit{std::move(unit)}
    , m_Value{std::move(value)}
    {
    }

    [[nodiscard]] const std::string& getAttributeId() const
    {
      return m_Attribute.getAttributeId();
    }

    [[nodiscard]] const std::string& getName() const
    {
      return m_Attribute.getName();
    }

    [[nodiscard]] const TUnit& getUnit() const
    {
      return m_Unit;
    }

    [[nodiscard]] database::TValueType getValueType() const
    {
      return m_Attribute.getValueType();
    }

    [[nodiscard]] bool hasValue() const
    {
      return !m_Value.isEmpty();
    }

    template<typename T>
    [[nodiscard]] const T& getValue() const
    {
      return m_Value.getValue<T>();
    }

  private:
    const database::TAttribute& m_Attribute;
    TUnit m_Unit;
    TValue m_Value;
  };

  using TAttributes = std::vector<TAttribute>;
}

#endif
