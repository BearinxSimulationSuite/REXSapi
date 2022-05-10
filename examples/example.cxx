
#include <rexsapi/Rexsapi.hxx>

#include <iostream>

struct TREXSVersionNumber {
  TREXSVersionNumber() = default;

  TREXSVersionNumber(unsigned int major, unsigned int minor)
  : m_MajorVersionNr{major}
  , m_MinorVersionNr{minor}
  {
  }

  unsigned int m_MajorVersionNr{0};
  unsigned int m_MinorVersionNr{0};
};

enum TSideType { REXS_component, REXS_component_group };

enum TDirection { bidirectional, REXS_to_Bearinx };

enum TAttributeDimension { scalar_dimension };

enum TAttributeType { integer_type, reference_type };

class TRule
{
public:
  virtual ~TRule() = default;
};

class TComponentRule : public TRule
{
public:
  TSideType Type_Side_1;
  TDirection Direction;
  std::string Name_Side_1;
  std::string Name_Side_2;
  TREXSVersionNumber From_REXS_Version;
  TREXSVersionNumber To_REXS_Version;
};

class TAttributeRule : public TRule
{
public:
  TSideType Type_Side_1;
  TDirection Direction;
  std::string Attribute_Name_side_1;
  std::string Attribute_Name_side_2;
  TAttributeType Attribute_Type;
  TAttributeDimension Attribute_Dimension;
  TREXSVersionNumber From_REXS_Version;
  TREXSVersionNumber To_REXS_Version;
};

struct TIntermediateLayerAttribute {
  void setAttributeName(std::string name)
  {
    Name = std::move(name);
  }

  void setAttributeType(TAttributeType type)
  {
    Type = type;
  }

  void setAttributeDimension(TAttributeDimension dimension)
  {
    Dimension = dimension;
  }

  std::string Name;
  TAttributeType Type;
  TAttributeDimension Dimension;
};

struct TIntermediateLayerObject {
  std::string LayerObjectType;
  std::string Name;

  std::vector<TIntermediateLayerAttribute*> IntermediateLayerAttributes;
};

struct TREXSTransmissionModelIntermediateLayer {
  void setREXSVersion(TREXSVersionNumber version)
  {
    m_Version = version;
  }

  TREXSVersionNumber m_Version;
  std::vector<TIntermediateLayerObject*> IntermediateLayerObjects;
};

struct Data {
  TREXSTransmissionModelIntermediateLayer* IntermediateLayer;
};

class TComponentRules
{
public:
  TComponentRules(const TREXSVersionNumber& rexs_version, std::vector<TRule*> rules)
  {
    for (const auto* rule : rules) {
      if (const auto* componentRule = dynamic_cast<const TComponentRule*>(rule); componentRule) {
        if (componentRule->Type_Side_1 == REXS_component &&
            ((componentRule->Direction == bidirectional || componentRule->Direction == REXS_to_Bearinx) &&
             (rexs_version.m_MajorVersionNr > componentRule->From_REXS_Version.m_MajorVersionNr) &&
             (rexs_version.m_MajorVersionNr < componentRule->To_REXS_Version.m_MajorVersionNr))) {
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
  TAttributeRules(const TREXSVersionNumber& rexs_version, std::vector<TRule*> rules)
  {
    for (const auto* rule : rules) {
      if (const auto* attributeRule = dynamic_cast<const TAttributeRule*>(rule); attributeRule) {
        if ((attributeRule->Type_Side_1 == REXS_component || attributeRule->Type_Side_1 == REXS_component_group) &&
            ((attributeRule->Direction == bidirectional || attributeRule->Direction == REXS_to_Bearinx) &&
             (rexs_version.m_MajorVersionNr > attributeRule->From_REXS_Version.m_MajorVersionNr) &&
             (rexs_version.m_MajorVersionNr < attributeRule->To_REXS_Version.m_MajorVersionNr))) {
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

    return true;
  }

  bool fillIntermediateLayerComponents(Data& data, const rexsapi::TComponents& components) const
  {
    TComponentRules componentRules{data.IntermediateLayer->m_Version, m_Rules};
    TAttributeRules attributeRules{data.IntermediateLayer->m_Version, m_Rules};

    for (const auto& component : components) {
      if (const auto* componentRule = componentRules.getRule(component); componentRule) {
        auto* new_layer_object = new TIntermediateLayerObject;
        new_layer_object->LayerObjectType = componentRule->Name_Side_2;
        new_layer_object->Name = component.getName();
        data.IntermediateLayer->IntermediateLayerObjects.emplace_back(new_layer_object);

        for (const auto& attribute : component.getAttributes()) {
          if (const auto* attributeRule = attributeRules.getRule(attribute); attributeRule) {
            auto* new_layer_attribute = new TIntermediateLayerAttribute;
            new_layer_attribute->setAttributeName(attributeRule->Attribute_Name_side_2);
            new_layer_attribute->setAttributeType(attributeRule->Attribute_Type);
            new_layer_attribute->setAttributeDimension(attributeRule->Attribute_Dimension);
            new_layer_object->IntermediateLayerAttributes.emplace_back(new_layer_attribute);
          }
        }
      }
    }

    return false;
  }

  std::vector<TRule*> m_Rules{};
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
