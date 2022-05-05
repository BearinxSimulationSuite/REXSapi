
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
    , m_Unit{unit}
    , m_Value{std::move(value)}
    {
    }

    const std::string& getAttributeId() const
    {
      return m_Attribute.getAttributeId();
    }

    const std::string& getName() const
    {
      return m_Attribute.getName();
    }

    const TUnit& getUnit() const
    {
      return m_Unit;
    }

    const TValue& getValue() const
    {
      return m_Value;
    }

  private:
    const database::TAttribute& m_Attribute;
    TUnit m_Unit;
    TValue m_Value;
  };

  using TAttributes = std::vector<TAttribute>;
}

#endif
