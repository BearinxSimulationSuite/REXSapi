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
  class TComponentId
  {
  public:
    explicit TComponentId(uint64_t id)
    : m_Id{id}
    {
    }

    explicit TComponentId(std::string id)
    : m_Id{std::move(id)}
    {
    }

    friend bool operator==(const TComponentId& lhs, const TComponentId& rhs)
    {
      return lhs.m_Id == rhs.m_Id;
    }

    friend bool operator<(const TComponentId& lhs, const TComponentId& rhs)
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
  struct hash<rexsapi::TComponentId> {
    std::size_t operator()(const rexsapi::TComponentId& id) const noexcept
    {
      return id.hash();
    }
  };
}

namespace rexsapi
{
  namespace detail
  {
    struct TAttributeEntry {
      const database::TAttribute* m_Attribute{nullptr};
      std::string m_AttributeId{};
      std::optional<TValueType> m_ValueType{};
      std::optional<TUnit> m_Unit{};
      TValue m_Value{};
    };

    struct TComponentEntry {
      TComponentId m_Id;
      const database::TComponent* m_component{nullptr};
      std::string m_Name{};
      std::vector<TAttributeEntry> m_Attributes{};
    };
  }


  class TComponentBuilder
  {
  public:
    explicit TComponentBuilder(const database::TModel& databaseModel)
    : m_DatabaseModel{databaseModel}
    {
    }

    TComponentBuilder& addComponent(const std::string& component) &;

    TComponentBuilder& addComponent(const std::string& component, std::string id) &;

    TComponentBuilder& name(std::string name) &;

    TComponentBuilder& addAttribute(const std::string& attributeId) &;

    TComponentBuilder& addCustomAttribute(const std::string& attribute, TValueType type) &;

    TComponentBuilder& unit(const std::string& unit) &;

    template<typename T>
    TComponentBuilder& value(T val) &;

    [[nodiscard]] TComponentId id() const;

    [[nodiscard]] TComponents build();

    const TComponent& getComponentForId(const TComponents& components, const TComponentId& id) const&;

  private:
    void checkDuplicateComponent(const std::string& component, const std::string& id = "") const
    {
      const auto it = std::find_if(m_Components.begin(), m_Components.end(), [&component, &id](const auto& comp) {
        bool res = comp.m_component->getComponentId() == component;
        res |= comp.m_Id == TComponentId{id};
        return res;
      });
      if (it != m_Components.end()) {
        throw TException{fmt::format("component {} already added", component)};
      }
    }

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

    uint64_t getComponentForId(const TComponentId& id) const;

    [[nodiscard]] TComponentId getNextComponentId()
    {
      return TComponentId{++m_ComponentId};
    }

    friend class TModelBuilder;
    const database::TModel& m_DatabaseModel;
    uint64_t m_ComponentId{0};
    std::vector<detail::TComponentEntry> m_Components;
    std::unordered_map<TComponentId, uint64_t> m_ComponentMapping;
  };


  class TLoadCaseBuilder
  {
  public:
    explicit TLoadCaseBuilder(const database::TModel& databaseModel)
    : m_DatabaseModel{databaseModel}
    {
    }

    ~TLoadCaseBuilder() = default;

    TLoadCaseBuilder(const TLoadCaseBuilder&) = delete;
    TLoadCaseBuilder& operator=(const TLoadCaseBuilder&) = delete;
    TLoadCaseBuilder(TLoadCaseBuilder&&) = default;
    TLoadCaseBuilder& operator=(TLoadCaseBuilder&&) = delete;

    TLoadCaseBuilder& addComponent(TComponentId id) &;

    TLoadCaseBuilder& addComponent(std::string id) &;

    TLoadCaseBuilder& addAttribute(const std::string& attributeId) &;

    TLoadCaseBuilder& addCustomAttribute(const std::string& attribute, TValueType type) &;

    TLoadCaseBuilder& unit(const std::string& unit) &;

    template<typename T>
    TLoadCaseBuilder& value(T val) &;

    TLoadCase build(const TComponents& components, const TComponentBuilder& componentBuilder) const;

  private:
    void checkDuplicateComponent(const TComponentId& component) const
    {
      const auto it = std::find_if(m_Components.begin(), m_Components.end(), [&component](const auto& comp) {
        return comp.m_Id == component;
      });
      if (it != m_Components.end()) {
        throw TException{fmt::format("component id={} already added to loadcase", component.asString())};
      }
    }

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
    std::vector<detail::TComponentEntry> m_Components;
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

    TModelBuilder& addRelation(TRelationType type) &;

    TModelBuilder& order(uint32_t order) &;

    TModelBuilder& addRef(TRelationRole role, TComponentId id) &;

    TModelBuilder& addRef(TRelationRole role, std::string id) &;

    TModelBuilder& hint(std::string hint) &;

    TModelBuilder& addComponent(const std::string& component) &;

    TModelBuilder& addComponent(const std::string& component, std::string id) &;

    TModelBuilder& name(std::string name) &;

    TModelBuilder& addAttribute(const std::string& attribute) &;

    TModelBuilder& addCustomAttribute(const std::string& attribute, TValueType type) &;

    TModelBuilder& unit(const std::string& unit) &;

    template<typename T>
    TModelBuilder& value(T val) &;

    [[nodiscard]] TLoadCaseBuilder& addLoadCase() &;

    [[nodiscard]] TComponentId id() const;

    [[nodiscard]] TModel build(std::string applicationId, std::string applicationVersion,
                               std::optional<std::string> applicationLanguage);

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
      TComponentId m_Id;
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

  inline std::string TComponentId::asString() const
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


  inline TComponentBuilder& TComponentBuilder::addComponent(const std::string& component) &
  {
    checkDuplicateComponent(component);
    m_Components.emplace_back(
      detail::TComponentEntry{getNextComponentId(), &m_DatabaseModel.findComponentById(component)});
    return *this;
  }

  inline TComponentBuilder& TComponentBuilder::addComponent(const std::string& component, std::string id) &
  {
    checkDuplicateComponent(component, id);
    m_Components.emplace_back(
      detail::TComponentEntry{TComponentId{std::move(id)}, &m_DatabaseModel.findComponentById(component)});
    return *this;
  }

  inline TComponentBuilder& TComponentBuilder::name(std::string name) &
  {
    checkComponent();
    m_Components.back().m_Name = std::move(name);
    return *this;
  }

  inline TComponentBuilder& TComponentBuilder::addAttribute(const std::string& attribute) &
  {
    checkComponent();
    m_Components.back().m_Attributes.emplace_back(
      detail::TAttributeEntry{&m_DatabaseModel.findAttributetById(attribute)});
    return *this;
  }

  inline TComponentBuilder& TComponentBuilder::addCustomAttribute(const std::string& attribute, TValueType type) &
  {
    checkComponent();
    m_Components.back().m_Attributes.emplace_back(detail::TAttributeEntry{nullptr, attribute, type});
    return *this;
  }

  inline TComponentBuilder& TComponentBuilder::unit(const std::string& unit) &
  {
    checkAttribute();
    // TODO (lcf): check unit
    if (m_Components.back().m_Attributes.back().m_Attribute != nullptr) {
      m_Components.back().m_Attributes.back().m_Unit = TUnit{m_DatabaseModel.findUnitByName(unit)};
    } else {
      m_Components.back().m_Attributes.back().m_Unit = TUnit{unit};
    }
    return *this;
  }

  template<typename T>
  inline TComponentBuilder& TComponentBuilder::value(T val) &
  {
    checkAttribute();
    // TODO (lcf): check value type valid for attribute
    m_Components.back().m_Attributes.back().m_Value = TValue{std::move(val)};
    return *this;
  }

  inline TComponentId TComponentBuilder::id() const
  {
    checkComponent();
    return m_Components.back().m_Id;
  }

  inline static TAttribute createAttribute(const detail::TAttributeEntry& entry)
  {
    TUnit unit{};
    if (entry.m_Attribute != nullptr) {
      if (entry.m_Unit.has_value()) {
        unit = *entry.m_Unit;
      } else {
        unit = TUnit{entry.m_Attribute->getUnit()};
      }
      return TAttribute{*entry.m_Attribute, unit, entry.m_Value};
    }
    if (entry.m_Unit.has_value()) {
      unit = *entry.m_Unit;
    }
    return TAttribute{entry.m_AttributeId, unit, *entry.m_ValueType, entry.m_Value};
  }

  inline TComponents TComponentBuilder::build()
  {
    m_ComponentMapping.clear();
    uint64_t internalComponentId{0};
    TComponents components;

    for (const auto& component : m_Components) {
      TAttributes attributes;
      for (const auto& attribute : component.m_Attributes) {
        attributes.emplace_back(createAttribute(attribute));
      }
      components.emplace_back(TComponent{++internalComponentId, component.m_component->getComponentId(),
                                         component.m_Name, std::move(attributes)});
      m_ComponentMapping[component.m_Id] = internalComponentId;
    }

    return components;
  }

  inline const TComponent& TComponentBuilder::getComponentForId(const TComponents& components,
                                                                const TComponentId& id) const&
  {
    const auto cid = getComponentForId(id);
    auto it = std::find_if(components.begin(), components.end(), [cid](const auto& component) {
      return component.getInternalId() == cid;
    });
    if (it == components.end()) {
      throw TException{fmt::format("no component found for id={}", id.asString())};
    }

    return *it;
  }

  inline uint64_t TComponentBuilder::getComponentForId(const TComponentId& id) const
  {
    auto it = m_ComponentMapping.find(id);
    if (it == m_ComponentMapping.end()) {
      return 0;
    }

    return it->second;
  }


  inline TLoadCaseBuilder& TLoadCaseBuilder::addComponent(TComponentId id) &
  {
    checkDuplicateComponent(id);
    m_Components.emplace_back(detail::TComponentEntry{TComponentId{std::move(id)}});
    return *this;
  }

  inline TLoadCaseBuilder& TLoadCaseBuilder::addComponent(std::string id) &
  {
    checkDuplicateComponent(TComponentId{id});
    m_Components.emplace_back(detail::TComponentEntry{TComponentId{std::move(id)}});
    return *this;
  }

  inline TLoadCaseBuilder& TLoadCaseBuilder::addAttribute(const std::string& attribute) &
  {
    checkComponent();
    m_Components.back().m_Attributes.emplace_back(
      detail::TAttributeEntry{&m_DatabaseModel.findAttributetById(attribute)});
    return *this;
  }

  inline TLoadCaseBuilder& TLoadCaseBuilder::addCustomAttribute(const std::string& attribute, TValueType type) &
  {
    checkComponent();
    m_Components.back().m_Attributes.emplace_back(detail::TAttributeEntry{nullptr, attribute, type});
    return *this;
  }

  inline TLoadCaseBuilder& TLoadCaseBuilder::unit(const std::string& unit) &
  {
    checkAttribute();
    if (m_Components.back().m_Attributes.back().m_Attribute != nullptr) {
      m_Components.back().m_Attributes.back().m_Unit = TUnit{m_DatabaseModel.findUnitByName(unit)};
    } else {
      m_Components.back().m_Attributes.back().m_Unit = TUnit{unit};
    }

    return *this;
  }

  template<typename T>
  inline TLoadCaseBuilder& TLoadCaseBuilder::value(T val) &
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
        loadAttributes.emplace_back(createAttribute(attribute));
      }
      loadComponents.emplace_back(
        TLoadComponent{componentBuilder.getComponentForId(components, component.m_Id), std::move(loadAttributes)});
    }

    return TLoadCase{std::move(loadComponents)};
  }


  inline TModelBuilder& TModelBuilder::addRelation(TRelationType type) &
  {
    m_Relations.emplace_back(RelationEntry{type});
    return *this;
  }

  inline TModelBuilder& TModelBuilder::order(uint32_t order) &
  {
    checkRelation();
    m_Relations.back().m_Order = order;
    return *this;
  }

  inline TModelBuilder& TModelBuilder::addRef(TRelationRole role, TComponentId id) &
  {
    checkRelation();
    m_Relations.back().m_References.emplace_back(ReferenceEntry{role, std::move(id)});
    return *this;
  }

  inline TModelBuilder& TModelBuilder::addRef(TRelationRole role, std::string id) &
  {
    checkRelation();
    m_Relations.back().m_References.emplace_back(ReferenceEntry{role, TComponentId{std::move(id)}});
    return *this;
  }

  inline TModelBuilder& TModelBuilder::hint(std::string hint) &
  {
    checkReference();
    m_Relations.back().m_References.back().m_Hint = std::move(hint);
    return *this;
  }

  inline TModelBuilder& TModelBuilder::addComponent(const std::string& component) &
  {
    m_ComponentBuilder.addComponent(component);
    return *this;
  }

  inline TModelBuilder& TModelBuilder::addComponent(const std::string& component, std::string id) &
  {
    m_ComponentBuilder.addComponent(component, std::move(id));
    return *this;
  }

  inline TModelBuilder& TModelBuilder::name(std::string name) &
  {
    m_ComponentBuilder.name(std::move(name));
    return *this;
  }

  inline TModelBuilder& TModelBuilder::addAttribute(const std::string& attribute) &
  {
    m_ComponentBuilder.addAttribute(attribute);
    return *this;
  }

  inline TModelBuilder& TModelBuilder::addCustomAttribute(const std::string& attribute, TValueType type) &
  {
    m_ComponentBuilder.addCustomAttribute(attribute, type);
    return *this;
  }

  inline TModelBuilder& TModelBuilder::unit(const std::string& unit) &
  {
    m_ComponentBuilder.unit(unit);
    return *this;
  }

  template<typename T>
  inline TModelBuilder& TModelBuilder::value(T val) &
  {
    m_ComponentBuilder.value(std::move(val));
    return *this;
  }

  inline TLoadCaseBuilder& TModelBuilder::addLoadCase() &
  {
    m_LoadCases.emplace_back(TLoadCaseBuilder{m_ComponentBuilder.m_DatabaseModel});
    return m_LoadCases.back();
  }

  inline TComponentId TModelBuilder::id() const
  {
    return m_ComponentBuilder.id();
  }

  inline TModel TModelBuilder::build(std::string applicationId, std::string applicationVersion,
                                     std::optional<std::string> language)
  {
    TRelations relations;
    const rexsapi::TModelInfo info{std::move(applicationId), std::move(applicationVersion),
                                   getTimeStringISO8601(std::chrono::system_clock::now()),
                                   m_ComponentBuilder.m_DatabaseModel.getVersion(), std::move(language)};
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
