
#include <rexsapi/Rexsapi.hxx>

#include <iostream>

#define ASSERT_OTHERWISE_THROW(expression, message)                                                                    \
  {                                                                                                                    \
    if (!(expression)) {                                                                                               \
      throw rexsapi::TException{message};                                                                              \
    }                                                                                                                  \
  }

struct TREXSVersionNumber {
  TREXSVersionNumber() = default;

  TREXSVersionNumber(unsigned int major, unsigned int minor)
  : m_MajorVersionNr{major}
  , m_MinorVersionNr{minor}
  {
  }

  bool matches(const TREXSVersionNumber& fromVersion, const TREXSVersionNumber& toVersion) const
  {
    return *this >= fromVersion && *this <= toVersion;
  }

  friend bool operator>=(const TREXSVersionNumber& lhs, const TREXSVersionNumber& rhs)
  {
    return (lhs.m_MajorVersionNr > rhs.m_MajorVersionNr ||
            (lhs.m_MajorVersionNr == rhs.m_MajorVersionNr && lhs.m_MinorVersionNr >= rhs.m_MinorVersionNr));
  }

  friend bool operator<=(const TREXSVersionNumber& lhs, const TREXSVersionNumber& rhs)
  {
    return (lhs.m_MajorVersionNr < rhs.m_MajorVersionNr ||
            (lhs.m_MajorVersionNr == rhs.m_MajorVersionNr && lhs.m_MinorVersionNr <= rhs.m_MinorVersionNr));
  }

  unsigned int m_MajorVersionNr{0};
  unsigned int m_MinorVersionNr{0};
};

enum object_type { REXS_component, REXS_component_group, intermediate_layer_object };

enum direction_type { bidirectional, REXS_to_Bearinx, Bearinx_to_REXS };

enum dimension_of_attribute { scalar_dimension, vector_dimension, matrix_2D_dimension };

enum type_of_attribute { integer_type, reference_type };

enum relation_type { invalid_relation_type, top_level, sub_level };

class TRule
{
public:
  virtual ~TRule() = default;

  TREXSVersionNumber From_REXS_Version;
  TREXSVersionNumber To_REXS_Version;
};

class TComponentRule : public TRule
{
public:
  object_type Type_Side_1;
  direction_type Direction;
  std::string Name_Side_1;
  std::string Name_Side_2;
};

class TComponentGroupRule : public TRule
{
public:
  object_type Group_Type;
  std::string Group_Name;
  std::vector<std::string> Types;
};

class TAttributeRule : public TRule
{
public:
  object_type Type_Side_1;
  direction_type Direction;
  std::string Attribute_Name_side_1;
  std::string Attribute_Name_side_2;
  type_of_attribute Attribute_Type;
  dimension_of_attribute Attribute_Dimension;
  std::string Attribute_Unit_side_1;
  std::string Attribute_Unit_side_2;
};

class TRelationRule : public TRule
{
public:
  std::string getParameter(const unsigned int i) const
  {
    std::string param = "invalid";
    switch (i) {
      case 0:
        param = Parameter_1;
        break;
      case 1:
        param = Parameter_2;
        break;
      case 2:
        param = Parameter_3;
        break;
      default:
        ASSERT_OTHERWISE_THROW(false, "unhandled");
        break;
    }
    return param;
  }

  relation_type getRelationType(const unsigned int i) const
  {
    relation_type rel_type = invalid_relation_type;
    switch (i) {
      case 0:
        rel_type = Relation_Type_1;
        break;
      case 1:
        rel_type = Relation_Type_2;
        break;
      case 2:
        rel_type = Relation_Type_3;
        break;
      default:
        ASSERT_OTHERWISE_THROW(false, "unhandled");
        break;
    }
    return rel_type;
  }
  std::string getName(const unsigned int i) const
  {
    std::string name = "invalid";
    switch (i) {
      case 0:
        name = Name_1;
        break;
      case 1:
        name = Name_2;
        break;
      case 2:
        name = Name_3;
        break;
      default:
        ASSERT_OTHERWISE_THROW(false, "unhandled");
        break;
    }
    return name;
  }

  std::string Name;
  object_type Object_Type_1;
  object_type Object_Type_2;
  std::string Parameter_1;
  std::string Parameter_2;
  std::string Parameter_3;
  relation_type Relation_Type_1;
  relation_type Relation_Type_2;
  relation_type Relation_Type_3;
  std::string Name_1;
  std::string Name_2;
  std::string Name_3;
  direction_type Direction;
  std::string Type;
};

struct TIntermediateLayerAttribute {
  void setAttributeName(std::string name)
  {
    Name = std::move(name);
  }

  void setAttributeType(type_of_attribute type)
  {
    Type = type;
  }

  void setAttributeDimension(dimension_of_attribute dimension)
  {
    Dimension = dimension;
  }

  void setAttributeUnit(const std::string attribute_unit)
  {
    AttributeUnit = attribute_unit;
  }

  void setAttributeValue(const std::string attribute_value, const int attribute_index = -1)
  {
    (void)attribute_value;
    (void)attribute_index;
  }

  std::string Name;
  type_of_attribute Type;
  dimension_of_attribute Dimension;
  std::string AttributeUnit;
};

struct TIntermediateLayerObject {
  std::string LayerObjectType;
  std::string Name;

  void register_attribute(TIntermediateLayerAttribute* new_attribute)
  {
    Attributes.emplace_back(new_attribute);
  }

private:
  std::vector<TIntermediateLayerAttribute*> Attributes;
};

class TIntermediateLayerRelation
{
public:
  class Component
  {
  public:
    relation_type BearinxRelation_Type = invalid_relation_type;
    std::string BearinxObjectType;
    relation_type REXSRelation_Type = invalid_relation_type;
    std::string REXSObjectType;
    TIntermediateLayerObject* Object;
    int ObjectIndex = -1;  // ChildIndex, Order
  };

  std::string LayerRelationType;

  std::vector<Component*> Comps;

  ~TIntermediateLayerRelation()
  {
    for (std::vector<Component*>::iterator it = Comps.begin(); it < Comps.end(); ++it) {
      delete *it;
    }
  }
};

struct TREXSTransmissionModelIntermediateLayer {
  void setREXSVersion(TREXSVersionNumber version)
  {
    m_Version = version;
  }

  TREXSVersionNumber getREXSVersion() const
  {
    return m_Version;
  }

  std::string convert_value(const std::string value, const type_of_attribute attribute_type, const object_type type_1,
                            const std::string unit_1, const object_type type_2, const std::string unit_2) const
  {
    (void)value;
    (void)attribute_type;
    (void)type_1;
    (void)unit_1;
    (void)type_2;
    (void)unit_2;
    return "";
  }

  std::vector<TRule*> Rules;
  std::vector<TIntermediateLayerObject*> IntermediateLayerObjects;
  std::vector<TIntermediateLayerRelation*> IntermediateLayerRelation;

private:
  TREXSVersionNumber m_Version;
};

struct Data {
  TREXSTransmissionModelIntermediateLayer* IntermediateLayer;
};

static bool is_of_rexs_type(const rexsapi::TComponent& component, const std::string& rexs_type_string,
                            const TREXSTransmissionModelIntermediateLayer* intermediate_layer)
{
  bool of_rexs_type = false;
  const std::vector<TRule*>& rules = intermediate_layer->Rules;
  std::string REXS_xml_component_type = component.getId();
  if (REXS_xml_component_type == rexs_type_string) {
    of_rexs_type = true;
  } else {
    // In Gruppen schauen
    for (std::vector<TRule*>::const_iterator rule_it_for_groups = rules.begin();
         (rule_it_for_groups != rules.end()) && (!of_rexs_type); ++rule_it_for_groups) {
      TComponentGroupRule* comp_group_rule = dynamic_cast<TComponentGroupRule*>(*rule_it_for_groups);
      if ((comp_group_rule != NULL) && (comp_group_rule->Group_Type == REXS_component_group)) {
        if (comp_group_rule->Group_Name == rexs_type_string) {
          for (std::vector<std::string>::iterator types_it = comp_group_rule->Types.begin();
               types_it != comp_group_rule->Types.end(); ++types_it) {
            if ((*types_it) == REXS_xml_component_type) {
              of_rexs_type = true;
              break;
            }
          }
        }
      }
    }
  }

  return of_rexs_type;
}
class TRelationRules
{
public:
  TRelationRules(const TREXSVersionNumber& rexs_version, const std::vector<TRule*>& rules)
  {
    for (const auto* rule : rules) {
      if (const auto* relationRule = dynamic_cast<const TRelationRule*>(rule); relationRule) {
        if ((relationRule->Object_Type_1 == REXS_component || relationRule->Object_Type_1 == REXS_component_group) &&
            (relationRule->Direction == bidirectional || relationRule->Direction == REXS_to_Bearinx) &&
            rexs_version.matches(relationRule->From_REXS_Version, relationRule->To_REXS_Version)) {
          m_RelationRules[relationRule->Type] = relationRule;
        }
      }
    }
  }

  const TRelationRule* getRule(const rexsapi::TRelation& relation) const
  {
    auto it = m_RelationRules.find(rexsapi::toRealtionTypeString(relation.getType()));
    if (it == m_RelationRules.end()) {
      return nullptr;
    }

    return it->second;
  }

private:
  std::unordered_map<std::string, const TRelationRule*> m_RelationRules;
};

class TComponentRules
{
public:
  TComponentRules(const TREXSVersionNumber& rexs_version, const std::vector<TRule*>& rules)
  {
    for (const auto* rule : rules) {
      if (const auto* componentRule = dynamic_cast<const TComponentRule*>(rule); componentRule) {
        if (componentRule->Type_Side_1 == REXS_component &&
            (componentRule->Direction == bidirectional || componentRule->Direction == REXS_to_Bearinx) &&
            rexs_version.matches(componentRule->From_REXS_Version, componentRule->To_REXS_Version)) {
          m_ComponentRules[componentRule->Name_Side_1] = componentRule;
        }
      }
    }
  }

  const TComponentRule* getRule(const rexsapi::TComponent& component) const
  {
    auto it = m_ComponentRules.find(component.getId());
    if (it == m_ComponentRules.end()) {
      return nullptr;
    }

    return it->second;
  }

private:
  std::unordered_map<std::string, const TComponentRule*> m_ComponentRules;
};

class TAttributeRules
{
public:
  TAttributeRules(const TREXSVersionNumber& rexs_version, const std::vector<TRule*>& rules)
  {
    for (const auto* rule : rules) {
      if (const auto* attributeRule = dynamic_cast<const TAttributeRule*>(rule); attributeRule) {
        if ((attributeRule->Type_Side_1 == REXS_component || attributeRule->Type_Side_1 == REXS_component_group) &&
            (attributeRule->Direction == bidirectional || attributeRule->Direction == REXS_to_Bearinx) &&
            rexs_version.matches(attributeRule->From_REXS_Version, attributeRule->To_REXS_Version)) {
          m_AttributeRules[attributeRule->Attribute_Name_side_1] = attributeRule;
        }
      }
    }
  }

  const TAttributeRule* getRule(const rexsapi::TAttribute& attribute) const
  {
    auto it = m_AttributeRules.find(attribute.getAttributeId());
    if (it == m_AttributeRules.end()) {
      return nullptr;
    }

    return it->second;
  }

private:
  std::unordered_map<std::string, const TAttributeRule*> m_AttributeRules;
};


static inline rexsapi::database::TModelRegistry createModelRegistry(const std::filesystem::path& databaseSchemaPath,
                                                                    const std::filesystem::path& databasePath)
{
  rexsapi::xml::TFileXsdSchemaLoader schemaLoader{databaseSchemaPath};
  rexsapi::database::TFileResourceLoader resourceLoader{databasePath};
  rexsapi::database::TXmlModelLoader modelLoader{resourceLoader, schemaLoader};
  return rexsapi::database::TModelRegistry::createModelRegistry(modelLoader).first;
}


static inline rexsapi::xml::TXSDSchemaValidator createSchameValidator(const std::filesystem::path& modelSchemaPath)
{
  rexsapi::xml::TFileXsdSchemaLoader schemaLoader{modelSchemaPath};
  return rexsapi::xml::TXSDSchemaValidator{schemaLoader};
}

template<typename T>
void setAttributeValue(Data& data, TIntermediateLayerAttribute& layerAttribute, const TAttributeRule& attributeRule,
                       const std::vector<T>& values)
{
  int i = 0;
  for (const auto& val : values) {
    layerAttribute.setAttributeValue(
      data.IntermediateLayer->convert_value(rexsapi::TValue{val}.asString(), attributeRule.Attribute_Type,
                                            REXS_component, attributeRule.Attribute_Unit_side_1,
                                            intermediate_layer_object, attributeRule.Attribute_Unit_side_2),
      i++);
  }
}

void setAttributeValue(Data& data, TIntermediateLayerAttribute& layerAttribute, const TAttributeRule& attributeRule,
                       rexsapi::TValueType type, rexsapi::TValue& value)
{
  switch (type) {
    case rexsapi::TValueType::FLOATING_POINT:
    case rexsapi::TValueType::BOOLEAN:
    case rexsapi::TValueType::INTEGER:
    case rexsapi::TValueType::ENUM:
    case rexsapi::TValueType::STRING:
    case rexsapi::TValueType::FILE_REFERENCE:
      layerAttribute.setAttributeValue(data.IntermediateLayer->convert_value(
        value.asString(), attributeRule.Attribute_Type, REXS_component, attributeRule.Attribute_Unit_side_1,
        intermediate_layer_object, attributeRule.Attribute_Unit_side_2));
      break;
    case rexsapi::TValueType::BOOLEAN_ARRAY:
      setAttributeValue(data, layerAttribute, attributeRule, value.getValue<std::vector<rexsapi::Bool>>());
      break;
    case rexsapi::TValueType::FLOATING_POINT_ARRAY:
    case rexsapi::TValueType::REFERENCE_COMPONENT:
    case rexsapi::TValueType::FLOATING_POINT_MATRIX:
    case rexsapi::TValueType::INTEGER_ARRAY:
    case rexsapi::TValueType::ENUM_ARRAY:
    case rexsapi::TValueType::ARRAY_OF_INTEGER_ARRAYS:
      break;
  }
}

class TREXSTransmissionModelXmlInterface
{
public:
  static TREXSTransmissionModelIntermediateLayer* load(const std::filesystem::path& basePath,
                                                       const std::filesystem::path& modelFile)
  {
    std::unique_ptr<TREXSTransmissionModelIntermediateLayer> intermediateLayer{
      new TREXSTransmissionModelIntermediateLayer};
    Data data{intermediateLayer.get()};

    TREXSTransmissionModelXmlInterface transmissionInterface{
      createModelRegistry(basePath / "models" / "rexs-dbmodel.xsd", basePath / "models"),
      createSchameValidator(basePath / "models" / "rexs-schema.xsd")};

    if (!transmissionInterface.load(data, modelFile)) {
      // add some message
      return nullptr;
    }

    return intermediateLayer.release();
  }

private:
  TREXSTransmissionModelXmlInterface(rexsapi::database::TModelRegistry registry,
                                     rexsapi::xml::TXSDSchemaValidator validator)
  : m_Registry{std::move(registry)}
  , m_Validator{std::move(validator)}
  {
  }

  bool load(Data& data, const std::filesystem::path& modelFile) const
  {
    rexsapi::TFileModelLoader loader{m_Validator, modelFile};
    rexsapi::TLoaderResult result;
    auto model = loader.load(result, m_Registry);

    if (!result) {
      // add some message
      return false;
    }

    return fillIntermediateLayer(data, *model);
  }

  bool fillIntermediateLayer(Data& data, const rexsapi::TModel& model) const
  {
    bool success{true};

    data.IntermediateLayer->setREXSVersion(
      TREXSVersionNumber{model.getInfo().getVersion().getMajor(), model.getInfo().getVersion().getMinor()});

    if (success) {
      success = fillIntermediateLayerComponents(data, model.getComponents());
    }
    if (success) {
      success = fillIntermediateLayerRelations(data, model.getRelations());
    }

    return true;
  }

  bool fillIntermediateLayerComponents(Data& data, const rexsapi::TComponents& components) const
  {
    TComponentRules componentRules{data.IntermediateLayer->getREXSVersion(), data.IntermediateLayer->Rules};
    TAttributeRules attributeRules{data.IntermediateLayer->getREXSVersion(), data.IntermediateLayer->Rules};

    for (const auto& component : components) {
      if (const auto* componentRule = componentRules.getRule(component); componentRule) {
        auto* new_layer_object = new TIntermediateLayerObject;
        new_layer_object->LayerObjectType = componentRule->Name_Side_2;
        new_layer_object->Name = component.getName();
        data.IntermediateLayer->IntermediateLayerObjects.emplace_back(new_layer_object);

        for (const auto& attribute : component.getAttributes()) {
          if (const auto* attributeRule = attributeRules.getRule(attribute); attributeRule) {
            if (!((attribute.getUnit().getName() == attributeRule->Attribute_Unit_side_1) ||
                  (attribute.getUnit().getName() == "none") ||
                  ((attribute.getUnit().getName() == "") && (attributeRule->Attribute_Unit_side_1 == "none")))) {
              // add some error message
            }

            auto* new_layer_attribute = new TIntermediateLayerAttribute;
            new_layer_attribute->setAttributeName(attributeRule->Attribute_Name_side_2);
            new_layer_attribute->setAttributeType(attributeRule->Attribute_Type);
            new_layer_attribute->setAttributeDimension(attributeRule->Attribute_Dimension);
            new_layer_attribute->setAttributeUnit(attributeRule->Attribute_Unit_side_2);

            switch (attributeRule->Attribute_Dimension) {
              case scalar_dimension:
                if (attributeRule->Attribute_Type != reference_type) {
                  new_layer_attribute->setAttributeValue(data.IntermediateLayer->convert_value(
                    attribute.getValueAsString(), attributeRule->Attribute_Type, REXS_component,
                    attributeRule->Attribute_Unit_side_1, intermediate_layer_object,
                    attributeRule->Attribute_Unit_side_2));
                } else {  // Referenz-Typ --> Rexs-ID mit Zwischenschicht-ID ersetzen
                  std::string layer_id = "reference not found";
                  // TODO (lcf): solve reference object problem see Issue #33
                  new_layer_attribute->setAttributeValue(layer_id);
                }
                break;
              case vector_dimension: {
                /*

                XMLSize_t element_node_list_length = array_element_node_list->getLength();
                for (XMLSize_t i = 0; i < element_node_list_length; ++i) {
                  DOMNode* node_i = array_element_node_list->item(i);
                  DOMElement* array_element_i = dynamic_cast<DOMElement*>(node_i);
                  std::string rexs_attribute_value_i = XMLCh_to_string(array_element_i->getTextContent());
                  new_layer_attribute->setAttributeValue(
                    intermediate_layer->convert_value(rexs_attribute_value_i, attr_rule->Attribute_Type, REXS_component,
                                                      attr_rule->Attribute_Unit_side_1, intermediate_layer_object,
                                                      attr_rule->Attribute_Unit_side_2),
                    static_cast<int>(i));
                }*/
                break;
              }
              case matrix_2D_dimension:
                break;
              default:
                ASSERT_OTHERWISE_THROW(attributeRule->Attribute_Dimension == matrix_2D_dimension, "REXS import error");
            }

            new_layer_object->register_attribute(new_layer_attribute);
          }
        }
      }
    }

    return true;
  }

  bool fillIntermediateLayerRelations(Data& data, const rexsapi::TRelations& relations) const
  {
    bool success{true};

    TRelationRules relationRules{data.IntermediateLayer->getREXSVersion(), data.IntermediateLayer->Rules};

    for (const auto& relation : relations) {
      if (const auto* relationRule = relationRules.getRule(relation); relationRule) {
        const auto& references = relation.getReferences();
        ASSERT_OTHERWISE_THROW(references.size() <= 3, "noch nicht behandelt");
        std::unique_ptr<TIntermediateLayerRelation> new_relation{new TIntermediateLayerRelation};
        bool relation_ok = true;
        new_relation->LayerRelationType = relationRule->Name;
        new_relation->Comps.resize(references.size());

        for (const auto& reference : references) {
          auto role = reference.getRole();
          const auto& component = reference.getComponent();
          TIntermediateLayerObject* layer_object =
            findIntermediateObject(data.IntermediateLayer->IntermediateLayerObjects, component);
          TIntermediateLayerRelation::Component* new_relation_component = new TIntermediateLayerRelation::Component;
          new_relation_component->Object = layer_object;

          auto component_idx = getComponentIndex(*relationRule, role, references.size());
          relation_type rel_type =
            rexsapi::getRoleType(role) == rexsapi::TRelationRoleType::TOP_LEVEL ? top_level : sub_level;

          ASSERT_OTHERWISE_THROW(rel_type == relationRule->getRelationType(component_idx), "check failed");
          ASSERT_OTHERWISE_THROW(rexsapi::toRealtionRoleString(role) == relationRule->getParameter(component_idx),
                                 "check failed");
          new_relation_component->REXSRelation_Type = rel_type;
          std::string rel_rule_name_j = relationRule->getName(component_idx);
          relation_ok = relation_ok && is_of_rexs_type(component, rel_rule_name_j, data.IntermediateLayer);
          new_relation_component->REXSObjectType = component.getId();

          ASSERT_OTHERWISE_THROW(new_relation->Comps[component_idx] == nullptr, "check failed");
          new_relation->Comps[component_idx] = new_relation_component;

          if (hasManufacturingStep(relation)) {
            int order_idx = static_cast<int>(relation.getOrder().value_or(1));
            ASSERT_OTHERWISE_THROW((order_idx >= 0) && (order_idx < 5), "Unexpected range!");
            new_relation->Comps[component_idx]->ObjectIndex = order_idx;
          }
          if (relation_ok) {
            // check, dass alles gesetzt wurde
            for (uint32_t k = 0; k < references.size(); ++k) {
              ASSERT_OTHERWISE_THROW(new_relation->Comps[k] != nullptr, "bug");
            }
            data.IntermediateLayer->IntermediateLayerRelation.push_back(new_relation.release());
          }
        }
      }
    }

    return success;
  }

  static uint32_t getComponentIndex(const TRelationRule& rel_rule, rexsapi::TRelationRole role, size_t nr_of_comps)
  {
    uint32_t component_idx = 0;
    std::string role_string = rexsapi::toRealtionRoleString(role);
    for (; component_idx < nr_of_comps; ++component_idx) {
      std::string parameter_string_i = rel_rule.getParameter(component_idx);
      if (role_string == parameter_string_i) {
        break;
      }
    }
    ASSERT_OTHERWISE_THROW(component_idx < nr_of_comps, "bug");
    return component_idx;
  }

  static TIntermediateLayerObject*
  findIntermediateObject(const std::vector<TIntermediateLayerObject*>& intermediateLayerObjects,
                         const rexsapi::TComponent& component)
  {
    auto it =
      std::find_if(intermediateLayerObjects.begin(), intermediateLayerObjects.end(), [&component](const auto* object) {
        return component.getName() == object->Name;
      });
    if (it == intermediateLayerObjects.end()) {
      // should never happen, maybe assert
      return nullptr;
    }
    return *it;
  }

  static bool hasManufacturingStep(const rexsapi::TRelation& relation)
  {
    return relation.getType() == rexsapi::TRelationType::MANUFACTURING_STEP;
  }

  rexsapi::database::TModelRegistry m_Registry;
  rexsapi::xml::TXSDSchemaValidator m_Validator;
};


int main(int argc, char** argv)
{
  if (argc != 3) {
    std::cerr << "Usage: example <base path> <model file>" << std::endl;
    return -1;
  }

  auto* intermediateLayer =
    TREXSTransmissionModelXmlInterface::load(std::filesystem::path{argv[1]}, std::filesystem::path{argv[2]});

  if (intermediateLayer) {
    std::cerr << "Successfully imported" << std::endl;
  } else {
    std::cerr << "Error importing model file" << std::endl;
  }

  return 0;
}
