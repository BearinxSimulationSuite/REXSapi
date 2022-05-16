
#ifndef REXSAPI_XML_MODEL_SERIALIZER_HXX
#define REXSAPI_XML_MODEL_SERIALIZER_HXX

#include <rexsapi/Model.hxx>
#include <rexsapi/Xml.hxx>

#include <test/TestHelper.hxx>

namespace rexsapi
{
  class XMLModelSerializer
  {
  public:
    void serialize(const TModel& model)
    {
      createDocument();
      auto models = serialize(model.getInfo());
      pugi::xml_node relationsNode = models.append_child("relations");
      serialize(models, model.getComponents());
      serialize(relationsNode, model.getRelations());

      m_Doc.save_file((projectDir() / "test" / "FVA_worm_stage_1-4.rexs").c_str());
    }

  private:
    void createDocument()
    {
      m_Doc.reset();
      auto decl = m_Doc.append_child(pugi::node_declaration);
      decl.append_attribute("version") = "1.0";
      decl.append_attribute("encoding") = "UTF-8";
      decl.append_attribute("standalone") = "no";
    }

    pugi::xml_node serialize(const TModelInfo& info)
    {
      pugi::xml_node modelNode = m_Doc.append_child("model");
      modelNode.append_attribute("applicationId").set_value(info.getApplicationId().c_str());
      modelNode.append_attribute("applicationVersion").set_value(info.getApplicationVersion().c_str());
      modelNode.append_attribute("date").set_value(info.getDate().c_str());
      modelNode.append_attribute("version").set_value(info.getVersion().asString().c_str());
      return modelNode;
    }

    void serialize(pugi::xml_node& relationsNode, const TRelations& relations)
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
          refNode.append_attribute("hint").set_value(reference.getHint().c_str());
          auto id = getComponentId(reference.getComponent().getInternalId());
          refNode.append_attribute("id").set_value(id.c_str());
          refNode.append_attribute("role").set_value(toRealtionRoleString(reference.getRole()).c_str());
        }
      }
    }

    void serialize(pugi::xml_node& modelNode, const TComponents& components)
    {
      pugi::xml_node componentsNode = modelNode.append_child("components");
      for (const auto& component : components) {
        pugi::xml_node compNode = componentsNode.append_child("component");
        auto id = getNextComponentId();
        m_ComponentMapping.emplace(component.getInternalId(), id);
        compNode.append_attribute("id").set_value(id.c_str());
        compNode.append_attribute("name").set_value(component.getName().c_str());
        compNode.append_attribute("type").set_value(component.getType().c_str());
        serialize(compNode, component.getAttributes());
      }
    }

    void serialize(pugi::xml_node& compNode, const TAttributes& attributes)
    {
      for (const auto& attribute : attributes) {
        pugi::xml_node attNode = compNode.append_child("attribute");
        attNode.append_attribute("id").set_value(attribute.getAttributeId().c_str());
        attNode.append_attribute("unit").set_value(attribute.getUnit().getName().c_str());
        if (attribute.getValueType() != rexsapi::TValueType::FLOATING_POINT_ARRAY &&
            attribute.getValueType() != rexsapi::TValueType::INTEGER_ARRAY) {
          attNode.append_child(pugi::node_pcdata).set_value(attribute.getValueAsString().c_str());
        }
      }
    }

    std::string getNextComponentId()
    {
      return std::to_string(++m_ComponentId);
    }

    std::string getNextRelationId()
    {
      return std::to_string(++m_RelationId);
    }

    std::string getComponentId(uint64_t internalId) const
    {
      auto it = m_ComponentMapping.find(internalId);
      if (it == m_ComponentMapping.end()) {
        throw TException{fmt::format("cannot find referenced component with id {}", internalId)};
      }
      return it->second;
    }

    pugi::xml_document m_Doc;
    uint64_t m_ComponentId{0};
    uint64_t m_RelationId{0};
    std::unordered_map<uint64_t, std::string> m_ComponentMapping;
  };
}

#endif
