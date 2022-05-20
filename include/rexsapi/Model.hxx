/*
 * Copyright Schaeffler Technologies AG & Co. KG (info.de@schaeffler.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef REXSAPI_MODEL_HXX
#define REXSAPI_MODEL_HXX

#include <rexsapi/Relation.hxx>
#include <rexsapi/RexsVersion.hxx>

namespace rexsapi
{
  class TModelInfo
  {
  public:
    TModelInfo(std::string applicationId, std::string applicationVersion, std::string date, TRexsVersion version)
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

    [[nodiscard]] const TRexsVersion& getVersion() const
    {
      return m_Version;
    }

  private:
    std::string m_ApplicationId;
    std::string m_ApplicationVersion;
    std::string m_Date;
    TRexsVersion m_Version;
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

    ~TModel() = default;

    TModel(const TModel&) = delete;
    TModel(TModel&&) = default;
    TModel& operator=(const TModel&) = delete;
    TModel& operator=(TModel&&) = default;

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
