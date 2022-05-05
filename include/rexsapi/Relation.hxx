
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

    const std::string& getHint() const
    {
      return m_Component.getId();
    }

    TRelationRole getRole() const
    {
      return m_Role;
    }

    const TComponent& getComponent() const
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
    TRelation(TRelationType type, TRelationReferences&& references)
    : m_Type{type}
    , m_References{std::move(references)}
    {
    }

    TRelationType getType() const
    {
      return m_Type;
    }

    const TRelationReferences& getReferences() const
    {
      return m_References;
    }

  private:
    TRelationType m_Type;
    TRelationReferences m_References;
  };

  using TRelations = std::vector<TRelation>;
}

#endif
