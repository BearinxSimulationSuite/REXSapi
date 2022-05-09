
#ifndef REXSAPI_MODEL_HXX
#define REXSAPI_MODEL_HXX

#include <rexsapi/Relation.hxx>

namespace rexsapi
{
  class TModelInfo
  {
  public:
    TModelInfo(std::string applicationId, std::string applicationVersion, std::string date, std::string version)
    : m_ApplicationId{std::move(applicationId)}
    , m_ApplicationVersion{std::move(applicationVersion)}
    , m_Date{std::move(date)}
    , m_Version{std::move(version)}
    {
    }

    [[nodiscard]] const std::string& getApplicationId() const
    {
      return m_ApplicationId;
    }

    [[nodiscard]] const std::string& getApplicationVersion() const
    {
      return m_ApplicationVersion;
    }

    [[nodiscard]] const std::string& getDate() const
    {
      return m_Date;
    }

    [[nodiscard]] const std::string& getVersion() const
    {
      return m_Version;
    }

  private:
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

    [[nodiscard]] const TModelInfo& getInfo() const
    {
      return m_Info;
    }

    [[nodiscard]] const TComponents& getComponents() const
    {
      return m_Components;
    }

    [[nodiscard]] const TRelations& getRelations() const
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
