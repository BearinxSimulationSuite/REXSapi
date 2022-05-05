
#ifndef REXSAPI_COMPONENT_HXX
#define REXSAPI_COMPONENT_HXX

#include <rexsapi/Attribute.hxx>

namespace rexsapi
{
  class TComponent
  {
  public:
    TComponent(std::string id, std::string name, TAttributes&& attributes)
    : m_Id{std::move(id)}
    , m_Name{std::move(name)}
    , m_Attributes{std::move(attributes)}
    {
    }

    const std::string& getId() const
    {
      return m_Id;
    }

    const std::string& getName() const
    {
      return m_Name;
    }

    const TAttributes& getAttributes() const
    {
      return m_Attributes;
    }

  private:
    std::string m_Id;
    std::string m_Name;
    TAttributes m_Attributes;
  };

  using TComponents = std::vector<TComponent>;
}

#endif
