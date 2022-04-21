
#ifndef REXSCXX_DATABASE_COMPONENT_ATTRIBUTE_MAPPER_HXX
#define REXSCXX_DATABASE_COMPONENT_ATTRIBUTE_MAPPER_HXX

#include <rexsapi/database/Model.hxx>

namespace rexsapi::database
{
  class TComponentAttributeMapper
  {
  public:
    explicit TComponentAttributeMapper(const TModel& model, std::vector<std::pair<std::string, std::string>>&& attributeMappings)
    : m_Model{model}
    , m_AttributeMappings{std::move(attributeMappings)}
    {
    }

    [[nodiscard]] std::vector<std::reference_wrapper<const TAttribute>> getAttributesForComponent(const std::string& id) const
    {
      std::vector<std::reference_wrapper<const TAttribute>> attributes;

      std::for_each(m_AttributeMappings.begin(), m_AttributeMappings.end(), [this, &id, &attributes](const auto& element) {
        if (id == element.first) {
          attributes.emplace_back(m_Model.findAttributetById(element.second));
        }
      });

      return attributes;
    }

  private:
    const TModel& m_Model;
    std::vector<std::pair<std::string, std::string>> m_AttributeMappings;
  };

}


#endif
