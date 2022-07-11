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

#ifndef REXSAPI_JSON_MODEL_SERIALIZER_HXX
#define REXSAPI_JSON_MODEL_SERIALIZER_HXX

#include <rexsapi/CodedValue.hxx>
#include <rexsapi/Json.hxx>
#include <rexsapi/Model.hxx>

#include <iostream>

namespace rexsapi
{
  class JsonModelSerializer
  {
  public:
    template<typename TSerializer>
    void serialize(const TModel& model, TSerializer& serializer);

  private:
    void serialize(ordered_json& model, const TModelInfo& info) const;
    void serialize(ordered_json& model, const TComponents& components);
    void serialize(ordered_json& component, const TAttributes& attributes);
    void serialize(ordered_json& attributeNode, const TAttribute& attribute);
    void serialize(ordered_json& model, const TRelations& relations);
    void serialize(ordered_json& model, const TLoadSpectrum& spectrum);

    uint64_t getNextComponentId()
    {
      return ++m_ComponentId;
    }

    uint64_t getNextRelationId()
    {
      return ++m_RelationId;
    }

    uint64_t getComponentId(uint64_t internalId) const;

    ordered_json m_Doc;
    uint64_t m_ComponentId{0};
    uint64_t m_RelationId{0};
    std::unordered_map<uint64_t, uint64_t> m_ComponentMapping;
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  template<typename TSerializer>
  inline void JsonModelSerializer::serialize(const TModel& model, TSerializer& serializer)
  {
    m_Doc.clear();

    ordered_json models;
    serialize(models, model.getInfo());
    models["relations"] = json::array();
    models["components"] = json::array();
    serialize(models, model.getComponents());
    serialize(models, model.getRelations());
    if (model.getLoadSpectrum().hasLoadCases()) {
      serialize(models, model.getLoadSpectrum());
    }
    m_Doc["model"] = std::move(models);

    serializer.serialize(m_Doc);
  }

  inline void JsonModelSerializer::serialize(ordered_json& model, const TModelInfo& info) const
  {
    model["applicationId"] = info.getApplicationId();
    model["applicationVersion"] = info.getApplicationVersion();
    model["date"] = info.getDate();
    model["version"] = info.getVersion().asString();
    if (info.getApplicationLanguage().has_value()) {
      model["applicationLanguage"] = *info.getApplicationLanguage();
    }
  }

  inline void JsonModelSerializer::serialize(ordered_json& model, const TComponents& components)
  {
    auto& componentsNode = model["components"];

    for (const auto& component : components) {
      ordered_json componentNode;
      auto id = getNextComponentId();
      m_ComponentMapping.emplace(component.getInternalId(), id);
      componentNode["id"] = id;
      componentNode["type"] = component.getType();
      componentNode["name"] = component.getName();

      serialize(componentNode, component.getAttributes());

      componentsNode.emplace_back(std::move(componentNode));
    }
  }

  inline void JsonModelSerializer::serialize(ordered_json& componentNode, const TAttributes& attributes)
  {
    componentNode["attributes"] = json::array();
    auto& attributesNode = componentNode["attributes"];

    for (const auto& attribute : attributes) {
      ordered_json attributeNode;
      attributeNode["id"] = attribute.getAttributeId();
      attributeNode["unit"] = attribute.getUnit().getName();
      serialize(attributeNode, attribute);

      attributesNode.emplace_back(std::move(attributeNode));
    }
  }

  template<typename T>
  inline void encodeCodedArray(ordered_json& j, TCodeType type, const std::vector<T>& array)
  {
    if (type != TCodeType::None) {
      j = json::object();
      auto [val, code] = detail::encodeArray(array, type);
      j["code"] = detail::toCodedValueString(code);
      j["value"] = std::move(val);
    } else {
      j = json::array();
      for (const auto& element : array) {
        j.emplace_back(element);
      }
    }
  }

  template<typename T>
  inline void encodeCodedMatrix(ordered_json& j, TCodeType type, const TMatrix<T>& matrix)
  {
    if (type != TCodeType::None) {
      j = json::object();
      auto [val, code] = detail::encodeMatrix(matrix, type);
      j["code"] = detail::toCodedValueString(code);
      j["rows"] = matrix.m_Values.size();
      j["columns"] = matrix.m_Values.size();
      j["value"] = std::move(val);
    } else {
      j = json::array();
      for (const auto& row : matrix.m_Values) {
        auto columns = json::array();
        for (const auto& column : row) {
          columns.emplace_back(column);
        }
        j.emplace_back(std::move(columns));
      }
    }
  }

  inline void JsonModelSerializer::serialize(ordered_json& attributeNode, const TAttribute& attribute)
  {
    attributeNode[toTypeString(attribute.getValueType())] = nullptr;
    auto typeName = toTypeString(attribute.getValueType());
    if (attribute.getValue().coded() != TCodeType::None) {
      typeName += "_coded";
    }
    auto& j = attributeNode[typeName];
    if (attribute.getValue().isEmpty()) {
      return;
    }

    rexsapi::dispatch<void>(attribute.getValueType(), attribute.getValue(),
                            {[&j](rexsapi::FloatTag, const auto& d) -> void {
                               j = d;
                             },
                             [&j](rexsapi::BoolTag, const auto& b) -> void {
                               j = b;
                             },
                             [&j](rexsapi::IntTag, const auto& i) -> void {
                               j = i;
                             },
                             [&j](rexsapi::EnumTag, const auto& s) -> void {
                               j = s;
                             },
                             [&j](rexsapi::StringTag, const auto& s) -> void {
                               j = s;
                             },
                             [&j](rexsapi::FileReferenceTag, const auto& s) -> void {
                               j = s;
                             },
                             [&j, &attribute](rexsapi::FloatArrayTag, const auto& a) -> void {
                               encodeCodedArray(j, attribute.getValue().coded(), a);
                             },
                             [&j](rexsapi::BoolArrayTag, const auto& a) -> void {
                               j = json::array();
                               for (const auto& element : a) {
                                 j.emplace_back(*element);
                               }
                             },
                             [&j, &attribute](rexsapi::IntArrayTag, const auto& a) -> void {
                               encodeCodedArray(j, attribute.getValue().coded(), a);
                             },
                             [&j](rexsapi::EnumArrayTag, const auto& a) -> void {
                               j = json::array();
                               for (const auto& element : a) {
                                 j.emplace_back(element);
                               }
                             },
                             [&j](rexsapi::StringArrayTag, const auto& a) -> void {
                               j = json::array();
                               for (const auto& element : a) {
                                 j.emplace_back(element);
                               }
                             },
                             [&j](rexsapi::ReferenceComponentTag, const auto& n) -> void {
                               j = n;
                             },
                             [&j, &attribute](rexsapi::FloatMatrixTag, const auto& m) -> void {
                               encodeCodedMatrix(j, attribute.getValue().coded(), m);
                             },
                             [&j](rexsapi::StringMatrixTag, const auto& m) -> void {
                               j = json::array();
                               for (const auto& row : m.m_Values) {
                                 auto columns = json::array();
                                 for (const auto& column : row) {
                                   columns.emplace_back(column);
                                 }
                                 j.emplace_back(std::move(columns));
                               }
                             },
                             [&j](rexsapi::ArrayOfIntArraysTag, const auto& a) -> void {
                               j = json::array();
                               for (const auto& array : a) {
                                 auto columns = json::array();
                                 for (const auto& column : array) {
                                   columns.emplace_back(column);
                                 }
                                 j.emplace_back(std::move(columns));
                               }
                             }});
  }

  inline void JsonModelSerializer::serialize(ordered_json& model, const TRelations& relations)
  {
    auto& relationsNode = model["relations"];

    for (const auto& relation : relations) {
      ordered_json relationNode;
      relationNode["id"] = getNextRelationId();
      relationNode["type"] = toRealtionTypeString(relation.getType());
      if (relation.getOrder().has_value()) {
        relationNode["order"] = *relation.getOrder();
      }

      relationNode["refs"] = json::array();
      auto& referencesNode = relationNode["refs"];
      for (const auto& reference : relation.getReferences()) {
        ordered_json referenceNode;
        auto id = getComponentId(reference.getComponent().getInternalId());
        referenceNode["id"] = id;
        referenceNode["role"] = toRealtionRoleString(reference.getRole());
        if (!reference.getHint().empty()) {
          referenceNode["hint"] = reference.getHint();
        }

        referencesNode.emplace_back(std::move(referenceNode));
      }

      relationsNode.emplace_back(std::move(relationNode));
    }
  }

  inline void JsonModelSerializer::serialize(ordered_json& model, const TLoadSpectrum& spectrum)
  {
    model["load_spectrum"] = ordered_json{};
    auto& spectrumNode = model["load_spectrum"];
    spectrumNode["id"] = 1;
    uint64_t loadCaseId{0};
    spectrumNode["load_cases"] = json::array();
    auto& loadCases = spectrumNode["load_cases"];
    for (const auto& loadCase : spectrum.getLoadCases()) {
      ordered_json loadCaseNode;
      loadCaseNode["id"] = ++loadCaseId;
      loadCaseNode["components"] = json::array();
      auto& components = loadCaseNode["components"];
      for (const auto& component : loadCase.getLoadComponents()) {
        ordered_json componentNode;
        componentNode["id"] = getComponentId(component.getComponent().getInternalId());
        serialize(componentNode, component.getLoadAttributes());
        components.emplace_back(std::move(componentNode));
      }
      loadCases.emplace_back(std::move(loadCaseNode));
    }

    if (spectrum.hasAccumulation()) {
      spectrumNode["accumulation"] = ordered_json{};
      auto& accumulationNode = spectrumNode["accumulation"];
      accumulationNode["components"] = json::array();
      auto& components = accumulationNode["components"];
      for (const auto& component : spectrum.getAccumulation().getLoadComponents()) {
        ordered_json componentNode;
        componentNode["id"] = getComponentId(component.getComponent().getInternalId());
        serialize(componentNode, component.getLoadAttributes());
        components.emplace_back(std::move(componentNode));
      }
    }
  }

  inline uint64_t JsonModelSerializer::getComponentId(uint64_t internalId) const
  {
    auto it = m_ComponentMapping.find(internalId);
    if (it == m_ComponentMapping.end()) {
      throw TException{fmt::format("cannot find referenced component with id {}", internalId)};
    }
    return it->second;
  }
}

#endif
