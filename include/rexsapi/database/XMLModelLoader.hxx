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

#ifndef REXSAPI_DATABASE_XML_MODEL_LOADER_HXX
#define REXSAPI_DATABASE_XML_MODEL_LOADER_HXX

#include <rexsapi/ConversionHelper.hxx>
#include <rexsapi/XSDSchemaValidator.hxx>
#include <rexsapi/XmlUtils.hxx>
#include <rexsapi/database/ComponentAttributeMapper.hxx>

#include <cstring>

/** @file */

namespace rexsapi::database
{
  /**
   * @brief Loads REXS database model xml files
   *
   * @tparam TResourceLoader Loader class for loading resources. Has to define the following method <br> ```TResult
   * load(const std::function<void(TResult&, std::vector<uint8_t>&)>& callback) const```
   *
   * @tparam TSchemaLoader
   */
  template<typename TResourceLoader, typename TSchemaLoader>
  class TXmlModelLoader
  {
  public:
    explicit TXmlModelLoader(const TResourceLoader& loader, const TSchemaLoader& schemaLoader)
    : m_Loader{loader}
    , m_SchemaLoader{schemaLoader}
    {
    }

    TResult load(const std::function<void(TModel)>& callback) const;

  private:
    std::optional<TInterval> readInterval(const pugi::xpath_node& node) const;

    const TResourceLoader& m_Loader;
    const TSchemaLoader& m_SchemaLoader;
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  template<typename TResourceLoader, typename TSchemaLoader>
  inline TResult
  TXmlModelLoader<TResourceLoader, TSchemaLoader>::load(const std::function<void(TModel)>& callback) const
  {
    return m_Loader.load([this, &callback](TResult& result, std::vector<uint8_t>& buffer) {
      pugi::xml_document doc = loadXMLDocument(result, buffer, rexsapi::xml::TXSDSchemaValidator{m_SchemaLoader});
      if (!result) {
        return;
      }

      const auto rexsModel = *doc.select_nodes("/rexsModel").begin();
      TModel model{TRexsVersion{xml::getStringAttribute(rexsModel, "version")},
                   xml::getStringAttribute(rexsModel, "language"), xml::getStringAttribute(rexsModel, "date"),
                   statusFromString(xml::getStringAttribute(rexsModel, "status"))};

      for (const auto& node : doc.select_nodes("/rexsModel/units/unit")) {
        auto id = convertToUint64(xml::getStringAttribute(node, "id"));
        auto name = xml::getStringAttribute(node, "name");
        model.addUnit(TUnit{id, name});
      }

      for (const auto& node : doc.select_nodes("/rexsModel/valueTypes/valueType")) {
        auto id = convertToUint64(xml::getStringAttribute(node, "id"));
        auto name = xml::getStringAttribute(node, "name");
        model.addType(id, typeFromString(name));
      }

      for (const auto& node : doc.select_nodes("/rexsModel/attributes/attribute")) {
        auto attributeId = xml::getStringAttribute(node, "attributeId");
        auto name = xml::getStringAttribute(node, "name");
        auto valueType = model.findValueTypeById(convertToUint64(xml::getStringAttribute(node, "valueType")));
        auto unit = convertToUint64(xml::getStringAttribute(node, "unit"));
        std::string symbol = xml::getStringAttribute(node, "symbol", "");

        std::optional<TInterval> interval = readInterval(node);

        std::optional<TEnumValues> enumValues;
        if (const auto& enums = node.node().first_child();
            (valueType == TValueType::ENUM || valueType == TValueType::ENUM_ARRAY) && !enums.empty() &&
            std::strncmp(enums.name(), "enumValues", ::strlen("enumValues")) == 0) {
          std::vector<TEnumValue> values;
          for (const auto& value : enums.children()) {
            auto enumValue = xml::getStringAttribute(value, "value");
            auto enumName = xml::getStringAttribute(value, "name");
            values.emplace_back(TEnumValue{enumValue, enumName});
          }
          enumValues = TEnumValues{std::move(values)};
        }

        model.addAttribute(
          TAttribute{attributeId, name, valueType, model.findUnitById(unit), symbol, interval, enumValues});
      }

      std::vector<std::pair<std::string, std::string>> attributeMappings;
      for (const auto& node : doc.select_nodes("/rexsModel/componentAttributeMappings/componentAttributeMapping")) {
        auto componentId = xml::getStringAttribute(node, "componentId");
        auto attributeId = xml::getStringAttribute(node, "attributeId");
        attributeMappings.emplace_back(componentId, attributeId);
      }
      detail::TComponentAttributeMapper attributeMapper{model, std::move(attributeMappings)};

      for (const auto& node : doc.select_nodes("/rexsModel/components/component")) {
        auto id = xml::getStringAttribute(node, "componentId");
        auto name = xml::getStringAttribute(node, "name");
        auto attributes = attributeMapper.getAttributesForComponent(id);
        model.addComponent(TComponent{id, name, std::move(attributes)});
      }

      callback(std::move(model));
    });
  }

  template<typename TResourceLoader, typename TSchemaLoader>
  std::optional<TInterval>
  TXmlModelLoader<TResourceLoader, TSchemaLoader>::readInterval(const pugi::xpath_node& node) const
  {
    std::optional<TInterval> interval;

    TIntervalEndpoint min;
    TIntervalEndpoint max;

    if (auto att = node.node().attribute("rangeMin"); !att.empty()) {
      auto open = xml::getBoolAttribute(node, "rangeMinIntervalOpen", true);
      min = TIntervalEndpoint{convertToDouble(att.value()), open ? TIntervalType::OPEN : TIntervalType::CLOSED};
    }
    if (auto att = node.node().attribute("rangeMax"); !att.empty()) {
      auto open = xml::getBoolAttribute(node, "rangeMaxIntervalOpen", true);
      max = TIntervalEndpoint{convertToDouble(att.value()), open ? TIntervalType::OPEN : TIntervalType::CLOSED};
    }

    if (max.isSet() || min.isSet()) {
      interval = TInterval{min, max};
    }

    return interval;
  }
}

#endif
