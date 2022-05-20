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
  class ComponentId
  {
  public:
    explicit ComponentId(uint64_t id)
    : m_Id{id}
    {
    }

    explicit ComponentId(std::string id)
    : m_Id{id}
    {
    }

  private:
    std::variant<uint64_t, std::string> m_Id;
  };

  class TComponentBuilder
  {
  public:
    explicit TComponentBuilder(const database::TModel& databaseModel)
    : m_DatabaseModel{databaseModel}
    {
    }

    TComponentBuilder& addComponent(const std::string& component)
    {
      m_Components.emplace_back(ComponentEntry{getNextComponentId(), &m_DatabaseModel.findComponentById(component)});
      return *this;
    }

    TComponentBuilder& addComponent(const std::string& component, std::string id)
    {
      // TODO (lcf): check duplicate id
      m_Components.emplace_back(
        ComponentEntry{ComponentId{std::move(id)}, &m_DatabaseModel.findComponentById(component)});
      return *this;
    }

    TComponentBuilder& name(std::string name)
    {
      checkComponent();
      m_Components.back().m_Name = std::move(name);
      return *this;
    }

    TComponentBuilder& addAttribute(const std::string& attribute)
    {
      checkComponent();
      m_Components.back().m_Attributes.emplace_back(AttributeEntry{&m_DatabaseModel.findAttributetById(attribute)});
      return *this;
    }

    TComponentBuilder& unit(const std::string& unit)
    {
      checkAttribute();
      m_Components.back().m_Attributes.back().m_Unit = &m_DatabaseModel.findUnitByName(unit);
      return *this;
    }

    template<typename T>
    TComponentBuilder& value(T val)
    {
      checkAttribute();
      // TODO (lcf): check value type valid for attribute
      m_Components.back().m_Attributes.back().m_Value = TValue{val};
      return *this;
    }

    ComponentId id() const
    {
      checkComponent();
      return m_Components.back().m_Id;
    }

    TComponents build() const
    {
      uint64_t internalComponentId{0};
      TComponents components;

      for (const auto& component : m_Components) {
        TAttributes attributes;
        for (const auto& attribute : component.m_Attributes) {
          // TODO (lcf): check TUnit
          attributes.emplace_back(
            TAttribute{*attribute.m_Attribute, TUnit{attribute.m_Attribute->getUnit()}, attribute.m_Value});
        }
        components.emplace_back(TComponent{++internalComponentId, component.m_component->getComponentId(),
                                           component.m_component->getName(), std::move(attributes)});
      }

      return components;
    }

  private:
    void checkComponent() const
    {
      if (m_Components.empty()) {
        throw TException{"no components added yet"};
      }
    }

    void checkAttribute() const
    {
      checkComponent();
      if (m_Components.back().m_Attributes.empty()) {
        throw TException{"no attributes added yet"};
      }
    }

    ComponentId getNextComponentId()
    {
      return ComponentId{++m_ComponentId};
    }

    struct AttributeEntry {
      const database::TAttribute* m_Attribute{nullptr};
      const database::TUnit* m_Unit{nullptr};
      TValue m_Value{};
    };

    struct ComponentEntry {
      ComponentId m_Id;
      const database::TComponent* m_component{nullptr};
      std::string m_Name{};
      std::vector<AttributeEntry> m_Attributes{};
    };

    friend class TModelBuilder;
    const database::TModel& m_DatabaseModel;
    uint64_t m_ComponentId{0};
    std::vector<ComponentEntry> m_Components;
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

    TModelBuilder& addRef(TRelationRole role, ComponentId id)
    {
      (void)role;
      (void)id;
      return *this;
    }

    TModelBuilder& addComponent(const std::string& component)
    {
      m_ComponentBuilder.addComponent(component);
      return *this;
    }

    TModelBuilder& addComponent(const std::string& component, std::string id)
    {
      m_ComponentBuilder.addComponent(component, id);
      return *this;
    }

    TModelBuilder& name(std::string name)
    {
      m_ComponentBuilder.name(std::move(name));
      return *this;
    }

    TModelBuilder& addAttribute(const std::string& attribute)
    {
      m_ComponentBuilder.addAttribute(attribute);
      return *this;
    }

    TModelBuilder& unit(const std::string& unit)
    {
      m_ComponentBuilder.unit(unit);
      return *this;
    }

    template<typename T>
    TModelBuilder& value(T val)
    {
      m_ComponentBuilder.value(std::move(val));
      return *this;
    }

    ComponentId id() const
    {
      return m_ComponentBuilder.id();
    }

    TModel build(std::string applicationId, std::string applicationVersion) const
    {
      TRelations relations;
      rexsapi::TModelInfo info{std::move(applicationId), std::move(applicationVersion), "2022-05-20T08:59:10+01:00",
                               m_ComponentBuilder.m_DatabaseModel.getVersion()};
      return TModel{info, m_ComponentBuilder.build(), std::move(relations)};
    }

  private:
    TComponentBuilder m_ComponentBuilder;
  };
}

#endif
