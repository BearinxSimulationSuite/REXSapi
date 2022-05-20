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

    friend bool operator==(const ComponentId& lhs, const ComponentId& rhs)
    {
      return lhs.m_Id == rhs.m_Id;
    }

    friend bool operator<(const ComponentId& lhs, const ComponentId& rhs)
    {
      return lhs.m_Id < rhs.m_Id;
    }

    std::size_t hash() const
    {
      return std::hash<Id>{}(m_Id);
    }

  private:
    using Id = std::variant<uint64_t, std::string>;
    Id m_Id;
  };
}

namespace std
{
  template<>
  struct std::hash<rexsapi::ComponentId> {
    std::size_t operator()(const rexsapi::ComponentId& id) const noexcept
    {
      return id.hash();
    }
  };
}

namespace rexsapi
{
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

    TComponents build()
    {
      m_ComponentMapping.clear();
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
        m_ComponentMapping[component.m_Id] = internalComponentId;
      }

      return components;
    }

    uint64_t getComponentForId(ComponentId id) const
    {
      auto it = m_ComponentMapping.find(id);
      if (it == m_ComponentMapping.end()) {
        return 0;
      }

      return it->second;
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
    std::unordered_map<ComponentId, uint64_t> m_ComponentMapping;
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
      m_Relations.emplace_back(RelationEntry{type});
      return *this;
    }

    TModelBuilder& addRef(TRelationRole role, ComponentId id)
    {
      checkRelation();
      m_Relations.back().m_References.emplace_back(ReferenceEntry{role, id});
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

    TModel build(std::string applicationId, std::string applicationVersion)
    {
      TRelations relations;
      // TODO (lcf): create current date
      rexsapi::TModelInfo info{std::move(applicationId), std::move(applicationVersion), "2022-05-20T08:59:10+01:00",
                               m_ComponentBuilder.m_DatabaseModel.getVersion()};
      auto components = m_ComponentBuilder.build();

      for (const auto& relation : m_Relations) {
        TRelationReferences references;
        for (const auto& reference : relation.m_References) {
          auto id = m_ComponentBuilder.getComponentForId(reference.m_Id);
          if (id == 0) {
            // TODO (lcf): stringify id
            throw TException{"component for id {} not found"};
          }
          references.emplace_back(TRelationReference{reference.m_Role, "hint", getComponentForId(components, id)});
        }
        relations.emplace_back(rexsapi::TRelation{relation.m_Type, {}, std::move(references)});
      }

      return TModel{info, std::move(components), std::move(relations)};
    }

  private:
    void checkRelation() const
    {
      if (m_Relations.empty()) {
        throw TException{"no relations added yet"};
      }
    }

    static const TComponent& getComponentForId(const TComponents& components, uint64_t id)
    {
      auto it = std::find_if(components.begin(), components.end(), [&id](const auto& component) {
        return component.getInternalId() == id;
      });
      if (it == components.end()) {
        throw TException{fmt::format("no component found for id {}", id)};
      }

      return *it;
    }

    struct ReferenceEntry {
      TRelationRole m_Role;
      ComponentId m_Id;
    };

    struct RelationEntry {
      TRelationType m_Type;
      std::vector<ReferenceEntry> m_References{};
    };

    TComponentBuilder m_ComponentBuilder;
    std::vector<RelationEntry> m_Relations;
  };
}

#endif
