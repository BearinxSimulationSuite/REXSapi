
#ifndef REXSAPI_RELATION_HXX
#define REXSAPI_RELATION_HXX

#include <rexsapi/Component.hxx>
#include <rexsapi/Types.hxx>

namespace rexsapi
{
  class TRelationReference
  {
  public:
    TRelationReference(TRelationRole role, const TComponent& component)
    : m_Role{role}
    , m_Component{component}
    {
    }

    [[nodiscard]] const std::string& getHint() const
    {
      return m_Component.getId();
    }

    [[nodiscard]] TRelationRole getRole() const
    {
      return m_Role;
    }

    [[nodiscard]] const TComponent& getComponent() const
    {
      return m_Component;
    }

  private:
    TRelationRole m_Role;
    const TComponent& m_Component;
  };

  using TRelationReferences = std::vector<TRelationReference>;


  class TRelation
  {
  public:
    TRelation(TRelationType type, std::optional<uint32_t> order, TRelationReferences&& references)
    : m_Type{type}
    , m_Order{std::move(order)}
    , m_References{std::move(references)}
    {
    }

    [[nodiscard]] TRelationType getType() const
    {
      return m_Type;
    }

    [[nodiscard]] std::optional<uint32_t> getOrder() const
    {
      return m_Order;
    }

    [[nodiscard]] const TRelationReferences& getReferences() const
    {
      return m_References;
    }

  private:
    TRelationType m_Type;
    std::optional<uint32_t> m_Order;
    TRelationReferences m_References;
  };

  using TRelations = std::vector<TRelation>;
}

#endif
