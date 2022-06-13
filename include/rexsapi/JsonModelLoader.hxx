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

#ifndef REXSAPI_JSON_MODEL_LOADER_HXX
#define REXSAPI_JSON_MODEL_LOADER_HXX

#include <rexsapi/ConversionHelper.hxx>
#include <rexsapi/Json.hxx>
#include <rexsapi/JsonValueDecoder.hxx>
#include <rexsapi/Mode.hxx>
#include <rexsapi/Model.hxx>
#include <rexsapi/ValidityChecker.hxx>
#include <rexsapi/database/ModelRegistry.hxx>

#include <set>

namespace rexsapi
{
  template<typename ValueDecoderType>
  class TModelLoaderHelper
  {
  public:
    TModelLoaderHelper(TMode mode)
    : m_Mode{mode}
    {
    }

    bool checkCustom(TResult& result, const std::string& context, const std::string& attributeId, uint64_t componentId,
                     const database::TComponent& componentType) const
    {
      bool isCustom{false};
      if (attributeId.substr(0, 7) == "custom_") {
        isCustom = true;
      } else {
        if (!componentType.hasAttribute(attributeId)) {
          isCustom = true;
          result.addError(
            TError{m_Mode.adapt(TErrorLevel::ERR), fmt::format("{}: attribute id={} is not part of component id={}",
                                                               context, attributeId, componentId)});
        }
      }
      return isCustom;
    }

    template<typename NodeType>
    TValue getValue(TResult& result, const database::TAttribute& dbAttribute, const std::string& context,
                    const std::string& attributeId, uint64_t componentId, const NodeType& attribute) const
    {
      auto value = m_Decoder.decode(dbAttribute.getValueType(), dbAttribute.getEnums(), attribute);
      if (!value.second) {
        result.addError(
          TError{m_Mode.adapt(TErrorLevel::ERR),
                 fmt::format("{}: value of attribute id={} of component id={} does not have the correct value type",
                             context, attributeId, componentId)});
        return TValue{};
      }
      if (!TValidityChecker::check(dbAttribute, value.first)) {
        result.addError(TError{m_Mode.adapt(TErrorLevel::ERR),
                               fmt::format("{}: value is out of range for attribute id={} of component id={}", context,
                                           attributeId, componentId)});
      }

      return value.first;
    }

    template<typename NodeType>
    TValue getValue(TResult& result, TValueType valueType, const std::string& context, const std::string& attributeId,
                    uint64_t componentId, const NodeType& attribute) const
    {
      auto value = m_Decoder.decode(valueType, {}, attribute);
      if (!value.second) {
        result.addError(
          TError{m_Mode.adapt(TErrorLevel::ERR),
                 fmt::format("{}: value of attribute id={} of component id={} does not have the correct value type",
                             context, attributeId, componentId)});
        return TValue{};
      }
      return value.first;
    }

  private:
    TModeAdapter m_Mode;
    ValueDecoderType m_Decoder;
  };

  class ComponentMapping
  {
  public:
    uint64_t addComponent(uint64_t componentId)
    {
      auto res = ++m_InternalComponentId;
      m_ComponentsMapping[componentId] = res;
      return res;
    }

    inline const TComponent* getComponent(uint64_t referenceId, const TComponents& components) const&
    {
      auto it = m_ComponentsMapping.find(referenceId);
      if (it == m_ComponentsMapping.end()) {
        return nullptr;
      }
      auto it_comp = std::find_if(components.begin(), components.end(), [&it](const auto& comp) {
        return comp.getInternalId() == it->second;
      });
      return it_comp.operator->();
    }

  private:
    uint64_t m_InternalComponentId{0};
    std::unordered_map<uint64_t, uint64_t> m_ComponentsMapping;
  };


  class TJsonModelValidator
  {
  public:
    TJsonModelValidator() = default;
  };


  class TJsonModelLoader
  {
  public:
    explicit TJsonModelLoader(TMode mode, const TJsonModelValidator& validator)
    : m_Mode{mode}
    , m_LoaderHelper{mode}
    , m_Validator{validator}
    {
    }

    std::optional<TModel> load(TResult& result, const database::TModelRegistry& registry,
                               std::vector<uint8_t>& buffer) const;

  private:
    TComponents getComponents(TResult& result, ComponentMapping componentMapping, const json& j,
                              const database::TModel& dbModel) const;

    TAttributes getAttributes(const std::string& context, TResult& result, uint64_t componentId,
                              const database::TComponent& componentType, const json& component) const;

    TRelations getRelations(TResult& result, ComponentMapping componentMapping, const TComponents& components,
                            const json& j) const;

    TValueType getValueType(const json& attribute) const;

    TModeAdapter m_Mode;
    TModelLoaderHelper<TJsonValueDecoder> m_LoaderHelper;
    const TJsonModelValidator& m_Validator;
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline std::optional<TModel> TJsonModelLoader::load(TResult& result, const database::TModelRegistry& registry,
                                                      std::vector<uint8_t>& buffer) const
  {
    try {
      json j;
      j = json::parse(buffer);

      std::optional<std::string> language;
      if (j.contains("/model/applicationLanguage"_json_pointer)) {
        language = j["/model/applicationLanguage"_json_pointer].get<std::string>();
      }

      TModelInfo info{j["/model/applicationId"_json_pointer].get<std::string>(),
                      j["/model/applicationVersion"_json_pointer].get<std::string>(),
                      j["/model/date"_json_pointer].get<std::string>(),
                      TRexsVersion{j["/model/version"_json_pointer].get<std::string>()}, language};

      const auto& dbModel = registry.getModel(info.getVersion(), "en");

      ComponentMapping componentMapping;
      TComponents components = getComponents(result, componentMapping, j, dbModel);

      TRelations relations = getRelations(result, componentMapping, components, j);

      TLoadCases loadCases;

      return TModel{info, std::move(components), std::move(relations), TLoadSpectrum{std::move(loadCases)}};
    } catch (const json::exception& ex) {
      result.addError(TError{TErrorLevel::CRIT, fmt::format("cannot parse json document: {}", ex.what())});
    }
    return {};
  }

  inline TComponents TJsonModelLoader::getComponents(TResult& result, ComponentMapping componentMapping, const json& j,
                                                     const database::TModel& dbModel) const
  {
    TComponents components;

    for (const auto& component : j["/model/components"_json_pointer]) {
      auto componentId = component["id"].get<uint64_t>();
      std::string componentName = component.value("name", "");
      try {
        const auto& componentType = dbModel.findComponentById(component["type"].get<std::string>());

        TAttributes attributes = getAttributes(componentName, result, componentId, componentType, component);

        components.emplace_back(TComponent{componentMapping.addComponent(componentId), componentType.getComponentId(),
                                           componentName, std::move(attributes)});
      } catch (const std::exception& ex) {
        result.addError(
          TError{m_Mode.adapt(TErrorLevel::ERR), fmt::format("component id={}: {}", componentId, ex.what())});
      }
    }

    return components;
  }

  inline TAttributes TJsonModelLoader::getAttributes(const std::string& context, TResult& result, uint64_t componentId,
                                                     const database::TComponent& componentType,
                                                     const json& component) const
  {
    TAttributes attributes;

    for (const auto& attribute : component["/attributes"_json_pointer]) {
      auto id = attribute["id"].get<std::string>();
      auto unit = attribute.value("unit", "");

      bool isCustom = m_LoaderHelper.checkCustom(result, context, id, componentId, componentType);

      if (!isCustom) {
        const auto& att = componentType.findAttributeById(id);
        auto value = m_LoaderHelper.getValue(result, att, context, id, componentId, attribute);
        attributes.emplace_back(TAttribute{att, TUnit{att.getUnit()}, value});
      } else {
        auto type = getValueType(attribute);
        auto value = m_LoaderHelper.getValue(result, type, context, id, componentId, attribute);
        attributes.emplace_back(TAttribute{id, TUnit{unit}, type, TValue{}});
      }
    }

    return attributes;
  }

  inline TRelations TJsonModelLoader::getRelations(TResult& result, ComponentMapping componentMapping,
                                                   const TComponents& components, const json& j) const
  {
    TRelations relations;
    std::set<uint64_t> usedComponents;
    for (const auto& relation : j["/model/relations"_json_pointer]) {
      auto relationId = relation["id"].get<uint64_t>();
      auto relationType = relationTypeFromString(relation["type"].get<std::string>());
      std::optional<uint32_t> order;
      if (relation.contains("order")) {
        order = relation["order"].get<uint32_t>();
        if (order < 1) {
          result.addError(
            TError{m_Mode.adapt(TErrorLevel::ERR), fmt::format("relation id={} order is <1", relationId)});
        }
      }

      TRelationReferences references;
      for (const auto& reference : relation["/refs"_json_pointer]) {
        auto referenceId = reference["id"].get<uint64_t>();
        auto hint = reference.value("hint", "");
        auto role = relationRoleFromString(reference["role"]);

        const auto* component = componentMapping.getComponent(referenceId, components);
        if (component == nullptr) {
          result.addError(
            TError{m_Mode.adapt(TErrorLevel::ERR),
                   fmt::format("relation id={} referenced component id={} does not exist", relationId, referenceId)});
        } else {
          usedComponents.emplace(component->getInternalId());
          references.emplace_back(TRelationReference{role, hint, *component});
        }
      }

      relations.emplace_back(TRelation{relationType, order, std::move(references)});
    }
    if (usedComponents.size() != components.size()) {
      result.addError(TError{TErrorLevel::WARN, fmt::format("{} components are not used in a relation",
                                                            components.size() - usedComponents.size())});
    }

    return relations;
  }

  inline TValueType TJsonModelLoader::getValueType(const json& attribute) const
  {
    for (const auto& [key, _] : attribute.items()) {
      if (key == "id" || key == "unit") {
        continue;
      }
      return typeFromString(key);
    }

    throw TException{"attribute does not have a type element"};
  }
}

#endif
