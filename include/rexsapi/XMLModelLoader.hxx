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
#include <rexsapi/ModelHelper.hxx>
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
    : m_Mode{mode}
    , m_Validator{validator}
    , m_LoaderHelper{mode}
    {
    }

    std::optional<TModel> load(TResult& result, const database::TModelRegistry& registry,
                               std::vector<uint8_t>& buffer) const;

  private:
    TAttributes getAttributes(const std::string& context, TResult& result, const std::string& componentId,
                              const database::TComponent& componentType,
                              const pugi::xpath_node_set& attributeNodes) const;

    TModeAdapter m_Mode;
    const xml::TXSDSchemaValidator& m_Validator;
    TModelHelper<TXMLValueDecoder> m_LoaderHelper;
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline std::optional<TModel> TXMLModelLoader::load(TResult& result, const database::TModelRegistry& registry,
                                                     std::vector<uint8_t>& buffer) const
  {
    const pugi::xml_document doc = loadXMLDocument(result, buffer, m_Validator);
    if (!result) {
      return {};
    }

    const auto rexsModel = *doc.select_nodes("/model").begin();
    auto language = xml::getStringAttribute(rexsModel, "applicationLanguage", "");
    TModelInfo info{
      xml::getStringAttribute(rexsModel, "applicationId"), xml::getStringAttribute(rexsModel, "applicationVersion"),
      xml::getStringAttribute(rexsModel, "date"), TRexsVersion{xml::getStringAttribute(rexsModel, "version")},
      language.empty() ? std::optional<std::string>{} : language};

    // TODO (lcf): version should be configurable, maybe have something
    // like a sub-model-registry based on the language
    const auto& dbModel = registry.getModel(info.getVersion(), "en");
    ComponentMapping componentsMapping;

    TComponents components;
    components.reserve(10);
    std::set<uint64_t> usedComponents;

    for (const auto& component : doc.select_nodes("/model/components/component")) {
      auto componentId = xml::getStringAttribute(component, "id");
      std::string componentName = xml::getStringAttribute(component, "name", "");
      try {
        const auto& componentType = dbModel.findComponentById(xml::getStringAttribute(component, "type"));

        const auto attributeNodes =
          doc.select_nodes(fmt::format("/model/components/component[@id = '{}']/attribute", componentId).c_str());
        TAttributes attributes = getAttributes(componentName, result, componentId, componentType, attributeNodes);

        components.emplace_back(TComponent{componentsMapping.addComponent(convertToUint64(componentId)),
                                           componentType.getComponentId(), componentName, std::move(attributes)});
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
      if (const auto orderAtt = relation.node().attribute("order"); !orderAtt.empty()) {
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

          const auto* component = componentsMapping.getComponent(convertToUint64(referenceId), components);
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

          const auto* refComponent = componentsMapping.getComponent(convertToUint64(componentId), components);
          if (refComponent == nullptr) {
            result.addError(
              TError{m_Mode.adapt(TErrorLevel::ERR),
                     fmt::format("load_case id={} component id={} does not exist", loadCaseId, componentId)});
            continue;
          }

          const auto attributeNodes =
            doc.select_nodes(fmt::format("/model/load_spectrum/load_case[@id = '{}']/component[@id = '{}']/attribute",
                                         loadCaseId, componentId)
                               .c_str());
          const auto context = fmt::format("load_case id={}", loadCaseId);
          TAttributes attributes = getAttributes(context, result, componentId,
                                                 dbModel.findComponentById(refComponent->getType()), attributeNodes);
          loadComponents.emplace_back(TLoadComponent(*refComponent, std::move(attributes)));
        }
        loadCases.emplace_back(std::move(loadComponents));
      }
    }
    std::optional<TAccumulation> accumulation;
    {
      TLoadComponents loadComponents;
      for (const auto& component : doc.select_nodes("/model/load_spectrum/accumulation/component")) {
        std::string componentId = xml::getStringAttribute(component, "id");

        const auto* refComponent = componentsMapping.getComponent(convertToUint64(componentId), components);
        if (refComponent == nullptr) {
          result.addError(TError{m_Mode.adapt(TErrorLevel::ERR),
                                 fmt::format("accumulation component id={} does not exist", componentId)});
          continue;
        }

        const auto attributeNodes = doc.select_nodes(
          fmt::format("/model/load_spectrum/accumulation/component[@id = '{}']/attribute", componentId).c_str());
        TAttributes attributes = getAttributes("accumulation", result, componentId,
                                               dbModel.findComponentById(refComponent->getType()), attributeNodes);
        loadComponents.emplace_back(TLoadComponent(*refComponent, std::move(attributes)));
      }
      if (!loadComponents.empty()) {
        accumulation = TAccumulation{std::move(loadComponents)};
      }
    }

    return TModel{info, std::move(components), std::move(relations),
                  TLoadSpectrum{std::move(loadCases), std::move(accumulation)}};
  }

  inline TAttributes TXMLModelLoader::getAttributes(const std::string& context, TResult& result,
                                                    const std::string& componentId,
                                                    const database::TComponent& componentType,
                                                    const pugi::xpath_node_set& attributeNodes) const
  {
    TAttributes attributes;
    for (const auto& attribute : attributeNodes) {
      std::string id = xml::getStringAttribute(attribute, "id");
      auto unit = xml::getStringAttribute(attribute, "unit");

      bool isCustom = m_LoaderHelper.checkCustom(result, context, id, convertToUint64(componentId), componentType);

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

        auto value = m_LoaderHelper.getValue(result, att, context, id, convertToUint64(componentId), attribute.node());
        attributes.emplace_back(TAttribute{att, TUnit{att.getUnit()}, value});
      } else {
        auto [value, type] = m_LoaderHelper.getDecoder().decodeUnknown(attribute.node());
        attributes.emplace_back(TAttribute{id, TUnit{unit}, type, std::move(value)});
      }
    }

    return attributes;
  }
}

#endif
