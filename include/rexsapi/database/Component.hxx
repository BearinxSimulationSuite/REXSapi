
#ifndef REXSAPI_DATABASE_COMPONENT_HXX
#define REXSAPI_DATABASE_COMPONENT_HXX

#include <rexsapi/database/Attribute.hxx>

namespace rexsapi::database
{
  class TComponent
  {
  public:
    TComponent(std::string id, std::string name, std::vector<std::reference_wrapper<const TAttribute>>&& attributes)
    : m_Id{std::move(id)}
    , m_Name{std::move(name)}
    , m_Attributes{std::move(attributes)}
    {
    }

    ~TComponent() = default;

    TComponent(const TComponent&) = delete;
    TComponent(TComponent&&) = default;
    TComponent& operator=(const TComponent&) = delete;
    TComponent& operator=(TComponent&&) = default;

    [[nodiscard]] const std::string& getId() const
    {
      return m_Id;
    }

    [[nodiscard]] const std::string& getName() const
    {
      return m_Name;
    }

    [[nodiscard]] std::vector<std::reference_wrapper<const TAttribute>> getAttributes() const
    {
      return m_Attributes;
    }

    [[nodiscard]] const TAttribute& findAttributeById(const std::string& id) const
    {
      auto it = std::find_if(m_Attributes.begin(), m_Attributes.end(), [&id](const TAttribute& attribute) {
        return attribute.getAttributeId() == id;
      });
      if (it == m_Attributes.end()) {
        throw TException{fmt::format("component '{}' does not contain attribute '{}'", m_Id, id)};
      }

      return *it;
    }

  private:
    std::string m_Id;
    std::string m_Name;
    std::vector<std::reference_wrapper<const TAttribute>> m_Attributes;
  };
}

#endif
