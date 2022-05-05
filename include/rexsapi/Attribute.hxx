
#ifndef REXSAPI_ATTRIBUTE_HXX
#define REXSAPI_ATTRIBUTE_HXX

#include <rexsapi/Value.hxx>
#include <rexsapi/database/Attribute.hxx>

namespace rexsapi
{
  class TAttribute
  {
  public:
    TAttribute(const database::TAttribute& attribute, TValue value)
    : m_Attribute{attribute}
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

    const TValue& getValue() const
    {
      return m_Value;
    }

  private:
    const database::TAttribute& m_Attribute;
    TValue m_Value;
  };

  using TAttributes = std::vector<TAttribute>;
}

#endif
