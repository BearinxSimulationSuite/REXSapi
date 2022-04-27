
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

  class TXSDElement;

  class IXSDValidationContext
  {
  public:
    virtual ~IXSDValidationContext() = default;

    virtual const TXSDElement* findElement(const std::string& name) const = 0;
    virtual void pushElement(std::string element) = 0;
    virtual void popElement() = 0;
    virtual std::string getElementPath() const = 0;
    virtual void addError(const std::string& error) = 0;
    virtual bool hasErrors() const = 0;
    virtual void swap(std::vector<std::string>& errors) = 0;
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

    virtual void dump(std::ostream& out) const = 0;

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

    void validate(const pugi::xml_node& node, IXSDValidationContext& context) const;

    void dump(std::ostream& out) const;

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
        if (!element) {
          context.addError(fmt::format("unkown element '{}'", child.name()));
        }
      }
      for (const auto& element : m_Elements) {
        element.validate(node, context);
      }
    }

    void dump(std::ostream& out) const
    {
      for (const auto& element : m_Elements) {
        element.dump(out);
      }
    }

  private:
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

    void dump(std::ostream& out) const
    {
      out << "\tAttribute " << m_Name << " " << m_Type.getName() << (m_Required ? " required" : "");
    }

  private:
    const std::string m_Name;
    const TXSDType& m_Type;
    const bool m_Required;
  };

  class TXSDComplexType
  {
  public:
    TXSDComplexType(TXSDSequence&& sequence, std::vector<TXSDAttribute>&& attributes)
    : m_Sequence{std::move(sequence)}
    , m_Attributes{std::move(attributes)}
    {
    }

    void validate(const pugi::xml_node& node, IXSDValidationContext& context) const
    {
      for (const auto& attribute : m_Attributes) {
        attribute.validate(node, context);
      }
      m_Sequence.validate(node, context);
    }

    void dump(std::ostream& out) const
    {
      m_Sequence.dump(out);
      for (const auto& attribute : m_Attributes) {
        attribute.dump(out);
        out << std::endl;
      }
    }

  private:
    TXSDSequence m_Sequence;
    std::vector<TXSDAttribute> m_Attributes;
  };

  class TXSDSimpleType : public TXSDType
  {
  public:
    explicit TXSDSimpleType(std::string name)
    : TXSDType{std::move(name)}
    {
    }

    void validate(const std::string& value, IXSDValidationContext& context) const override
    {
      (void)value;
      (void)context;
    }

    void dump(std::ostream& out) const override
    {
      out << "SimpleType " << getName() << std::endl;
    }
  };

  class TXSDStringType : public TXSDType
  {
  public:
    TXSDStringType()
    : TXSDType{"xsd:string"}
    {
    }

    void validate(const std::string& value, IXSDValidationContext& context) const override
    {
      (void)value;
      (void)context;
    }

    void dump(std::ostream& out) const override
    {
      out << "StringType " << getName() << std::endl;
    }
  };


  class TXSDIntegerType : public TXSDType
  {
  public:
    TXSDIntegerType()
    : TXSDType{"xsd:integer"}
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

    void dump(std::ostream& out) const override
    {
      out << "IntegerType " << getName() << std::endl;
    }
  };

  class TXSDDecimalType : public TXSDType
  {
  public:
    TXSDDecimalType()
    : TXSDType{"xsd:decimal"}
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

    void dump(std::ostream& out) const override
    {
      out << "DecimalType " << getName() << std::endl;
    }
  };

  class TXSDBooleanType : public TXSDType
  {
  public:
    TXSDBooleanType()
    : TXSDType{"xsd:boolean"}
    {
    }

    void validate(const std::string& value, IXSDValidationContext& context) const override
    {
      if (value != "true" && value != "false" && value != "1" && value != "0") {
        context.addError(fmt::format("cannot convert '{}' to bool", value));
      }
    }

    void dump(std::ostream& out) const override
    {
      out << "BooleanType " << getName() << std::endl;
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

    const std::string& getName() const
    {
      return m_Name;
    }

    void validate(const pugi::xml_node& node, IXSDValidationContext& context) const
    {
      context.pushElement(getName());
      m_Type.validate(node, context);
      context.popElement();
    }

    void dump(std::ostream& out) const
    {
      out << "Element " << getName() << std::endl;
      m_Type.dump(out);
    }

  private:
    const std::string m_Name;
    const TXSDComplexType m_Type;
  };

  void TXSDElementRef::dump(std::ostream& out) const
  {
    out << "\tElementRef " << m_Element.getName() << " " << m_Min << " - " << m_Max << std::endl;
  }

  void TXSDElementRef::validate(const pugi::xml_node& node, IXSDValidationContext& context) const
  {
    auto nodes = node.select_nodes(m_Element.getName().c_str());

    if (nodes.empty() && m_Min != 0) {
      context.addError(fmt::format("missing element '{}'", m_Element.getName()));
    }
    if (nodes.size() < m_Min) {
      context.addError(fmt::format("too few '{}' elements, found {} instead of {}", m_Element.getName(), nodes.size(), m_Min));
    }
    if (nodes.size() > m_Max) {
      context.addError(fmt::format("too many '{}' elements, found {} instead of {}", m_Element.getName(), nodes.size(), m_Max));
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
    : m_Elements{elements}
    {
    }

    const TXSDElement* findElement(const std::string& name) const override
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

    std::string getElementPath() const override
    {
      std::stringstream stream;
      stream << "/";
      for (const auto& s : m_ElementStack) {
        stream << s << "/";
      }
      return stream.str();
    }

    void addError(const std::string& error) override
    {
      m_Errors.emplace_back(fmt::format("[{}] {}", getElementPath(), error));
    }

    bool hasErrors() const override
    {
      return !m_Errors.empty();
    }

    void swap(std::vector<std::string>& errors) override
    {
      errors.swap(m_Errors);
    }

  private:
    const TXSDElements& m_Elements;
    std::vector<std::string> m_ElementStack;
    std::vector<std::string> m_Errors;
  };

  class TXSDSchemaValidator
  {
  public:
    explicit TXSDSchemaValidator(const std::filesystem::path& xsdFile)
    {
      if (pugi::xml_parse_result parseResult = m_Doc.load_file(xsdFile.string().c_str()); !parseResult) {
        throw Exception{fmt::format("cannot open xsd schema '{}'", xsdFile.string())};
      }

      if (auto root = m_Doc.select_node("/xsd:schema"); !root) {
        throw Exception{fmt::format("'{}' is not an xsd schema", xsdFile.string())};
      }

      initTypes();

      for (const auto& elements : m_Doc.select_nodes("/xsd:schema/xsd:simpleType")) {
        // TODO (lcf): check for duplicates
        auto type = std::make_unique<TXSDSimpleType>(elements.node().attribute("name").as_string());
        m_Types.try_emplace(type->getName(), std::move(type));
      }

      for (const auto& elements : m_Doc.select_nodes("/xsd:schema/xsd:element")) {
        // TODO (lcf): check for duplicates
        auto element = parseElement(elements.node());
        m_Elements.try_emplace(element.getName(), std::move(element));
      }
    }

    bool validate(const pugi::xml_document& doc, std::vector<std::string>& errors) const
    {
      TXSDValidationContext context{m_Elements};

      for (const auto& node : doc.children()) {
        const auto* element = context.findElement(node.name());
        if (!element) {
          context.addError(fmt::format("element '{}' not found", node.name()));
          continue;
        }

        element->validate(node, context);
      }
      bool result = context.hasErrors();
      context.swap(errors);

      return !result;
    }

    void dump(std::ostream& out) const
    {
      out << "Types:\n";
      for (const auto& [_, type] : m_Types) {
        type->dump(out);
      }
      out << "\nElements:\n";
      for (const auto& [_, element] : m_Elements) {
        element.dump(out);
      }
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

    const TXSDElement* findElement(const std::string& name) const
    {
      auto it = m_Elements.find(name);
      return it == m_Elements.end() ? nullptr : &(it->second);
    }

    const TXSDElement& findOrRegisterElement(const std::string& name)
    {
      if (const auto* p = findElement(name); p) {
        return *p;
      }

      auto node = m_Doc.select_node(fmt::format("/xsd:schema/xsd:element[@name='{}']", name).c_str());
      if (!node) {
        throw Exception{fmt::format("no element node '{}' found", name)};
      }

      auto element = parseElement(node.node());
      auto [it, inserted] = m_Elements.try_emplace(element.getName(), std::move(element));
      return it->second;
    }

    const TXSDType& findType(const std::string& name) const
    {
      auto it = m_Types.find(name);
      if (it == m_Types.end()) {
        throw Exception{fmt::format("no type '{}' found", name)};
      }
      return *it->second;
    }

    TXSDElement parseElement(const pugi::xml_node& node)
    {
      TXSDSequence sequence;

      for (const auto& element : node.select_nodes("xsd:complexType/xsd:sequence/xsd:element")) {
        const TXSDElement& ref = findOrRegisterElement(element.node().attribute("ref").as_string());
        auto min = convertToUint64(element.node().attribute("minOccurs").as_string());
        auto maxString = std::string(element.node().attribute("maxOccurs").as_string());
        auto max = maxString == "unbounded" ? std::numeric_limits<uint64_t>::max() : convertToUint64(maxString);
        sequence.addElementRef(ref, min, max);
      }

      std::vector<TXSDAttribute> attributes;
      for (const auto& attribute : node.select_nodes("xsd:complexType/xsd:attribute")) {
        auto use = attribute.node().attribute("use");
        auto typeName = attribute.node().attribute("type").as_string();
        const auto& type = findType(typeName);
        TXSDAttribute att{attribute.node().attribute("name").as_string(), type,
                          use ? std::string(use.as_string()) == "required" : false};
        attributes.emplace_back(att);
      }


      TXSDComplexType complexType{std::move(sequence), std::move(attributes)};
      return TXSDElement{node.attribute("name").as_string(), std::move(complexType)};
    }

    pugi::xml_document m_Doc;
    TXSDTypes m_Types;
    TXSDElements m_Elements;
  };
}

#endif
