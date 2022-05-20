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

#ifndef REXSAPI_MODEL_BUILDER_HXX
#define REXSAPI_MODEL_BUILDER_HXX

#include <rexsapi/Model.hxx>
#include <rexsapi/database/Model.hxx>


namespace rexsapi
{
  class TComponentBuilder
  {
  public:
    explicit TComponentBuilder(const database::TModel& databaseModel)
    : m_DatabaseModel{databaseModel}
    {
    }

    TComponentBuilder& addComponent(std::string component)
    {
      (void)component;
      return *this;
    }

    TComponentBuilder& addComponent(std::string component, std::string id)
    {
      (void)component;
      (void)id;
      return *this;
    }

    TComponentBuilder& addAttribute(std::string attribute)
    {
      (void)attribute;
      return *this;
    }

    TComponentBuilder& name(std::string name)
    {
      (void)name;
      return *this;
    }

    TComponentBuilder& unit(std::string unit)
    {
      (void)unit;
      return *this;
    }

    template<typename T>
    TComponentBuilder& value(T val)
    {
      (void)val;
      return *this;
    }

    uint64_t id() const
    {
      return 0;
    }

  private:
    friend class TModelBuilder;
    const database::TModel& m_DatabaseModel;
  };


  class TModelBuilder
  {
  public:
    explicit TModelBuilder(const database::TModel& databaseModel)
    : m_ComponentBuilder{databaseModel}
    {
    }

    explicit TModelBuilder(TComponentBuilder&& componentBuilder)
    : m_ComponentBuilder{std::move(componentBuilder)}
    {
    }

    TModelBuilder& addRelation(TRelationType type)
    {
      (void)type;
      return *this;
    }

    TModelBuilder& addRef(TRelationRole role, uint64_t id)
    {
      (void)role;
      (void)id;
      return *this;
    }

    TModelBuilder& addRef(TRelationRole role, const std::string& id)
    {
      (void)role;
      (void)id;
      return *this;
    }

    TModelBuilder& addComponent(std::string component)
    {
      (void)component;
      return *this;
    }

    TModelBuilder& addComponent(std::string component, std::string id)
    {
      (void)component;
      (void)id;
      return *this;
    }

    TModelBuilder& addAttribute(std::string attribute)
    {
      (void)attribute;
      return *this;
    }

    TModelBuilder& name(std::string name)
    {
      (void)name;
      return *this;
    }

    TModelBuilder& unit(std::string unit)
    {
      (void)unit;
      return *this;
    }

    template<typename T>
    TModelBuilder& value(T val)
    {
      (void)val;
      return *this;
    }

    uint64_t id() const
    {
      return 0;
    }

    TModel build(std::string applicationId, std::string applicationVersion) const
    {
      rexsapi::TComponents components;
      TRelations relations;
      rexsapi::TModelInfo info{std::move(applicationId), std::move(applicationVersion), "2022-05-20T08:59:10+01:00",
                               m_ComponentBuilder.m_DatabaseModel.getVersion()};
      return TModel{info, std::move(components), std::move(relations)};
    }

  private:
    TComponentBuilder m_ComponentBuilder;
  };
}

#endif
