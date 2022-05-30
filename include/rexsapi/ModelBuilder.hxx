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
    : m_Id{std::move(id)}
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

    std::string asString() const;

  private:
    using Id = std::variant<uint64_t, std::string>;
    Id m_Id;
  };
}

namespace std
{
  template<>
  struct hash<rexsapi::ComponentId> {
    std::size_t operator()(const rexsapi::ComponentId& id) const noexcept
    {
      return id.hash();
    }
  };
}

namespace rexsapi
{
  namespace detail
  {
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
  }


  class TComponentBuilder
  {
  public:
    explicit TComponentBuilder(const database::TModel& databaseModel)
    : m_DatabaseModel{databaseModel}
    {
    }

    TComponentBuilder& addComponent(const std::string& component);

    TComponentBuilder& addComponent(const std::string& component, std::string id);

    TComponentBuilder& name(std::string name);

    TComponentBuilder& addAttribute(const std::string& attribute);

    TComponentBuilder& unit(const std::string& unit);

    template<typename T>
    TComponentBuilder& value(T val);

    [[nodiscard]] ComponentId id() const;

    [[nodiscard]] TComponents build();

    const TComponent& getComponentForId(const TComponents& components, const ComponentId& id) const;

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

    uint64_t getComponentForId(const ComponentId& id) const;

    [[nodiscard]] ComponentId getNextComponentId()
    {
      return ComponentId{++m_ComponentId};
    }

    friend class TModelBuilder;
    const database::TModel& m_DatabaseModel;
    uint64_t m_ComponentId{0};
    std::vector<detail::ComponentEntry> m_Components;
    std::unordered_map<ComponentId, uint64_t> m_ComponentMapping;
  };


  class TLoadCaseBuilder
  {
  public:
    explicit TLoadCaseBuilder(const database::TModel& databaseModel)
    : m_DatabaseModel{databaseModel}
    {
    }

    TLoadCaseBuilder& addComponent(ComponentId id);

    TLoadCaseBuilder& addComponent(std::string id);

    TLoadCaseBuilder& addAttribute(const std::string& attribute);

    TLoadCaseBuilder& unit(const std::string& unit);

    template<typename T>
    TLoadCaseBuilder& value(T val);

    TLoadCase build(const TComponents& components, const TComponentBuilder& componentBuilder) const;

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

    const database::TModel& m_DatabaseModel;
    std::vector<detail::ComponentEntry> m_Components;
  };


  class TModelBuilder
  {
  public:
    explicit TModelBuilder(const database::TModel& databaseModel)
    : m_ComponentBuilder{databaseModel}
    {
    }

    explicit TModelBuilder(TComponentBuilder componentBuilder)
    : m_ComponentBuilder{std::move(componentBuilder)}
    {
    }

    TModelBuilder& addRelation(TRelationType type);

    TModelBuilder& order(uint32_t order);

    TModelBuilder& addRef(TRelationRole role, ComponentId id);

    TModelBuilder& addRef(TRelationRole role, std::string id);

    TModelBuilder& hint(std::string hint);

    TModelBuilder& addComponent(const std::string& component);

    TModelBuilder& addComponent(const std::string& component, std::string id);

    TModelBuilder& name(std::string name);

    TModelBuilder& addAttribute(const std::string& attribute);

    TModelBuilder& unit(const std::string& unit);

    template<typename T>
    TModelBuilder& value(T val);

    [[nodiscard]] TLoadCaseBuilder& addLoadCase();

    [[nodiscard]] ComponentId id() const;

    [[nodiscard]] TModel build(std::string applicationId, std::string applicationVersion);

  private:
    void checkRelation() const
    {
      if (m_Relations.empty()) {
        throw TException{"no relations added yet"};
      }
    }

    void checkReference() const
    {
      checkRelation();
      if (m_Relations.back().m_References.empty()) {
        throw TException{"no references added yet"};
      }
    }

    struct ReferenceEntry {
      TRelationRole m_Role;
      ComponentId m_Id;
      std::string m_Hint{};
    };

    struct RelationEntry {
      TRelationType m_Type;
      std::optional<uint32_t> m_Order{};
      std::vector<ReferenceEntry> m_References{};
    };

    TComponentBuilder m_ComponentBuilder;
    std::vector<RelationEntry> m_Relations;
    std::vector<TLoadCaseBuilder> m_LoadCases;
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline std::string ComponentId::asString() const
  {
    return std::visit(overload{[](const uint64_t& n) -> std::string {
                                 return std::to_string(n);
                               },
                               [](const std::string& s) {
                                 // TODO (lcf): add "" for string
                                 return s;
                               }},
                      m_Id);
  }


  inline TComponentBuilder& TComponentBuilder::addComponent(const std::string& component)
  {
    m_Components.emplace_back(
      detail::ComponentEntry{getNextComponentId(), &m_DatabaseModel.findComponentById(component)});
    return *this;
  }

  inline TComponentBuilder& TComponentBuilder::addComponent(const std::string& component, std::string id)
  {
    // TODO (lcf): check duplicate id
    m_Components.emplace_back(
      detail::ComponentEntry{ComponentId{std::move(id)}, &m_DatabaseModel.findComponentById(component)});
    return *this;
  }

  inline TComponentBuilder& TComponentBuilder::name(std::string name)
  {
    checkComponent();
    m_Components.back().m_Name = std::move(name);
    return *this;
  }

  inline TComponentBuilder& TComponentBuilder::addAttribute(const std::string& attribute)
  {
    checkComponent();
    m_Components.back().m_Attributes.emplace_back(
      detail::AttributeEntry{&m_DatabaseModel.findAttributetById(attribute)});
    return *this;
  }

  inline TComponentBuilder& TComponentBuilder::unit(const std::string& unit)
  {
    checkAttribute();
    m_Components.back().m_Attributes.back().m_Unit = &m_DatabaseModel.findUnitByName(unit);
    return *this;
  }

  template<typename T>
  inline TComponentBuilder& TComponentBuilder::value(T val)
  {
    checkAttribute();
    // TODO (lcf): check value type valid for attribute
    m_Components.back().m_Attributes.back().m_Value = TValue{std::move(val)};
    return *this;
  }

  inline ComponentId TComponentBuilder::id() const
  {
    checkComponent();
    return m_Components.back().m_Id;
  }

  inline TComponents TComponentBuilder::build()
  {
    m_ComponentMapping.clear();
    uint64_t internalComponentId{0};
    TComponents components;

    for (const auto& component : m_Components) {
      TAttributes attributes;
      for (const auto& attribute : component.m_Attributes) {
        // TODO (lcf): check TUnit
        // TODO (lcf): check value, may not be set
        attributes.emplace_back(
          TAttribute{*attribute.m_Attribute, TUnit{attribute.m_Attribute->getUnit()}, attribute.m_Value});
      }
      components.emplace_back(TComponent{++internalComponentId, component.m_component->getComponentId(),
                                         component.m_Name, std::move(attributes)});
      m_ComponentMapping[component.m_Id] = internalComponentId;
    }

    return components;
  }

  inline const TComponent& TComponentBuilder::getComponentForId(const TComponents& components,
                                                                const ComponentId& id) const
  {
    auto cid = getComponentForId(id);
    auto it = std::find_if(components.begin(), components.end(), [cid](const auto& component) {
      return component.getInternalId() == cid;
    });
    if (it == components.end()) {
      throw TException{fmt::format("no component found for id '{}'", id.asString())};
    }

    return *it;
  }

  inline uint64_t TComponentBuilder::getComponentForId(const ComponentId& id) const
  {
    auto it = m_ComponentMapping.find(id);
    if (it == m_ComponentMapping.end()) {
      return 0;
    }

    return it->second;
  }


  inline TLoadCaseBuilder& TLoadCaseBuilder::addComponent(ComponentId id)
  {
    m_Components.emplace_back(detail::ComponentEntry{ComponentId{std::move(id)}});
    return *this;
  }

  inline TLoadCaseBuilder& TLoadCaseBuilder::addComponent(std::string id)
  {
    m_Components.emplace_back(detail::ComponentEntry{ComponentId{std::move(id)}});
    return *this;
  }

  inline TLoadCaseBuilder& TLoadCaseBuilder::addAttribute(const std::string& attribute)
  {
    checkComponent();
    m_Components.back().m_Attributes.emplace_back(
      detail::AttributeEntry{&m_DatabaseModel.findAttributetById(attribute)});
    return *this;
  }

  inline TLoadCaseBuilder& TLoadCaseBuilder::unit(const std::string& unit)
  {
    checkAttribute();
    m_Components.back().m_Attributes.back().m_Unit = &m_DatabaseModel.findUnitByName(unit);
    return *this;
  }

  template<typename T>
  inline TLoadCaseBuilder& TLoadCaseBuilder::value(T val)
  {
    checkAttribute();
    // TODO (lcf): check value type valid for attribute
    m_Components.back().m_Attributes.back().m_Value = TValue{std::move(val)};
    return *this;
  }

  inline TLoadCase TLoadCaseBuilder::build(const TComponents& components,
                                           const TComponentBuilder& componentBuilder) const
  {
    TLoadComponents loadComponents;
    for (const auto& component : m_Components) {
      TAttributes loadAttributes;

      for (const auto& attribute : component.m_Attributes) {
        loadAttributes.emplace_back(
          TAttribute{*attribute.m_Attribute, TUnit{attribute.m_Attribute->getUnit()}, attribute.m_Value});
      }
      loadComponents.emplace_back(
        TLoadComponent{componentBuilder.getComponentForId(components, component.m_Id), std::move(loadAttributes)});
    }

    return TLoadCase{std::move(loadComponents)};
  }


  inline TModelBuilder& TModelBuilder::addRelation(TRelationType type)
  {
    m_Relations.emplace_back(RelationEntry{type});
    return *this;
  }

  inline TModelBuilder& TModelBuilder::order(uint32_t order)
  {
    checkRelation();
    m_Relations.back().m_Order = order;
    return *this;
  }

  inline TModelBuilder& TModelBuilder::addRef(TRelationRole role, ComponentId id)
  {
    checkRelation();
    m_Relations.back().m_References.emplace_back(ReferenceEntry{role, std::move(id)});
    return *this;
  }

  inline TModelBuilder& TModelBuilder::addRef(TRelationRole role, std::string id)
  {
    checkRelation();
    m_Relations.back().m_References.emplace_back(ReferenceEntry{role, ComponentId{std::move(id)}});
    return *this;
  }

  inline TModelBuilder& TModelBuilder::hint(std::string hint)
  {
    checkReference();
    m_Relations.back().m_References.back().m_Hint = std::move(hint);
    return *this;
  }

  inline TModelBuilder& TModelBuilder::addComponent(const std::string& component)
  {
    m_ComponentBuilder.addComponent(component);
    return *this;
  }

  inline TModelBuilder& TModelBuilder::addComponent(const std::string& component, std::string id)
  {
    m_ComponentBuilder.addComponent(component, std::move(id));
    return *this;
  }

  inline TModelBuilder& TModelBuilder::name(std::string name)
  {
    m_ComponentBuilder.name(std::move(name));
    return *this;
  }

  inline TModelBuilder& TModelBuilder::addAttribute(const std::string& attribute)
  {
    m_ComponentBuilder.addAttribute(attribute);
    return *this;
  }

  inline TModelBuilder& TModelBuilder::unit(const std::string& unit)
  {
    m_ComponentBuilder.unit(unit);
    return *this;
  }

  template<typename T>
  inline TModelBuilder& TModelBuilder::value(T val)
  {
    m_ComponentBuilder.value(std::move(val));
    return *this;
  }

  inline TLoadCaseBuilder& TModelBuilder::addLoadCase()
  {
    m_LoadCases.emplace_back(TLoadCaseBuilder{m_ComponentBuilder.m_DatabaseModel});
    return m_LoadCases.back();
  }

  inline ComponentId TModelBuilder::id() const
  {
    return m_ComponentBuilder.id();
  }

  inline TModel TModelBuilder::build(std::string applicationId, std::string applicationVersion)
  {
    TRelations relations;
    // TODO (lcf): create current date
    rexsapi::TModelInfo info{std::move(applicationId), std::move(applicationVersion), "2022-05-20T08:59:10+01:00",
                             m_ComponentBuilder.m_DatabaseModel.getVersion()};
    auto components = m_ComponentBuilder.build();

    // TODO (lcf): add model validation. check no components and/or no relations

    for (const auto& relation : m_Relations) {
      TRelationReferences references;
      for (const auto& reference : relation.m_References) {
        const auto& component = m_ComponentBuilder.getComponentForId(components, reference.m_Id);
        references.emplace_back(TRelationReference{reference.m_Role, reference.m_Hint, component});
      }
      relations.emplace_back(rexsapi::TRelation{relation.m_Type, relation.m_Order, std::move(references)});
    }

    TLoadCases loadCases;
    for (const auto& loadCase : m_LoadCases) {
      loadCases.emplace_back(loadCase.build(components, m_ComponentBuilder));
    }

    TLoadSpectrum spectrum{std::move(loadCases)};

    return TModel{info, std::move(components), std::move(relations), std::move(spectrum)};
  }
}

#endif
