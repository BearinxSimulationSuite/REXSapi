
#ifndef REXSAPI_XML_XSD_SCHEMA_VALIDATOR_HXX
#define REXSAPI_XML_XSD_SCHEMA_VALIDATOR_HXX

#include <rexsapi/ConversionHelper.hxx>
#include <rexsapi/Exception.hxx>
#include <rexsapi/Format.hxx>
#include <rexsapi/XMLParser.hxx>

#include <filesystem>
#include <iostream>
#include <limits>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace rexsapi::xml
{
  static constexpr const char* xsdSchemaNS = "xsd";

  class TXSDElement;

  class IXSDValidationContext
  {
  public:
    IXSDValidationContext() = default;
    virtual ~IXSDValidationContext() = default;

    IXSDValidationContext(const IXSDValidationContext&) = default;
    IXSDValidationContext(IXSDValidationContext&&) = default;
    IXSDValidationContext& operator=(const IXSDValidationContext&) = default;
    IXSDValidationContext& operator=(IXSDValidationContext&&) = default;

    [[nodiscard]] virtual const TXSDElement* findElement(const std::string& name) const = 0;
    virtual void pushElement(std::string element) = 0;
    virtual void popElement() = 0;
    [[nodiscard]] virtual std::string getElementPath() const = 0;
    virtual void addError(std::string error) = 0;
    [[nodiscard]] virtual bool hasErrors() const = 0;
    virtual void swap(std::vector<std::string>& errors) noexcept = 0;
  };

  class TXSDType
  {
  public:
    using Ptr = std::unique_ptr<TXSDType>;

    explicit TXSDType(std::string name)
    : m_Name{std::move(name)}
    {
    }

    virtual ~TXSDType() = default;

    virtual void validate(const std::string& value, IXSDValidationContext& context) const = 0;

    const std::string& getName() const
    {
      return m_Name;
    }

  private:
    const std::string m_Name;
  };


  class TXSDElementRef
  {
  public:
    explicit TXSDElementRef(const TXSDElement& element, uint64_t min, uint64_t max)
    : m_Element{element}
    , m_Min{min}
    , m_Max{max}
    {
    }

    [[nodiscard]] const std::string& getName() const;

    void validate(const pugi::xml_node& node, IXSDValidationContext& context) const;

  private:
    const TXSDElement& m_Element;
    uint64_t m_Min;
    uint64_t m_Max;
  };

  class TXSDSequence
  {
  public:
    void addElementRef(const TXSDElement& element, uint64_t min, uint64_t max)
    {
      m_Elements.emplace_back(TXSDElementRef{element, min, max});
    }

    void validate(const pugi::xml_node& node, IXSDValidationContext& context) const
    {
      for (const auto& child : node.children()) {
        const auto* element = context.findElement(child.name());
        if (element == nullptr) {
          context.addError(fmt::format("unkown element '{}'", child.name()));
        } else if (!checkContainsElement(child.name())) {
          context.addError(fmt::format("element '{}' is not allowed here", child.name()));
        }
      }
      for (const auto& element : m_Elements) {
        element.validate(node, context);
      }
    }

  private:
    [[nodiscard]] bool checkContainsElement(const std::string& child) const
    {
      auto it = std::find_if(m_Elements.begin(), m_Elements.end(), [&child](const auto& element) {
        return child == element.getName();
      });

      return it != m_Elements.end();
    }

    std::vector<TXSDElementRef> m_Elements;
  };

  class TXSDAttribute
  {
  public:
    explicit TXSDAttribute(std::string name, const TXSDType& type, bool required)
    : m_Name{std::move(name)}
    , m_Type{type}
    , m_Required{required}
    {
    }

    [[nodiscard]] const std::string& getName() const
    {
      return m_Name;
    }

    void validate(const pugi::xml_node& node, IXSDValidationContext& context) const
    {
      auto attribute = node.attribute(m_Name.c_str());
      if (!attribute && m_Required) {
        context.addError(fmt::format("missing required attribute '{}'", m_Name));
      }
      context.pushElement(m_Name);
      if (attribute) {
        m_Type.validate(attribute.value(), context);
      }
      context.popElement();
    }

  private:
    const std::string m_Name;
    const TXSDType& m_Type;
    const bool m_Required;
  };

  class TXSDAttributes
  {
  public:
    void addAttribute(const TXSDAttribute& attribute)
    {
      m_Attributes.emplace_back(attribute);
    }

    void validate(const pugi::xml_node& node, IXSDValidationContext& context) const
    {
      for (const auto& attribute : node.attributes()) {
        if (!containsAttribute(attribute.name())) {
          context.addError(fmt::format("unknown attribute '{}'", attribute.name()));
        }
      }

      for (const auto& attribute : m_Attributes) {
        attribute.validate(node, context);
      }
    }

  private:
    [[nodiscard]] bool containsAttribute(const std::string& attribute) const
    {
      auto it = std::find_if(m_Attributes.begin(), m_Attributes.end(), [&attribute](const auto& att) {
        return att.getName() == attribute;
      });
      return it != m_Attributes.end();
    }
    std::vector<TXSDAttribute> m_Attributes;
  };

  class TXSDComplexType
  {
  public:
    TXSDComplexType(TXSDSequence&& sequence, TXSDAttributes&& attributes)
    : m_Sequence{std::move(sequence)}
    , m_Attributes{std::move(attributes)}
    {
    }

    void validate(const pugi::xml_node& node, IXSDValidationContext& context) const
    {
      m_Attributes.validate(node, context);
      m_Sequence.validate(node, context);
    }

  private:
    TXSDSequence m_Sequence;
    TXSDAttributes m_Attributes;
  };

  class TXSDSimpleEnumType : public TXSDType
  {
  public:
    explicit TXSDSimpleEnumType(std::string name, std::vector<std::string> enumValues)
    : TXSDType{std::move(name)}
    , m_EnumValues{std::move(enumValues)}
    {
    }

    void validate(const std::string& value, IXSDValidationContext& context) const override
    {
      auto it = std::find_if(m_EnumValues.begin(), m_EnumValues.end(), [&value](const auto& item) {
        return item == value;
      });
      if (it == m_EnumValues.end()) {
        context.addError(fmt::format("unknown enum value '{}' for type '{}'", value, getName()));
      }
    }

  private:
    std::vector<std::string> m_EnumValues;
  };

  class TXSDStringType : public TXSDType
  {
  public:
    TXSDStringType()
    : TXSDType{fmt::format("{}:string", xsdSchemaNS)}
    {
    }

    void validate(const std::string& value, IXSDValidationContext& context) const override
    {
      (void)value;
      (void)context;
    }
  };


  class TXSDIntegerType : public TXSDType
  {
  public:
    TXSDIntegerType()
    : TXSDType{fmt::format("{}:integer", xsdSchemaNS)}
    {
    }

    void validate(const std::string& value, IXSDValidationContext& context) const override
    {
      try {
        std::size_t pos{};
        std::stoll(value, &pos);
        if (pos != value.length()) {
          context.addError(fmt::format("cannot convert '{}' to integer", value));
        }
      } catch (const std::exception&) {
        context.addError(fmt::format("cannot convert '{}' to integer", value));
      }
    }
  };

  class TXSDDecimalType : public TXSDType
  {
  public:
    TXSDDecimalType()
    : TXSDType{fmt::format("{}:decimal", xsdSchemaNS)}
    {
    }

    void validate(const std::string& value, IXSDValidationContext& context) const override
    {
      try {
        std::size_t pos{};
        std::stod(value, &pos);
        if (pos != value.length()) {
          context.addError(fmt::format("cannot convert '{}' to decimal", value));
        }
      } catch (const std::exception&) {
        context.addError(fmt::format("cannot convert '{}' to decimal", value));
      }
    }
  };

  class TXSDBooleanType : public TXSDType
  {
  public:
    TXSDBooleanType()
    : TXSDType{fmt::format("{}:boolean", xsdSchemaNS)}
    {
    }

    void validate(const std::string& value, IXSDValidationContext& context) const override
    {
      if (value != "true" && value != "false" && value != "1" && value != "0") {
        context.addError(fmt::format("cannot convert '{}' to bool", value));
      }
    }
  };

  class TXSDElement
  {
  public:
    explicit TXSDElement(std::string name, TXSDComplexType type)
    : m_Name{std::move(name)}
    , m_Type{std::move(type)}
    {
    }

    [[nodiscard]] const std::string& getName() const
    {
      return m_Name;
    }

    void validate(const pugi::xml_node& node, IXSDValidationContext& context) const
    {
      context.pushElement(getName());
      m_Type.validate(node, context);
      context.popElement();
    }

  private:
    const std::string m_Name;
    const TXSDComplexType m_Type;
  };

  [[nodiscard]] const std::string& TXSDElementRef::getName() const
  {
    return m_Element.getName();
  }

  void TXSDElementRef::validate(const pugi::xml_node& node, IXSDValidationContext& context) const
  {
    auto nodes = node.select_nodes(m_Element.getName().c_str());

    if (nodes.size() < m_Min) {
      context.addError(
        fmt::format("too few '{}' elements, found {} instead of at least {}", m_Element.getName(), nodes.size(), m_Min));
    }
    if (nodes.size() > m_Max) {
      context.addError(
        fmt::format("too many '{}' elements, found {} instead of at most {}", m_Element.getName(), nodes.size(), m_Max));
    }

    for (const auto& child : nodes) {
      m_Element.validate(child.node(), context);
    }
  }

  using TXSDTypes = std::unordered_map<std::string, TXSDType::Ptr>;
  using TXSDElements = std::unordered_map<std::string, TXSDElement>;

  class TXSDValidationContext : public IXSDValidationContext
  {
  public:
    explicit TXSDValidationContext(const TXSDElements& elements)
    : IXSDValidationContext{}
    , m_Elements{elements}
    {
    }

    [[nodiscard]] const TXSDElement* findElement(const std::string& name) const override
    {
      auto it = m_Elements.find(name);
      if (it == m_Elements.end()) {
        return nullptr;
      }
      return &it->second;
    }

    void pushElement(std::string element) override
    {
      m_ElementStack.emplace_back(std::move(element));
    }

    void popElement() override
    {
      m_ElementStack.pop_back();
    }

    [[nodiscard]] std::string getElementPath() const override
    {
      std::stringstream stream;
      stream << "/";
      for (const auto& s : m_ElementStack) {
        stream << s << "/";
      }
      return stream.str();
    }

    void addError(std::string error) override
    {
      m_Errors.emplace_back(fmt::format("[{}] {}", getElementPath(), std::move(error)));
    }

    [[nodiscard]] bool hasErrors() const override
    {
      return !m_Errors.empty();
    }

    void swap(std::vector<std::string>& errors) noexcept override
    {
      errors.swap(m_Errors);
    }

  private:
    const TXSDElements& m_Elements;
    std::vector<std::string> m_ElementStack;
    std::vector<std::string> m_Errors;
  };

  template<typename TXsdSchemaLoader>
  class TXSDSchemaValidator
  {
  public:
    explicit TXSDSchemaValidator(const TXsdSchemaLoader& loader)
    : m_Doc{loader.load()}
    {
      if (auto root = m_Doc.select_node(fmt::format("/{}:schema", xsdSchemaNS).c_str()); !root) {
        throw TException{fmt::format("{}:schema node not found", xsdSchemaNS)};
      }

      initTypes();

      for (const auto& elements : m_Doc.select_nodes(fmt::format("/{0}:schema/{0}:simpleType", xsdSchemaNS).c_str())) {
        // ATTENTION: this is a strong simplification of simple types
        std::vector<std::string> enumValues;
        for (const auto& enums :
             elements.node().select_nodes(fmt::format("{0}:restriction/{0}:enumeration", xsdSchemaNS).c_str())) {
          enumValues.emplace_back(enums.node().attribute("value").as_string());
        }
        auto type = std::make_unique<TXSDSimpleEnumType>(elements.node().attribute("name").as_string(), enumValues);
        // TODO (lcf): check for duplicates
        m_Types.try_emplace(type->getName(), std::move(type));
      }

      for (const auto& elements : m_Doc.select_nodes(fmt::format("/{0}:schema/{0}:element", xsdSchemaNS).c_str())) {
        // TODO (lcf): check for duplicates
        auto element = parseElement(elements.node());
        m_Elements.try_emplace(element.getName(), std::move(element));
      }
    }

    [[nodiscard]] bool validate(const pugi::xml_document& doc, std::vector<std::string>& errors) const
    {
      TXSDValidationContext context{m_Elements};

      for (const auto& node : doc.children()) {
        const auto* element = context.findElement(node.name());
        if (!element) {
          context.addError(fmt::format("unknown element '{}'", node.name()));
          continue;
        }

        element->validate(node, context);
      }
      bool result = context.hasErrors();
      context.swap(errors);

      return !result;
    }

  private:
    void initTypes()
    {
      auto type1 = std::make_unique<TXSDStringType>();
      m_Types.try_emplace(type1->getName(), std::move(type1));
      auto type2 = std::make_unique<TXSDIntegerType>();
      m_Types.try_emplace(type2->getName(), std::move(type2));
      auto type3 = std::make_unique<TXSDDecimalType>();
      m_Types.try_emplace(type3->getName(), std::move(type3));
      auto type4 = std::make_unique<TXSDBooleanType>();
      m_Types.try_emplace(type4->getName(), std::move(type4));
    }

    [[nodiscard]] const TXSDElement* findElement(const std::string& name) const
    {
      auto it = m_Elements.find(name);
      return it == m_Elements.end() ? nullptr : &(it->second);
    }

    [[nodiscard]] const TXSDElement& findOrRegisterElement(const std::string& name)
    {
      if (const auto* p = findElement(name); p) {
        return *p;
      }

      auto node = m_Doc.select_node(fmt::format("/{0}:schema/{0}:element[@name='{1}']", xsdSchemaNS, name).c_str());
      if (!node) {
        throw TException{fmt::format("no element node '{}' found", name)};
      }

      auto element = parseElement(node.node());
      auto [it, inserted] = m_Elements.try_emplace(element.getName(), std::move(element));
      return it->second;
    }

    [[nodiscard]] const TXSDType& findType(const std::string& name) const
    {
      auto it = m_Types.find(name);
      if (it == m_Types.end()) {
        throw TException{fmt::format("no type '{}' found", name)};
      }
      return *it->second;
    }

    [[nodiscard]] TXSDElement parseElement(const pugi::xml_node& node)
    {
      TXSDSequence sequence;

      for (const auto& element :
           node.select_nodes(fmt::format("{0}:complexType/{0}:sequence/{0}:element", xsdSchemaNS).c_str())) {
        const TXSDElement& ref = findOrRegisterElement(element.node().attribute("ref").as_string());
        auto min = convertToUint64(element.node().attribute("minOccurs").as_string());
        auto maxString = std::string(element.node().attribute("maxOccurs").as_string());
        auto max = maxString == "unbounded" ? std::numeric_limits<uint64_t>::max() : convertToUint64(maxString);
        sequence.addElementRef(ref, min, max);
      }

      TXSDAttributes attributes;
      for (const auto& attribute : node.select_nodes(fmt::format("{0}:complexType/{0}:attribute", xsdSchemaNS).c_str())) {
        auto use = attribute.node().attribute("use");
        auto typeName = attribute.node().attribute("type").as_string();
        const auto& type = findType(typeName);

        attributes.addAttribute(TXSDAttribute{attribute.node().attribute("name").as_string(), type,
                                              use ? std::string(use.as_string()) == "required" : false});
      }


      TXSDComplexType complexType{std::move(sequence), std::move(attributes)};
      return TXSDElement{node.attribute("name").as_string(), std::move(complexType)};
    }

    pugi::xml_document m_Doc;
    TXSDTypes m_Types;
    TXSDElements m_Elements;
  };

  class TFileXsdSchemaLoader
  {
  public:
    explicit TFileXsdSchemaLoader(std::filesystem::path xsdFile)
    : m_XsdFile{std::move(xsdFile)}
    {
    }

    [[nodiscard]] pugi::xml_document load() const
    {
      pugi::xml_document doc;
      if (pugi::xml_parse_result parseResult = doc.load_file(m_XsdFile.string().c_str()); !parseResult) {
        throw TException{fmt::format("cannot parse xsd schema file '{}': {}", m_XsdFile.string(), parseResult.description())};
      }

      return doc;
    }

  private:
    std::filesystem::path m_XsdFile;
  };

  class TBufferXsdSchemaLoader
  {
  public:
    explicit TBufferXsdSchemaLoader(std::string xsdSchema)
    : m_XsdSchema{std::move(xsdSchema)}
    {
    }

    [[nodiscard]] pugi::xml_document load() const
    {
      pugi::xml_document doc;
      if (pugi::xml_parse_result parseResult = doc.load_string(m_XsdSchema.c_str()); !parseResult) {
        throw TException{fmt::format("cannot parse xsd schema: {}", parseResult.description())};
      }

      return doc;
    }

  private:
    std::string m_XsdSchema;
  };
}

#endif
