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

#include <rexsapi/Json.hxx>
#include <rexsapi/JsonSchemaValidator.hxx>
#include <rexsapi/JsonValueDecoder.hxx>
#include <rexsapi/ModelHelper.hxx>
#include <rexsapi/database/ModelRegistry.hxx>

#include <set>

namespace rexsapi
{
  class TJsonModelLoader
  {
  public:
    explicit TJsonModelLoader(TMode mode, const TJsonSchemaValidator& validator)
    : m_Mode{mode}
    , m_LoaderHelper{mode}
    , m_Validator{validator}
    {
    }

    std::optional<TModel> load(TResult& result, const database::TModelRegistry& registry,
                               std::vector<uint8_t>& buffer) const;

  private:
    TComponents getComponents(TResult& result, ComponentMapping& componentMapping, const database::TModel& dbModel,
                              const json& j) const;

    TAttributes getAttributes(std::string_view context, TResult& result, uint64_t componentId,
                              const database::TComponent& componentType, const json& component) const;

    TRelations getRelations(TResult& result, const ComponentMapping& componentMapping, const TComponents& components,
                            const json& j) const;

    TLoadCases getLoadCases(TResult& result, const ComponentMapping& componentMapping, const TComponents& components,
                            const database::TModel& dbModel, const json& j) const;

    std::optional<TAccumulation> getAccumulation(TResult& result, const ComponentMapping& componentMapping,
                                                 const TComponents& components, const database::TModel& dbModel,
                                                 const json& j) const;

    static TValueType getValueType(const json& attribute);

    TModeAdapter m_Mode;
    TModelHelper<TJsonValueDecoder> m_LoaderHelper;
    const TJsonSchemaValidator& m_Validator;
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline std::optional<TModel> TJsonModelLoader::load(TResult& result, const database::TModelRegistry& registry,
                                                      std::vector<uint8_t>& buffer) const
  {
    try {
      const json j = json::parse(buffer);
      std::vector<std::string> errors;
      if (!m_Validator.validate(j, errors)) {
        for (const auto& error : errors) {
          result.addError(TError{TErrorLevel::CRIT, error});
        }
        return {};
      }

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
      TComponents components = getComponents(result, componentMapping, dbModel, j);
      TRelations relations = getRelations(result, componentMapping, components, j);
      TLoadCases loadCases = getLoadCases(result, componentMapping, components, dbModel, j);
      std::optional<TAccumulation> accumulation = getAccumulation(result, componentMapping, components, dbModel, j);

      return TModel{info, std::move(components), std::move(relations),
                    TLoadSpectrum{std::move(loadCases), std::move(accumulation)}};
    } catch (const json::exception& ex) {
      result.addError(TError{TErrorLevel::CRIT, fmt::format("cannot parse json document: {}", ex.what())});
    }
    return {};
  }

  inline TComponents TJsonModelLoader::getComponents(TResult& result, ComponentMapping& componentMapping,
                                                     const database::TModel& dbModel, const json& j) const
  {
    TComponents components;

    for (const auto& component : j["/model/components"_json_pointer]) {
      auto componentId = component["id"].get<uint64_t>();
      std::string componentName = component.value("name", "");
      try {
        const auto& componentType = dbModel.findComponentById(component["type"].get<std::string>());
        std::string context = componentName.empty() ? componentType.getName() : componentName;
        TAttributes attributes = getAttributes(context, result, componentId, componentType, component);

        components.emplace_back(TComponent{componentMapping.addComponent(componentId), componentType.getComponentId(),
                                           componentName, std::move(attributes)});
      } catch (const std::exception& ex) {
        result.addError(
          TError{m_Mode.adapt(TErrorLevel::ERR), fmt::format("component id={}: {}", componentId, ex.what())});
      }
    }
    ComponentPostProcessor postProcessor{result, m_Mode, components, componentMapping};
    return postProcessor.release();
  }

  inline TAttributes TJsonModelLoader::getAttributes(std::string_view context, TResult& result, uint64_t componentId,
                                                     const database::TComponent& componentType,
                                                     const json& component) const
  {
    TAttributes attributes;

    for (const auto& attribute : component["/attributes"_json_pointer]) {
      auto id = attribute["id"].get<std::string>();
      auto unit = attribute.value("unit", "");

      bool isCustom = m_LoaderHelper.checkCustom(result, context, id, componentId, componentType);
      auto type = getValueType(attribute);

      if (!isCustom) {
        const auto& att = componentType.findAttributeById(id);
        if (!unit.empty() && TUnit{unit} != att.getUnit()) {
          result.addError(
            TError{m_Mode.adapt(TErrorLevel::WARN),
                   fmt::format("{}: specified incorrect unit ({}) for attribute id={}", context, unit, id)});
        }
        TValue value;
        if (type != att.getValueType()) {
          result.addError(
            TError{m_Mode.adapt(TErrorLevel::WARN), fmt::format("{}: specified incorrect type ({}) for attribute id={}",
                                                                context, toTypeString(type), id)});
        } else {
          value = m_LoaderHelper.getValue(result, context, id, componentId, att, attribute);
        }
        attributes.emplace_back(TAttribute{att, TUnit{att.getUnit()}, value});
      } else {
        auto value = m_LoaderHelper.getValue(result, type, context, id, componentId, attribute);
        attributes.emplace_back(TAttribute{id, TUnit{unit}, type, value});
      }
    }

    return attributes;
  }

  inline TRelations TJsonModelLoader::getRelations(TResult& result, const ComponentMapping& componentMapping,
                                                   const TComponents& components, const json& j) const
  {
    TRelations relations;
    std::set<uint64_t> usedComponents;
    for (const auto& relation : j["/model/relations"_json_pointer]) {
      auto relationId = relation["id"].get<uint64_t>();
      try {
        auto relationType = relationTypeFromString(relation["type"].get<std::string>());
        std::optional<uint32_t> order;
        if (relation.contains("order")) {
          order = relation["order"].get<uint32_t>();
        }

        TRelationReferences references;
        for (const auto& reference : relation["/refs"_json_pointer]) {
          auto referenceId = reference["id"].get<uint64_t>();
          try {
            auto hint = reference.value("hint", "");
            auto role = relationRoleFromString(reference["role"]);

            const auto* component = componentMapping.getComponent(referenceId, components);
            if (component == nullptr) {
              result.addError(TError{
                m_Mode.adapt(TErrorLevel::ERR),
                fmt::format("relation id={} referenced component id={} does not exist", relationId, referenceId)});
            } else {
              usedComponents.emplace(component->getInternalId());
              references.emplace_back(TRelationReference{role, hint, *component});
            }
          } catch (const std::exception& ex) {
            result.addError(
              TError{m_Mode.adapt(TErrorLevel::ERR), fmt::format("relation id={} cannot process reference id={}: {}",
                                                                 relationId, referenceId, ex.what())});
          }
        }

        relations.emplace_back(TRelation{relationType, order, std::move(references)});
      } catch (const std::exception& ex) {
        result.addError(
          TError{m_Mode.adapt(TErrorLevel::ERR), fmt::format("realtion id={}: {}", relationId, ex.what())});
      }
    }
    if (usedComponents.size() != components.size()) {
      result.addError(TError{TErrorLevel::WARN, fmt::format("{} components are not used in a relation",
                                                            components.size() - usedComponents.size())});
    }

    return relations;
  }

  inline TLoadCases TJsonModelLoader::getLoadCases(TResult& result, const ComponentMapping& componentMapping,
                                                   const TComponents& components, const database::TModel& dbModel,
                                                   const json& j) const
  {
    TLoadCases loadCases;
    if (!j.contains("/model/load_spectrum/load_cases"_json_pointer)) {
      return loadCases;
    }

    for (const auto& loadCase : j["/model/load_spectrum/load_cases"_json_pointer]) {
      auto loadCaseId = loadCase["id"].get<uint64_t>();
      TLoadComponents loadComponents;

      for (const auto& componentRef : loadCase["/components"_json_pointer]) {
        auto componentId = componentRef["id"].get<uint64_t>();
        try {
          const auto* component = componentMapping.getComponent(componentId, components);
          if (component == nullptr) {
            result.addError(
              TError{m_Mode.adapt(TErrorLevel::ERR),
                     fmt::format("load_case id={} component id={} does not exist", loadCaseId, componentId)});
            continue;
          }
          const auto context = fmt::format("load_case id={}", loadCaseId);
          TAttributes attributes =
            getAttributes(context, result, componentId, dbModel.findComponentById(component->getType()), componentRef);
          loadComponents.emplace_back(TLoadComponent(*component, std::move(attributes)));
        } catch (const std::exception& ex) {
          result.addError(TError{m_Mode.adapt(TErrorLevel::ERR), fmt::format("load_case id={} component id={}: {}",
                                                                             loadCaseId, componentId, ex.what())});
        }
      }
      loadCases.emplace_back(std::move(loadComponents));
    }

    return loadCases;
  }

  inline std::optional<TAccumulation>
  TJsonModelLoader::getAccumulation(TResult& result, const ComponentMapping& componentMapping,
                                    const TComponents& components, const database::TModel& dbModel, const json& j) const
  {
    if (!j.contains("/model/load_spectrum/accumulation"_json_pointer)) {
      return std::optional<TAccumulation>{};
    }

    TLoadComponents loadComponents;
    for (const auto& componentRef : j["/model/load_spectrum/accumulation/components"_json_pointer]) {
      auto componentId = componentRef["id"].get<uint64_t>();
      try {
        const auto* component = componentMapping.getComponent(componentId, components);
        if (component == nullptr) {
          result.addError(TError{m_Mode.adapt(TErrorLevel::ERR),
                                 fmt::format("accumulation component id={} does not exist", componentId)});
          continue;
        }
        TAttributes attributes = getAttributes("accumulation", result, componentId,
                                               dbModel.findComponentById(component->getType()), componentRef);
        loadComponents.emplace_back(TLoadComponent(*component, std::move(attributes)));
      } catch (const std::exception& ex) {
        result.addError(TError{m_Mode.adapt(TErrorLevel::ERR),
                               fmt::format("accumulation component id={}: {}", componentId, ex.what())});
      }
    }

    return TAccumulation{std::move(loadComponents)};
  }

  inline TValueType TJsonModelLoader::getValueType(const json& attribute)
  {
    for (const auto& [key, _] : attribute.items()) {
      if (key == "id" || key == "unit") {
        continue;
      }
      if (key == "floating_point_array_coded") {
        return TValueType::FLOATING_POINT_ARRAY;
      }
      if (key == "integer_array_coded") {
        return TValueType::INTEGER_ARRAY;
      }
      if (key == "floating_point_matrix_coded") {
        return TValueType::FLOATING_POINT_MATRIX;
      }

      return typeFromString(key);
    }

    throw TException{"attribute does not have a type element"};
  }
}

#endif
