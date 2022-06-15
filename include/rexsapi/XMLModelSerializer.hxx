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

#ifndef REXSAPI_XML_MODEL_SERIALIZER_HXX
#define REXSAPI_XML_MODEL_SERIALIZER_HXX

#include <rexsapi/Model.hxx>
#include <rexsapi/Xml.hxx>

namespace rexsapi
{
  class XMLModelSerializer
  {
  public:
    template<typename TSerializer>
    void serialize(const TModel& model, TSerializer& serializer);

  private:
    void createDocument();

    pugi::xml_node serialize(const TModelInfo& info);

    void serialize(pugi::xml_node& relationsNode, const TRelations& relations);

    void serialize(pugi::xml_node& modelNode, const TComponents& components);

    pugi::xml_node serialize(pugi::xml_node& componentsNode, const TComponent& component);

    void serialize(pugi::xml_node& compNode, const TAttributes& attributes);

    void serialize(pugi::xml_node& attNode, const TAttribute& attribute);

    void serialize(pugi::xml_node& modelNode, const TLoadSpectrum& loadSpectrum);

    std::string getNextComponentId()
    {
      return std::to_string(++m_ComponentId);
    }

    std::string getNextRelationId()
    {
      return std::to_string(++m_RelationId);
    }

    std::string getComponentId(uint64_t internalId) const;

    pugi::xml_document m_Doc;
    uint64_t m_ComponentId{0};
    uint64_t m_RelationId{0};
    std::unordered_map<uint64_t, std::string> m_ComponentMapping;
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  template<typename TSerializer>
  inline void XMLModelSerializer::serialize(const TModel& model, TSerializer& serializer)
  {
    createDocument();
    auto models = serialize(model.getInfo());
    pugi::xml_node relationsNode = models.append_child("relations");
    serialize(models, model.getComponents());
    serialize(relationsNode, model.getRelations());
    serialize(models, model.getLoadSpectrum());

    serializer.serialize(m_Doc);
  }

  inline void XMLModelSerializer::createDocument()
  {
    m_Doc.reset();
    auto decl = m_Doc.append_child(pugi::node_declaration);
    decl.append_attribute("version") = "1.0";
    decl.append_attribute("encoding") = "UTF-8";
    decl.append_attribute("standalone") = "no";
  }

  inline pugi::xml_node XMLModelSerializer::serialize(const TModelInfo& info)
  {
    pugi::xml_node modelNode = m_Doc.append_child("model");
    modelNode.append_attribute("applicationId").set_value(info.getApplicationId().c_str());
    modelNode.append_attribute("applicationVersion").set_value(info.getApplicationVersion().c_str());
    modelNode.append_attribute("date").set_value(info.getDate().c_str());
    modelNode.append_attribute("version").set_value(info.getVersion().asString().c_str());
    if (info.getApplicationLanguage().has_value()) {
      modelNode.append_attribute("applicationLanguage").set_value(info.getApplicationLanguage()->c_str());
    }
    return modelNode;
  }

  inline void XMLModelSerializer::serialize(pugi::xml_node& relationsNode, const TRelations& relations)
  {
    for (const auto& relation : relations) {
      pugi::xml_node relNode = relationsNode.append_child("relation");
      relNode.append_attribute("id").set_value(getNextRelationId().c_str());
      relNode.append_attribute("type").set_value(toRealtionTypeString(relation.getType()).c_str());
      if (relation.getOrder().has_value()) {
        relNode.append_attribute("order").set_value(relation.getOrder().value());
      }
      for (const auto& reference : relation.getReferences()) {
        pugi::xml_node refNode = relNode.append_child("ref");
        if (!reference.getHint().empty()) {
          refNode.append_attribute("hint").set_value(reference.getHint().c_str());
        }
        auto id = getComponentId(reference.getComponent().getInternalId());
        refNode.append_attribute("id").set_value(id.c_str());
        refNode.append_attribute("role").set_value(toRealtionRoleString(reference.getRole()).c_str());
      }
    }
  }

  inline void XMLModelSerializer::serialize(pugi::xml_node& modelNode, const TComponents& components)
  {
    pugi::xml_node componentsNode = modelNode.append_child("components");
    for (const auto& component : components) {
      auto compNode = serialize(componentsNode, component);
      serialize(compNode, component.getAttributes());
    }
  }

  inline pugi::xml_node XMLModelSerializer::serialize(pugi::xml_node& componentsNode, const TComponent& component)
  {
    pugi::xml_node compNode = componentsNode.append_child("component");
    auto id = getNextComponentId();
    m_ComponentMapping.emplace(component.getInternalId(), id);
    compNode.append_attribute("id").set_value(id.c_str());
    compNode.append_attribute("name").set_value(component.getName().c_str());
    compNode.append_attribute("type").set_value(component.getType().c_str());
    return compNode;
  }

  inline void XMLModelSerializer::serialize(pugi::xml_node& compNode, const TAttributes& attributes)
  {
    for (const auto& attribute : attributes) {
      pugi::xml_node attNode = compNode.append_child("attribute");
      attNode.append_attribute("id").set_value(attribute.getAttributeId().c_str());
      attNode.append_attribute("unit").set_value(attribute.getUnit().getName().c_str());
      serialize(attNode, attribute);
    }
  }

  inline void XMLModelSerializer::serialize(pugi::xml_node& attNode, const TAttribute& attribute)
  {
    if (attribute.getValue().isEmpty()) {
      return;
    }

    rexsapi::dispatch<void>(
      attribute.getValueType(), attribute.getValue(),
      {[&attNode](rexsapi::FloatTag, const auto& d) -> void {
         attNode.append_child(pugi::node_pcdata).set_value(format(d).c_str());
       },
       [&attNode](rexsapi::BoolTag, const auto& b) -> void {
         attNode.append_child(pugi::node_pcdata).set_value(fmt::format("{}", b).c_str());
       },
       [&attNode](rexsapi::IntTag, const auto& i) -> void {
         attNode.append_child(pugi::node_pcdata).set_value(fmt::format("{}", i).c_str());
       },
       [&attNode](rexsapi::EnumTag, const auto& s) -> void {
         attNode.append_child(pugi::node_pcdata).set_value(s.c_str());
       },
       [&attNode](rexsapi::StringTag, const auto& s) -> void {
         attNode.append_child(pugi::node_pcdata).set_value(s.c_str());
       },
       [&attNode](rexsapi::FileReferenceTag, const auto& s) -> void {
         attNode.append_child(pugi::node_pcdata).set_value(s.c_str());
       },
       [&attNode](rexsapi::FloatArrayTag, const auto& a) -> void {
         auto arrayNode = attNode.append_child("array");
         for (const auto& element : a) {
           auto child = arrayNode.append_child("c");
           child.append_child(pugi::node_pcdata).set_value(format(element).c_str());
         }
       },
       [&attNode](rexsapi::BoolArrayTag, const auto& a) -> void {
         auto arrayNode = attNode.append_child("array");
         for (const auto& element : a) {
           auto child = arrayNode.append_child("c");
           child.append_child(pugi::node_pcdata).set_value(fmt::format("{}", element.m_Value).c_str());
         }
       },
       [&attNode](rexsapi::IntArrayTag, const auto& a) -> void {
         auto arrayNode = attNode.append_child("array");
         for (const auto& element : a) {
           auto child = arrayNode.append_child("c");
           child.append_child(pugi::node_pcdata).set_value(fmt::format("{}", element).c_str());
         }
       },
       [&attNode](rexsapi::EnumArrayTag, const auto& a) -> void {
         auto arrayNode = attNode.append_child("array");
         for (const auto& element : a) {
           auto child = arrayNode.append_child("c");
           child.append_child(pugi::node_pcdata).set_value(fmt::format("{}", element).c_str());
         }
       },
       [&attNode](rexsapi::StringArrayTag, const auto& a) -> void {
         auto arrayNode = attNode.append_child("array");
         for (const auto& element : a) {
           auto child = arrayNode.append_child("c");
           child.append_child(pugi::node_pcdata).set_value(fmt::format("{}", element).c_str());
         }
       },
       [&attNode](rexsapi::ReferenceComponentTag, const auto& n) -> void {
         attNode.append_child(pugi::node_pcdata).set_value(fmt::format("{}", n).c_str());
       },
       [&attNode](rexsapi::FloatMatrixTag, const auto& m) -> void {
         auto matrixNode = attNode.append_child("matrix");
         for (const auto& row : m.m_Values) {
           auto rowNode = matrixNode.append_child("r");
           for (const auto& column : row) {
             auto child = rowNode.append_child("c");
             child.append_child(pugi::node_pcdata).set_value(format(column).c_str());
           }
         }
       },
       [&attNode](rexsapi::StringMatrixTag, const auto& m) -> void {
         auto matrixNode = attNode.append_child("matrix");
         for (const auto& row : m.m_Values) {
           auto rowNode = matrixNode.append_child("r");
           for (const auto& column : row) {
             auto child = rowNode.append_child("c");
             child.append_child(pugi::node_pcdata).set_value(fmt::format("{}", column).c_str());
           }
         }
       },
       [&attNode](rexsapi::ArrayOfIntArraysTag, const auto& a) -> void {
         auto arraysNode = attNode.append_child("array_of_arrays");
         for (const auto& array : a) {
           auto aNode = arraysNode.append_child("array");
           for (const auto& c : array) {
             auto child = aNode.append_child("c");
             child.append_child(pugi::node_pcdata).set_value(fmt::format("{}", c).c_str());
           }
         }
       }});
  }

  inline void XMLModelSerializer::serialize(pugi::xml_node& modelNode, const TLoadSpectrum& loadSpectrum)
  {
    if (loadSpectrum.hasLoadCases()) {
      uint64_t loadCaseId{0};
      pugi::xml_node loadSpectrumNode = modelNode.append_child("load_spectrum");
      loadSpectrumNode.append_attribute("id").set_value("1");
      for (const auto& loadCase : loadSpectrum.getLoadCases()) {
        pugi::xml_node loadCaseNode = loadSpectrumNode.append_child("load_case");
        loadCaseNode.append_attribute("id").set_value(std::to_string(++loadCaseId).c_str());
        for (const auto& loadComponent : loadCase.getLoadComponents()) {
          auto compNode = serialize(loadCaseNode, loadComponent.getComponent());
          serialize(compNode, loadComponent.getLoadAttributes());
        }
      }
    }
  }

  inline std::string XMLModelSerializer::getComponentId(uint64_t internalId) const
  {
    auto it = m_ComponentMapping.find(internalId);
    if (it == m_ComponentMapping.end()) {
      throw TException{fmt::format("cannot find referenced component with id {}", internalId)};
    }
    return it->second;
  }
}

#endif
