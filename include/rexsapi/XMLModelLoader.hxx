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

#ifndef REXSAPI_XML_MODEL_LOADER_HXX
#define REXSAPI_XML_MODEL_LOADER_HXX

#include <rexsapi/ConversionHelper.hxx>
#include <rexsapi/Mode.hxx>
#include <rexsapi/Model.hxx>
#include <rexsapi/ValidityChecker.hxx>
#include <rexsapi/XMLValueDecoder.hxx>
#include <rexsapi/XSDSchemaValidator.hxx>
#include <rexsapi/XmlUtils.hxx>
#include <rexsapi/database/ModelRegistry.hxx>

#include <set>

namespace rexsapi
{
  class TXMLModelLoader
  {
  public:
    explicit TXMLModelLoader(TMode mode, const xml::TXSDSchemaValidator& validator)
    : m_Validator{validator}
    , m_Mode{mode}
    {
    }

    std::optional<TModel> load(TResult& result, const database::TModelRegistry& registry,
                               std::vector<uint8_t>& buffer) const;

  private:
    const TComponent* getComponent(const std::string& referenceId, TComponents& components,
                                   const std::unordered_map<std::string, uint64_t>& componentsMapping) const&;

    TAttributes getAttributes(const std::string& context, TResult& result, const std::string& componentId,
                              const database::TComponent& componentType,
                              const pugi::xpath_node_set& attributeNodes) const;

    const xml::TXSDSchemaValidator& m_Validator;
    TXMLValueDecoder m_Decoder{};
    TModeAdapter m_Mode;
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline std::optional<TModel> TXMLModelLoader::load(TResult& result, const database::TModelRegistry& registry,
                                                     std::vector<uint8_t>& buffer) const
  {
    pugi::xml_document doc = loadXMLDocument(result, buffer, m_Validator);
    if (!result) {
      return {};
    }

    auto rexsModel = *doc.select_nodes("/model").begin();
    auto language = xml::getStringAttribute(rexsModel, "applicationLanguage", "");
    TModelInfo info{
      xml::getStringAttribute(rexsModel, "applicationId"), xml::getStringAttribute(rexsModel, "applicationVersion"),
      xml::getStringAttribute(rexsModel, "date"), TRexsVersion{xml::getStringAttribute(rexsModel, "version")},
      language.empty() ? std::optional<std::string>{} : language};

    // TODO (lcf): version should be configurable, maybe have something
    // like a sub-model-registry based on the language
    const auto& dbModel = registry.getModel(info.getVersion(), "en");
    uint64_t internalComponentId{0};
    std::unordered_map<std::string, uint64_t> componentsMapping;

    TComponents components;
    components.reserve(10);
    std::set<uint64_t> usedComponents;

    for (const auto& component : doc.select_nodes("/model/components/component")) {
      auto componentId = xml::getStringAttribute(component, "id");
      std::string componentName = xml::getStringAttribute(component, "name", "");
      try {
        const auto& componentType = dbModel.findComponentById(xml::getStringAttribute(component, "type"));

        auto attributeNodes =
          doc.select_nodes(fmt::format("/model/components/component[@id = '{}']/attribute", componentId).c_str());
        TAttributes attributes = getAttributes(componentName, result, componentId, componentType, attributeNodes);

        components.emplace_back(
          TComponent{++internalComponentId, componentType.getComponentId(), componentName, std::move(attributes)});
        componentsMapping.emplace(componentId, internalComponentId);
      } catch (const std::exception& ex) {
        result.addError(
          TError{m_Mode.adapt(TErrorLevel::ERR), fmt::format("component id={}: {}", componentId, ex.what())});
      }
    }

    TRelations relations;
    for (const auto& relation : doc.select_nodes("/model/relations/relation")) {
      std::string relationId = xml::getStringAttribute(relation, "id");
      auto relationType = relationTypeFromString(xml::getStringAttribute(relation, "type"));
      std::optional<uint32_t> order;
      if (auto orderAtt = relation.node().attribute("order"); !orderAtt.empty()) {
        order = orderAtt.as_uint();
        if (order.value() < 1) {
          result.addError(
            TError{m_Mode.adapt(TErrorLevel::ERR), fmt::format("relation id={} order is <1", relationId)});
        }
      }

      TRelationReferences references;
      for (const auto& reference :
           doc.select_nodes(fmt::format("/model/relations/relation[@id = '{}']/ref", relationId).c_str())) {
        std::string referenceId = xml::getStringAttribute(reference, "id");
        try {
          auto role = relationRoleFromString(xml::getStringAttribute(reference, "role"));
          std::string hint = xml::getStringAttribute(reference, "hint", "");

          const auto* component = getComponent(referenceId, components, componentsMapping);
          if (component == nullptr) {
            result.addError(
              TError{m_Mode.adapt(TErrorLevel::ERR),
                     fmt::format("relation id={} referenced component id={} does not exist", relationId, referenceId)});
          } else {
            usedComponents.emplace(component->getInternalId());
            references.emplace_back(TRelationReference{role, hint, *component});
          }
        } catch (const std::exception& ex) {
          result.addError(TError{m_Mode.adapt(TErrorLevel::ERR),
                                 fmt::format("cannot process reference id={}: {}", referenceId, ex.what())});
        }
      }

      relations.emplace_back(TRelation{relationType, order, std::move(references)});
    }
    if (usedComponents.size() != components.size()) {
      result.addError(TError{TErrorLevel::WARN, fmt::format("{} components are not used in a relation",
                                                            components.size() - usedComponents.size())});
    }

    TLoadCases loadCases;
    {
      for (const auto& loadCase : doc.select_nodes("/model/load_spectrum/load_case")) {
        std::string loadCaseId = xml::getStringAttribute(loadCase, "id");
        TLoadComponents loadComponents;

        for (const auto& component : doc.select_nodes(
               fmt::format("/model/load_spectrum/load_case[@id = '{}']/component", loadCaseId).c_str())) {
          std::string componentId = xml::getStringAttribute(component, "id");

          const auto* refComponent = getComponent(componentId, components, componentsMapping);
          if (refComponent == nullptr) {
            result.addError(
              TError{m_Mode.adapt(TErrorLevel::ERR),
                     fmt::format("load_case id={} component id={} does not exist", loadCaseId, componentId)});
            continue;
          }

          auto attributeNodes =
            doc.select_nodes(fmt::format("/model/load_spectrum/load_case[@id = '{}']/component[@id = '{}']/attribute",
                                         loadCaseId, componentId)
                               .c_str());
          auto context = fmt::format("load_case id={}", loadCaseId);
          TAttributes attributes = getAttributes(context, result, componentId,
                                                 dbModel.findComponentById(refComponent->getType()), attributeNodes);
          loadComponents.emplace_back(TLoadComponent(*refComponent, std::move(attributes)));
        }
        loadCases.emplace_back(std::move(loadComponents));
      }
    }

    return TModel{info, std::move(components), std::move(relations), TLoadSpectrum{std::move(loadCases)}};
  }

  inline const TComponent*
  TXMLModelLoader::getComponent(const std::string& referenceId, TComponents& components,
                                const std::unordered_map<std::string, uint64_t>& componentsMapping) const&
  {
    auto it = componentsMapping.find(referenceId);
    if (it == componentsMapping.end()) {
      return nullptr;
    }
    auto it_comp = std::find_if(components.begin(), components.end(), [&it](const auto& comp) {
      return comp.getInternalId() == it->second;
    });
    return it_comp.operator->();
  }

  inline TAttributes TXMLModelLoader::getAttributes(const std::string& context, TResult& result,
                                                    const std::string& componentId,
                                                    const database::TComponent& componentType,
                                                    const pugi::xpath_node_set& attributeNodes) const
  {
    TAttributes attributes;
    for (const auto& attribute : attributeNodes) {
      std::string id = xml::getStringAttribute(attribute, "id");

      bool isCustom{false};
      if (id.substr(0, 7) == "custom_") {
        isCustom = true;
      } else {
        if (!componentType.hasAttribute(id)) {
          isCustom = true;
          result.addError(
            TError{m_Mode.adapt(TErrorLevel::ERR),
                   fmt::format("{}: attribute id={} is not part of component id={}", context, id, componentId)});
        }
      }

      auto unit = xml::getStringAttribute(attribute, "unit");

      if (!isCustom) {
        const auto& att = componentType.findAttributeById(id);

        if (unit.empty()) {
          unit = att.getUnit().getName();
        } else {
          if (!att.getUnit().compare(unit)) {
            result.addError(
              TError{m_Mode.adapt(TErrorLevel::ERR),
                     fmt::format("{}: attribute id={} of component id={} does not specify the correct unit: '{}'",
                                 context, id, componentId, unit)});
          }
        }

        auto value = m_Decoder.decode(att.getValueType(), att.getEnums(), attribute.node());
        if (!value.second) {
          result.addError(
            TError{m_Mode.adapt(TErrorLevel::ERR),
                   fmt::format("{}: value of attribute id={} of component id={} does not have the correct value type",
                               context, id, componentId)});
          continue;
        }
        if (!TValidityChecker::check(att, value.first)) {
          result.addError(TError{
            m_Mode.adapt(TErrorLevel::ERR),
            fmt::format("{}: value is out of range for attribute id={} of component id={}", context, id, componentId)});
        }
        attributes.emplace_back(TAttribute{att, TUnit{att.getUnit()}, value.first});
      } else {
        auto [value, type] = m_Decoder.decodeUnknown(attribute.node());
        attributes.emplace_back(TAttribute{id, TUnit{unit}, type, std::move(value)});
      }
    }

    return attributes;
  }
}

#endif
