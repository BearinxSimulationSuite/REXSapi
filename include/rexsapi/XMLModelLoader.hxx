
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
    TModelInfo info{getAttribute(rexsModel, "applicationId"), getAttribute(rexsModel, "applicationVersion"),
                    getAttribute(rexsModel, "date"), getAttribute(rexsModel, "version")};

    // TODO (lcf): version should be configurable, maybe have something
    // like a sub-model-registry based on the language
    const auto& dbModel = registry.getModel(info.getVersion(), "en");

    TComponents components;
    for (const auto& component : doc.select_nodes("/model/components/component")) {
      auto componentId = getAttribute(component, "id");
      std::string componentName = getAttribute(component, "name", "");
      const auto& componentType = dbModel.findComponentById(getAttribute(component, "type"));

      TAttributes attributes;
      for (const auto& attribute :
           doc.select_nodes(fmt::format("/model/components/component[@id = '{}']/attribute", componentId).c_str())) {
        std::string id = getAttribute(attribute, "id");
        std::string unit = getAttribute(attribute, "unit", "none");
        const auto& att = componentType.findAttributeById(id);
        if (!att.getUnit().compare(unit)) {
          result.addError(TResourceError{fmt::format(
            "attribute '{}' of component '{}' does specify the correct unit: '{}'", id, componentId, unit)});
          continue;
        }
        if (!m_Decoder.decode(att.getValueType(), att.getEnums(), attribute.node())) {
          result.addError(TResourceError{
            fmt::format("attribute '{}' of component '{}' does specify the correct value", id, component)});
          continue;
        }

        // TODO (lcf): check unit with attribute unit
        // TODO (lcf): custom units for custom attributes
        attributes.emplace_back(TAttribute{att, TUnit{dbModel.findUnitByName(unit)}, TValue{""}});
      }

      components.emplace_back(TComponent{componentId, componentName, std::move(attributes)});
    }

    TRelations relations;
    for (const auto& relation : doc.select_nodes("/model/relations/relation")) {
      std::string relationId = getAttribute(relation, "id");
      auto relationType = relationTypeFromString(getAttribute(relation, "type"));

      TRelationReferences references;
      for (const auto& reference :
           doc.select_nodes(fmt::format("/model/relations/relation[@id = '{}']/ref", relationId).c_str())) {
        std::string referenceId = getAttribute(reference, "id");
        auto role = relationRoleFromString(getAttribute(reference, "role"));

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

      relations.emplace_back(TRelation{relationType, std::move(references)});
    }

    // TODO (lcf): check that all components are used in at least one relation

    return TModel{info, std::move(components), std::move(relations)};
  }
}

#endif
