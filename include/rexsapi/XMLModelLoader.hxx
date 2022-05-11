
#ifndef REXSAPI_XML_MODEL_LOADER_HXX
#define REXSAPI_XML_MODEL_LOADER_HXX

#include <rexsapi/ConversionHelper.hxx>
#include <rexsapi/LoaderResult.hxx>
#include <rexsapi/Model.hxx>
#include <rexsapi/XMLParser.hxx>
#include <rexsapi/XMLValueDecoder.hxx>
#include <rexsapi/database/ModelRegistry.hxx>
#include <rexsapi/xml/XSDSchemaValidator.hxx>

namespace rexsapi
{
  class TXMLModelLoader
  {
  public:
    explicit TXMLModelLoader(const xml::TXSDSchemaValidator& validator)
    : m_Validator{validator}
    {
    }

    std::optional<TModel> load(TLoaderResult& result, const rexsapi::database::TModelRegistry& registry,
                               std::vector<uint8_t>& buffer) const;

  private:
    const xml::TXSDSchemaValidator& m_Validator;
    rexsapi::TXMLValueDecoder m_Decoder{};
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline std::optional<TModel> TXMLModelLoader::load(TLoaderResult& result,
                                                     const rexsapi::database::TModelRegistry& registry,
                                                     std::vector<uint8_t>& buffer) const
  {
    // TODO (lcf): extract to helper class
    pugi::xml_document doc;
    if (pugi::xml_parse_result parseResult = doc.load_buffer_inplace(buffer.data(), buffer.size()); !parseResult) {
      result.addError(TResourceError{parseResult.description(), parseResult.offset});
      return {};
    }

    {
      std::vector<std::string> errors;
      if (!m_Validator.validate(doc, errors)) {
        // TODO (lcf): errors should be added to the exception, or even better, to the result
        throw TException{"cannot validate db model file"};
      }
    }

    auto rexsModel = *doc.select_nodes("/model").begin();
    TModelInfo info{getStringAttribute(rexsModel, "applicationId"), getStringAttribute(rexsModel, "applicationVersion"),
                    getStringAttribute(rexsModel, "date"), TRexsVersion{getStringAttribute(rexsModel, "version")}};

    // TODO (lcf): version should be configurable, maybe have something
    // like a sub-model-registry based on the language
    const auto& dbModel = registry.getModel(info.getVersion(), "en");

    TComponents components;
    for (const auto& component : doc.select_nodes("/model/components/component")) {
      auto componentId = getStringAttribute(component, "id");
      std::string componentName = getStringAttribute(component, "name", "");
      const auto& componentType = dbModel.findComponentById(getStringAttribute(component, "type"));

      TAttributes attributes;
      for (const auto& attribute :
           doc.select_nodes(fmt::format("/model/components/component[@id = '{}']/attribute", componentId).c_str())) {
        std::string id = getStringAttribute(attribute, "id");
        std::string unit = getStringAttribute(attribute, "unit", "none");
        const auto& att = componentType.findAttributeById(id);
        if (!att.getUnit().compare(unit)) {
          result.addError(TResourceError{fmt::format(
            "attribute '{}' of component '{}' does specify the correct unit: '{}'", id, componentId, unit)});
          continue;
        }
        auto value = m_Decoder.decode(att.getValueType(), att.getEnums(), attribute.node());
        if (!value.second) {
          result.addError(TResourceError{fmt::format(
            "value of attribute '{}' of component '{}' does not have the correct value type", id, componentId)});
          continue;
        }

        // TODO (lcf): custom units for custom attributes
        attributes.emplace_back(TAttribute{att, TUnit{dbModel.findUnitByName(unit)}, value.first});
      }

      components.emplace_back(TComponent{componentId, componentName, std::move(attributes)});
    }

    TRelations relations;
    for (const auto& relation : doc.select_nodes("/model/relations/relation")) {
      std::string relationId = getStringAttribute(relation, "id");
      auto relationType = relationTypeFromString(getStringAttribute(relation, "type"));
      std::optional<uint32_t> order;
      if (auto orderAtt = relation.node().attribute("order"); !orderAtt.empty()) {
        order = orderAtt.as_uint();
        if (order.value() < 1) {
          result.addError(TResourceError{fmt::format("relation id={} order is <1", relationId)});
        }
      }

      TRelationReferences references;
      for (const auto& reference :
           doc.select_nodes(fmt::format("/model/relations/relation[@id = '{}']/ref", relationId).c_str())) {
        std::string referenceId = getStringAttribute(reference, "id");
        auto role = relationRoleFromString(getStringAttribute(reference, "role"));

        auto it = std::find_if(components.begin(), components.end(), [&referenceId](const auto& component) {
          return component.getId() == referenceId;
        });
        if (it == components.end()) {
          result.addError(TResourceError{
            fmt::format("relation id={} referenced component id={} does not exist", relationId, referenceId)});
        } else {
          references.emplace_back(TRelationReference{role, *it});
        }
      }

      relations.emplace_back(TRelation{relationType, order, std::move(references)});
    }

    // TODO (lcf): check that all components are used in at least one relation

    return TModel{info, std::move(components), std::move(relations)};
  }
}

#endif
