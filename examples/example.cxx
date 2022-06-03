
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

enum object_type { REXS_component, REXS_component_group, intermediate_layer_object, invalid_object_type };

enum direction_type { bidirectional, REXS_to_Bearinx, Bearinx_to_REXS };

enum dimension_of_attribute { scalar_dimension, vector_dimension, matrix_2D_dimension };

enum type_of_attribute { int_value_type, reference_type, boolean_type, double_value_type, enum_type, string_type };

enum relation_type { invalid_relation_type, top_level, sub_level };

enum type_of_attribute_rule { transfer };

class TRule
{
public:
  virtual ~TRule() = default;

  void set_from_to_REXS_version(const std::string& from_rexs_version, const std::string& to_rexs_version)
  {
    std::regex reg_expr("^(\\d+)\\.(\\d+)$");
    std::smatch s_match_from, s_match_to;
    std::regex_search(from_rexs_version, s_match_from, reg_expr);
    std::regex_search(to_rexs_version, s_match_to, reg_expr);
    ASSERT_OTHERWISE_THROW((s_match_from.size() == 3) && (s_match_to.size() == 3), "bug");
    From_REXS_Version = TREXSVersionNumber((uint)std::stoi(s_match_from[1]), (uint)std::stoi(s_match_from[2]));
    To_REXS_Version = TREXSVersionNumber((uint)std::stoi(s_match_to[1]), (uint)std::stoi(s_match_to[2]));
  }

  TREXSVersionNumber From_REXS_Version;
  TREXSVersionNumber To_REXS_Version;
};

class TComponentRule : public TRule
{
public:
  TComponentRule(direction_type dir, object_type side_1, std::string name_1, object_type side_2, std::string name_2,
                 const std::string from_rexs_version, const std::string to_rexs_version)
  : Direction(dir)
  , Type_Side_1(side_1)
  , Type_Side_2(side_2)
  , Name_Side_1(name_1)
  , Name_Side_2(name_2)
  {
    set_from_to_REXS_version(from_rexs_version, to_rexs_version);
  }

  direction_type Direction;
  object_type Type_Side_1;
  object_type Type_Side_2;
  std::string Name_Side_1;
  std::string Name_Side_2;
};

class TComponentGroupRule : public TRule
{
public:
  TComponentGroupRule(object_type group_type, std::string group_name, std::vector<std::string> types)
  : Group_Type(group_type)
  , Group_Name(group_name)
  {
    Types = types;
  }

  object_type Group_Type;
  std::string Group_Name;
  std::vector<std::string> Types;
};

class TAttributeRule : public TRule
{
public:
  TAttributeRule(const direction_type direction, const type_of_attribute_rule rule_type, const object_type object_t_1,
                 const std::string object_n_1, const std::string attribute_1, const std::string unit_1,
                 const object_type object_t_2, const std::string object_n_2, const std::string attribute_2,
                 const std::string unit_2, const type_of_attribute attribute_type,
                 const dimension_of_attribute attribute_dimension, const std::string from_rexs_version,
                 const std::string to_rexs_version)
  : Direction(direction)
  , Attribute_Rule_Type(rule_type)
  , Object_Type_side_1(object_t_1)
  , Object_Name_side_1(object_n_1)
  , Attribute_Name_side_1(attribute_1)
  , Attribute_Unit_side_1(unit_1)
  , Object_Type_side_2(object_t_2)
  , Object_Name_side_2(object_n_2)
  , Attribute_Name_side_2(attribute_2)
  , Attribute_Unit_side_2(unit_2)
  , Attribute_Type(attribute_type)
  , Attribute_Dimension(attribute_dimension)
  {
    set_from_to_REXS_version(from_rexs_version, to_rexs_version);
  }

  TAttributeRule(const direction_type direction, const type_of_attribute_rule rule_type, const std::string comment,
                 const object_type object_t, const std::string object_n, const std::string attribute,
                 const std::string value, const std::string unit, const type_of_attribute attribute_type,
                 const dimension_of_attribute attribute_dimension, const std::string from_rexs_version,
                 const std::string to_rexs_version)
  : Direction(direction)
  , Attribute_Rule_Type(rule_type)
  , Object_Type_side_1(object_t)
  , Object_Name_side_1(object_n)
  , Attribute_Name_side_1(attribute)
  , Attribute_Value_side_1(value)
  , Attribute_Unit_side_1(unit)
  , Object_Type_side_2(invalid_object_type)
  , Object_Name_side_2("")
  , Attribute_Name_side_2("")
  , Attribute_Unit_side_2("")
  , Attribute_Type(attribute_type)
  , Attribute_Dimension(attribute_dimension)
  {
    (void)comment;
    set_from_to_REXS_version(from_rexs_version, to_rexs_version);
  }

  direction_type Direction;
  type_of_attribute_rule Attribute_Rule_Type;
  object_type Object_Type_side_1;
  std::string Object_Name_side_1;
  std::string Attribute_Name_side_1;
  std::string Attribute_Value_side_1;
  std::string Attribute_Unit_side_1;
  object_type Object_Type_side_2;
  std::string Object_Name_side_2;
  std::string Attribute_Name_side_2;
  std::string Attribute_Unit_side_2;
  type_of_attribute Attribute_Type;
  dimension_of_attribute Attribute_Dimension;
};

class TRelationRule : public TRule
{
public:
  TRelationRule(std::string name, direction_type direction, std::string type, object_type o_type_1,
                relation_type r_type_1, std::string name_1, std::string param_1, object_type o_type_2,
                relation_type r_type_2, std::string name_2, std::string param_2, const std::string from_rexs_version,
                const std::string to_rexs_version)
  : Name(name)
  , Direction(direction)
  , Type(type)
  , Nr_of_Objects(2)
  , Object_Type_1(o_type_1)
  , Relation_Type_1(r_type_1)
  , Name_1(name_1)
  , Parameter_1(param_1)
  , Object_Type_2(o_type_2)
  , Relation_Type_2(r_type_2)
  , Name_2(name_2)
  , Parameter_2(param_2)
  {
    set_from_to_REXS_version(from_rexs_version, to_rexs_version);
  }

  TRelationRule(std::string name, direction_type direction, std::string type, object_type o_type_1,
                relation_type r_type_1, std::string name_1, std::string param_1, object_type o_type_2,
                relation_type r_type_2, std::string name_2, std::string param_2, object_type o_type_3,
                relation_type r_type_3, std::string name_3, std::string param_3, const std::string from_rexs_version,
                const std::string to_rexs_version)
  : Name(name)
  , Direction(direction)
  , Type(type)
  , Nr_of_Objects(3)
  , Object_Type_1(o_type_1)
  , Relation_Type_1(r_type_1)
  , Name_1(name_1)
  , Parameter_1(param_1)
  , Object_Type_2(o_type_2)
  , Relation_Type_2(r_type_2)
  , Name_2(name_2)
  , Parameter_2(param_2)
  , Object_Type_3(o_type_3)
  , Relation_Type_3(r_type_3)
  , Name_3(name_3)
  , Parameter_3(param_3)
  {
    set_from_to_REXS_version(from_rexs_version, to_rexs_version);
  }

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
  direction_type Direction;
  std::string Type;
  const unsigned int Nr_of_Objects;
  object_type Object_Type_1;
  relation_type Relation_Type_1;
  std::string Name_1;
  std::string Parameter_1;
  object_type Object_Type_2;
  relation_type Relation_Type_2;
  std::string Name_2;
  std::string Parameter_2;
  object_type Object_Type_3;
  relation_type Relation_Type_3;
  std::string Name_3;
  std::string Parameter_3;
};


class TUnitRule : public TRule
{
public:
  object_type Object_Type_side_1;
  std::string Unit_Name_side_1;
  object_type Object_Type_side_2;
  std::string Unit_Name_side_2;
  double Conversion_Factor_side_1_to_side_2;

  TUnitRule(const object_type type_1, const std::string unit_1, const object_type type_2, const std::string unit_2,
            const double factor_1_2)
  : Object_Type_side_1(type_1)
  , Unit_Name_side_1(unit_1)
  , Object_Type_side_2(type_2)
  , Unit_Name_side_2(unit_2)
  , Conversion_Factor_side_1_to_side_2(factor_1_2)
  {
  }
};

class TREXSTransmissionModelTransformationRules
{
public:
  void set_Rules()
  {
#if __has_include("REXSTransmissionModelTransformationRulesGeneratedCode_reduced.hxx")
  #include "REXSTransmissionModelTransformationRulesGeneratedCode_reduced.hxx"
#endif
  }

  std::vector<TRule*> Rules;
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
  void setAttributeValue(const std::string attribute_value, const int attribute_row_index,
                         const int attribute_column_index)
  {
    (void)attribute_value;
    (void)attribute_row_index;
    (void)attribute_column_index;
  }

  std::string Name;
  type_of_attribute Type;
  dimension_of_attribute Dimension;
  std::string AttributeUnit;
};

struct TIntermediateLayerObject {
  std::string LayerObjectType;
  std::string Name;
  uint64_t Id{0};

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
  TREXSTransmissionModelIntermediateLayer()
  {
    TransformationRules.set_Rules();
  }

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
    return value;
  }

  const std::vector<TRule*>& getRules() const
  {
    return TransformationRules.Rules;
  }

  TREXSTransmissionModelTransformationRules TransformationRules;
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
  const std::vector<TRule*>& rules = intermediate_layer->getRules();
  std::string REXS_xml_component_type = component.getType();
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
          m_RelationRules.emplace_back(relationRule);
        }
      }
    }
  }

  std::vector<const TRelationRule*> getRules(const rexsapi::TRelation& relation) const
  {
    std::vector<const TRelationRule*> rules;

    std::for_each(m_RelationRules.begin(), m_RelationRules.end(),
                  [&rules, type = rexsapi::toRealtionTypeString(relation.getType())](const auto& rule) {
                    if (rule->Type == type) {
                      rules.emplace_back(rule);
                    }
                  });

    return rules;
  }

private:
  std::vector<const TRelationRule*> m_RelationRules;
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
    auto it = m_ComponentRules.find(component.getType());
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
        if ((attributeRule->Object_Type_side_1 == REXS_component ||
             attributeRule->Object_Type_side_1 == REXS_component_group) &&
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


static inline rexsapi::xml::TXSDSchemaValidator createSchemaValidator(const std::filesystem::path& modelSchemaPath)
{
  rexsapi::xml::TFileXsdSchemaLoader schemaLoader{modelSchemaPath};
  return rexsapi::xml::TXSDSchemaValidator{schemaLoader};
}

template<typename T>
static void setAttributeValue(const Data& data, TIntermediateLayerAttribute& layerAttribute,
                              const TAttributeRule& attributeRule, const std::vector<T>& values)
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

template<typename T>
static void setAttributeValue(const Data& data, TIntermediateLayerAttribute& layerAttribute,
                              const TAttributeRule& attributeRule, const rexsapi::TMatrix<T>& values)
{
  int i = 0;
  int j = 0;
  for (const auto& row : values.m_Values) {
    for (const auto& col : row) {
      layerAttribute.setAttributeValue(
        data.IntermediateLayer->convert_value(rexsapi::TValue{col}.asString(), attributeRule.Attribute_Type,
                                              REXS_component, attributeRule.Attribute_Unit_side_1,
                                              intermediate_layer_object, attributeRule.Attribute_Unit_side_2),
        i, j++);
    }
    ++i;
  }
}

static void setAttributeValue(const Data& data, TIntermediateLayerAttribute& layerAttribute,
                              const TAttributeRule& attributeRule, rexsapi::TValueType type,
                              const rexsapi::TValue& value)
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
      setAttributeValue(data, layerAttribute, attributeRule, value.getValue<rexsapi::TBoolArrayType>());
      break;
    case rexsapi::TValueType::FLOATING_POINT_ARRAY:
      setAttributeValue(data, layerAttribute, attributeRule, value.getValue<rexsapi::TFloatArrayType>());
      break;
    case rexsapi::TValueType::INTEGER_ARRAY:
      setAttributeValue(data, layerAttribute, attributeRule, value.getValue<rexsapi::TIntArrayType>());
      break;
    case rexsapi::TValueType::ENUM_ARRAY:
      setAttributeValue(data, layerAttribute, attributeRule, value.getValue<rexsapi::TEnumArrayType>());
      break;
    case rexsapi::TValueType::STRING_ARRAY:
      setAttributeValue(data, layerAttribute, attributeRule, value.getValue<rexsapi::TStringArrayType>());
      break;
    case rexsapi::TValueType::REFERENCE_COMPONENT:
      // TODO (lcf)
      break;
    case rexsapi::TValueType::FLOATING_POINT_MATRIX:
      setAttributeValue(data, layerAttribute, attributeRule, value.getValue<rexsapi::TFloatMatrixType>());
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
      createSchemaValidator(basePath / "models" / "rexs-schema.xsd")};

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

    /* only bail out on critical errors
    if (!result) {
      // add some message
      return false;
    }*/

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
    TComponentRules componentRules{data.IntermediateLayer->getREXSVersion(), data.IntermediateLayer->getRules()};
    TAttributeRules attributeRules{data.IntermediateLayer->getREXSVersion(), data.IntermediateLayer->getRules()};

    for (const auto& component : components) {
      if (const auto* componentRule = componentRules.getRule(component); componentRule) {
        auto* new_layer_object = new TIntermediateLayerObject;
        new_layer_object->Id = component.getInternalId();
        new_layer_object->LayerObjectType = componentRule->Name_Side_2;
        new_layer_object->Name = component.getName();
        data.IntermediateLayer->IntermediateLayerObjects.emplace_back(new_layer_object);

        for (const auto& attribute : component.getAttributes()) {
          if (const auto* attributeRule = attributeRules.getRule(attribute); attributeRule) {
            if (!is_of_rexs_type(component, attributeRule->Object_Name_side_1, data.IntermediateLayer)) {
              continue;
            }
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
              case vector_dimension:
              case matrix_2D_dimension:
                setAttributeValue(data, *new_layer_attribute, *attributeRule, attribute.getValueType(),
                                  attribute.getValue());
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

    TRelationRules relationRules{data.IntermediateLayer->getREXSVersion(), data.IntermediateLayer->getRules()};

    for (const auto& relation : relations) {
      for (const auto* relationRule : relationRules.getRules(relation)) {
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
          new_relation_component->REXSObjectType = component.getType();

          ASSERT_OTHERWISE_THROW(new_relation->Comps[component_idx] == nullptr, "check failed");
          new_relation->Comps[component_idx] = new_relation_component;

          if (hasManufacturingStep(relation)) {
            int order_idx = static_cast<int>(relation.getOrder().value_or(1));
            ASSERT_OTHERWISE_THROW((order_idx >= 0) && (order_idx < 5), "Unexpected range!");
            new_relation->Comps[component_idx]->ObjectIndex = order_idx;
          }
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
        return component.getInternalId() == object->Id;
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
