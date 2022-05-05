
#ifndef REXSAPI_DATABASE_XML_MODEL_LOADER_HXX
#define REXSAPI_DATABASE_XML_MODEL_LOADER_HXX

#include <rexsapi/ConversionHelper.hxx>
#include <rexsapi/LoaderResult.hxx>
#include <rexsapi/XMLParser.hxx>
#include <rexsapi/database/ComponentAttributeMapper.hxx>
#include <rexsapi/xml/XSDSchemaValidator.hxx>

#include <cstring>

namespace rexsapi::database
{
  template<typename TResourceLoader, typename TSchemaLoader>
  class TXmlModelLoader
  {
  public:
    explicit TXmlModelLoader(const TResourceLoader& loader, const TSchemaLoader& schemaLoader)
    : m_Loader{loader}
    , m_SchemaLoader{schemaLoader}
    {
    }

    TLoaderResult load(const std::function<void(TModel)>& callback) const;

    const TResourceLoader& m_Loader;
    const TSchemaLoader& m_SchemaLoader;
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  template<typename TResourceLoader, typename TSchemaLoader>
  inline TLoaderResult
  TXmlModelLoader<TResourceLoader, TSchemaLoader>::load(const std::function<void(TModel)>& callback) const
  {
    return m_Loader.load([this, &callback](TLoaderResult& result, std::vector<uint8_t>& buffer) -> void {
      pugi::xml_document doc;
      if (pugi::xml_parse_result parseResult = doc.load_buffer_inplace(buffer.data(), buffer.size()); !parseResult) {
        result.addError(TResourceError{parseResult.description(), parseResult.offset});
        return;
      }

      {
        std::vector<std::string> errors;
        if (!rexsapi::xml::TSchemaValidator{m_SchemaLoader}.validate(doc, errors)) {
          // TODO (lcf): errors should be added to the exception, or even better, to the result
          throw TException{"cannot validate db model file"};
        }
      }

      auto rexsModel = *doc.select_nodes("/rexsModel").begin();
      TModel model{rexsModel.node().attribute("version").value(), rexsModel.node().attribute("language").value(),
                   rexsModel.node().attribute("date").value(),
                   statusFromString(rexsModel.node().attribute("status").value())};

      for (const auto& node : doc.select_nodes("/rexsModel/units/unit")) {
        auto id = convertToUint64(node.node().attribute("id").value());
        const auto* name = node.node().attribute("name").value();
        model.addUnit(Unit{id, name});
      }

      for (const auto& node : doc.select_nodes("/rexsModel/valueTypes/valueType")) {
        auto id = convertToUint64(node.node().attribute("id").value());
        const auto* name = node.node().attribute("name").value();
        model.addType(id, typeFromString(name));
      }

      for (const auto& node : doc.select_nodes("/rexsModel/attributes/attribute")) {
        const auto* attributeId = node.node().attribute("attributeId").value();
        const auto* name = node.node().attribute("name").value();
        auto valueType = model.findValueTypeById(convertToUint64(node.node().attribute("valueType").value()));
        auto unit = convertToUint64(node.node().attribute("unit").value());
        std::string symbol;
        if (const auto& symNode = node.node().attribute("symbol"); !symNode.empty()) {
          symbol = symNode.value();
        }
        // TODO (lcf): get interval
        std::optional<TInterval> interval;

        std::optional<TEnumValues> enumValues;
        if (valueType == TValueType::ENUM || valueType == TValueType::ENUM_ARRAY) {
          if (const auto& enums = node.node().first_child();
              !enums.empty() && std::strncmp(enums.name(), "enumValues", 10) == 0) {
            std::vector<TEnumValue> values;
            for (const auto& value : enums.children()) {
              const auto* enumValue = value.attribute("value").value();
              const auto* enumName = value.attribute("name").value();
              values.emplace_back(TEnumValue{enumValue, enumName});
            }
            enumValues = TEnumValues{std::move(values)};
          }
        }

        model.addAttribute(
          TAttribute{attributeId, name, valueType, model.findUnitById(unit), symbol, interval, enumValues});
      }

      std::vector<std::pair<std::string, std::string>> attributeMappings;
      for (const auto& node : doc.select_nodes("/rexsModel/componentAttributeMappings/componentAttributeMapping")) {
        const auto* componentId = node.node().attribute("componentId").value();
        const auto* attributeId = node.node().attribute("attributeId").value();
        attributeMappings.emplace_back(componentId, attributeId);
      }
      TComponentAttributeMapper attributeMapper{model, std::move(attributeMappings)};

      for (const auto& node : doc.select_nodes("/rexsModel/components/component")) {
        const auto* id = node.node().attribute("componentId").value();
        const auto* name = node.node().attribute("name").value();
        auto attributes = attributeMapper.getAttributesForComponent(id);
        model.addComponent(TComponent{id, name, std::move(attributes)});
      }

      callback(std::move(model));
    });
  }

}

#endif
