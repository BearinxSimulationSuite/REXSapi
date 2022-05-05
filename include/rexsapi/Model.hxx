
#ifndef REXSAPI_MODEL_HXX
#define REXSAPI_MODEL_HXX

#include <rexsapi/Relation.hxx>

namespace rexsapi
{
  struct TModelInfo {
    std::string m_ApplicationId;
    std::string m_ApplicationVersion;
    std::string m_Date;
    std::string m_Version;
  };

  class TModel
  {
  public:
    TModel(TModelInfo info, TComponents&& components, TRelations&& relations)
    : m_Info{std::move(info)}
    , m_Components{std::move(components)}
    , m_Relations{std::move(relations)}
    {
    }

    const TModelInfo& getInfo() const
    {
      return m_Info;
    }

    const TComponents& getComponents() const
    {
      return m_Components;
    }

    const TRelations& getRelations() const
    {
      return m_Relations;
    }

  private:
    TModelInfo m_Info;
    TComponents m_Components;
    TRelations m_Relations;
  };
}

#endif
