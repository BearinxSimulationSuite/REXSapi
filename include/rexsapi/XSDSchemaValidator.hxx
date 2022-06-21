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

#ifndef REXSAPI_XSD_SCHEMA_VALIDATOR_HXX
#define REXSAPI_XSD_SCHEMA_VALIDATOR_HXX

#include <rexsapi/ConversionHelper.hxx>
#include <rexsapi/Exception.hxx>
#include <rexsapi/Format.hxx>
#include <rexsapi/Xml.hxx>

#include <filesystem>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace rexsapi::xml
{
  static constexpr const char* xsdSchemaNS = "xsd";

  class TElement;
  class TValidationContext;


  class TSimpleType
  {
  public:
    using Ptr = std::unique_ptr<TSimpleType>;

    explicit TSimpleType(std::string name)
    : m_Name{std::move(name)}
    {
    }

    virtual ~TSimpleType() = default;

    virtual void validate(const std::string& value, TValidationContext& context) const = 0;

    [[nodiscard]] const std::string& getName() const&
    {
      return m_Name;
    }

  protected:
    TSimpleType(const TSimpleType&) = default;
    TSimpleType& operator=(const TSimpleType&) = delete;
    TSimpleType(TSimpleType&&) = default;
    TSimpleType& operator=(TSimpleType&&) = delete;

  private:
    std::string m_Name;
  };


  class TElementType
  {
  public:
    using Ptr = std::unique_ptr<TElementType>;

    explicit TElementType(std::string name)
    : m_Name{std::move(name)}
    {
    }

    virtual ~TElementType() = default;

    virtual void validate(const pugi::xml_node& node, TValidationContext& context) const = 0;

    [[nodiscard]] const std::string& getName() const&
    {
      return m_Name;
    }

  protected:
    TElementType(const TElementType&) = default;
    TElementType& operator=(const TElementType&) = delete;
    TElementType(TElementType&&) = default;
    TElementType& operator=(TElementType&&) = delete;

  private:
    std::string m_Name;
  };


  class TElement
  {
  public:
    explicit TElement(std::string name, TElementType::Ptr&& type)
    : m_Name{std::move(name)}
    , m_Type{std::move(type)}
    {
    }

    TElement(const TElement&) = delete;
    TElement(TElement&&) = default;
    TElement& operator=(const TElement&) = delete;
    TElement& operator=(TElement&&) = delete;

    [[nodiscard]] const std::string& getName() const&
    {
      return m_Name;
    }

    void validate(const pugi::xml_node& node, TValidationContext& context) const;

  private:
    std::string m_Name;
    TElementType::Ptr m_Type;
  };


  class TElementRef
  {
  public:
    explicit TElementRef(const TElement& element, uint64_t min, uint64_t max)
    : m_Element{element}
    , m_Min{min}
    , m_Max{max}
    {
    }

    [[nodiscard]] const std::string& getName() const&;

    void validate(const pugi::xml_node& node, TValidationContext& context) const;

  private:
    const TElement& m_Element;
    uint64_t m_Min;
    uint64_t m_Max;
  };


  class TSequence
  {
  public:
    void addElementRef(const TElement& element, uint64_t min, uint64_t max);
    void addDirectElement(const std::string& name, const TSimpleType& type, uint64_t min, uint64_t max);

    void validate(const pugi::xml_node& node, TValidationContext& context) const;

  private:
    [[nodiscard]] bool checkContainsElement(const std::string& child) const;

    std::vector<TElementRef> m_Elements;
    std::unordered_map<std::string, TElement> m_DirectElements;
  };


  class TAttribute
  {
  public:
    explicit TAttribute(std::string name, const TSimpleType& type, bool required)
    : m_Name{std::move(name)}
    , m_Type{type}
    , m_Required{required}
    {
    }

    [[nodiscard]] const std::string& getName() const&
    {
      return m_Name;
    }

    void validate(const pugi::xml_node& node, TValidationContext& context) const;

  private:
    std::string m_Name;
    const TSimpleType& m_Type;
    bool m_Required;
  };


  enum class TAttributeMode { STRICT, RELAXED };

  class TAttributes
  {
  public:
    void setRelaxed();

    void addAttribute(const TAttribute& attribute);

    void validate(const pugi::xml_node& node, TValidationContext& context) const;

  private:
    [[nodiscard]] bool containsAttribute(const std::string& attribute) const;

    TAttributeMode m_Relaxed{TAttributeMode::STRICT};
    std::vector<TAttribute> m_Attributes;
  };


  class TText
  {
  public:
    void validate(const pugi::xml_node& node, TValidationContext& context) const;
  };

  class TComplexType : public TElementType
  {
  public:
    TComplexType(std::string name, TSequence&& sequence, TAttributes&& attributes, std::optional<TText>&& text)
    : TElementType{std::move(name)}
    , m_Sequence{std::move(sequence)}
    , m_Attributes{std::move(attributes)}
    , m_Text{std::move(text)}
    {
    }

    void validate(const pugi::xml_node& node, TValidationContext& context) const override;

  private:
    TSequence m_Sequence;
    TAttributes m_Attributes;
    std::optional<TText> m_Text;
  };


  class TInlineContentType : public TElementType
  {
  public:
    TInlineContentType(std::string name, const TSimpleType& type)
    : TElementType{std::move(name)}
    , m_Type{type}
    {
    }

    void validate(const pugi::xml_node& node, TValidationContext& context) const override;

  private:
    const TSimpleType& m_Type;
  };


  class TSimpleContentType : public TElementType
  {
  public:
    TSimpleContentType(std::string name, const TSimpleType& type)
    : TElementType{std::move(name)}
    , m_Type{type}
    {
    }

    void validate(const pugi::xml_node& node, TValidationContext& context) const override;

  private:
    const TSimpleType& m_Type;
  };


  using TSimpleTypes = std::unordered_map<std::string, TSimpleType::Ptr>;
  using TElements = std::unordered_map<std::string, TElement>;


  class TXSDSchemaValidator
  {
  public:
    template<typename TXsdSchemaLoader>
    explicit TXSDSchemaValidator(const TXsdSchemaLoader& loader)
    : m_Doc{loader.load()}
    {
      init();
    }

    [[nodiscard]] bool validate(const pugi::xml_document& doc, std::vector<std::string>& errors) const;

  private:
    void init();
    void initTypes();

    [[nodiscard]] const TElement* findElement(const std::string& name) const&;

    [[nodiscard]] const TElement& findOrRegisterElement(const std::string& name);

    [[nodiscard]] const TSimpleType& findType(const std::string& name) const&;

    [[nodiscard]] TElement parseElement(const pugi::xml_node& node);

    pugi::xml_document m_Doc;
    TSimpleTypes m_Types;
    TElements m_Elements;
  };


  class TFileXsdSchemaLoader
  {
  public:
    explicit TFileXsdSchemaLoader(std::filesystem::path xsdFile)
    : m_XsdFile{std::move(xsdFile)}
    {
      if (!std::filesystem::exists(m_XsdFile) || !std::filesystem::is_regular_file(m_XsdFile)) {
        throw TException{fmt::format("{} does not exist or is not a regular file", m_XsdFile.string())};
      }
    }

    [[nodiscard]] pugi::xml_document load() const;

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

    explicit TBufferXsdSchemaLoader(const char* xsdSchema)
    : m_XsdSchema{xsdSchema}
    {
    }

    [[nodiscard]] pugi::xml_document load() const;

  private:
    std::string m_XsdSchema;
  };


  class TEnumeration
  {
  public:
    explicit TEnumeration(std::string name, std::vector<std::string> enumValues)
    : m_Name{std::move(name)}
    , m_EnumValues{std::move(enumValues)}
    {
    }

    void validate(const std::string& value, TValidationContext& context) const;

  private:
    std::string m_Name;
    std::vector<std::string> m_EnumValues;
  };


  class TRestrictedType : public TSimpleType
  {
  public:
    explicit TRestrictedType(std::string name, const TSimpleType& baseType, std::optional<TEnumeration>&& enumeration)
    : TSimpleType{std::move(name)}
    , m_BaseType{baseType}
    , m_Enumeration{std::move(enumeration)}
    {
    }

    void validate(const std::string& value, TValidationContext& context) const override;

  private:
    const TSimpleType& m_BaseType;
    std::optional<TEnumeration> m_Enumeration;
  };


  class TStringType
  {
  public:
    void validate(const std::string& value, TValidationContext& context) const;
  };


  class TIntegerType
  {
  public:
    void validate(const std::string& value, TValidationContext& context) const;
  };


  class TNonNegativeIntegerType
  {
  public:
    void validate(const std::string& value, TValidationContext& context) const;
  };


  class TDecimalType
  {
  public:
    void validate(const std::string& value, TValidationContext& context) const;
  };


  class TBooleanType
  {
  public:
    void validate(const std::string& value, TValidationContext& context) const;
  };


  class TValidationContext
  {
  public:
    explicit TValidationContext(const TElements& elements)
    : m_Elements{elements}
    {
    }

    [[nodiscard]] const TElement* findElement(const std::string& name) const&;

    void pushElement(std::string element);

    void popElement();

    [[nodiscard]] std::string getElementPath() const;

    void addError(std::string error);

    [[nodiscard]] bool hasErrors() const;

    void swap(std::vector<std::string>& errors) noexcept;

  private:
    const TElements& m_Elements;
    std::vector<std::string> m_ElementStack;
    std::vector<std::string> m_Errors;
  };


  template<typename T>
  class TPodType : public TSimpleType
  {
  public:
    explicit TPodType(std::string name)
    : TSimpleType{std::move(name)}
    {
    }

    void validate(const std::string& value, TValidationContext& context) const override;

  private:
    T m_Type;
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  template<typename T>
  inline void TPodType<T>::validate(const std::string& value, TValidationContext& context) const
  {
    m_Type.validate(value, context);
  }


  inline void TStringType::validate(const std::string& value, TValidationContext& context) const
  {
    // nothing to do here
    (void)value;
    (void)context;
  }

  inline void TIntegerType::validate(const std::string& value, TValidationContext& context) const
  {
    try {
      std::size_t pos{};
      (void)std::stoll(value, &pos);
      if (pos != value.length()) {
        context.addError(fmt::format("cannot convert '{}' to integer", value));
      }
    } catch (const std::exception&) {
      context.addError(fmt::format("cannot convert '{}' to integer", value));
    }
  }

  inline void TNonNegativeIntegerType::validate(const std::string& value, TValidationContext& context) const
  {
    try {
      (void)convertToUint64(value);
    } catch (const std::exception&) {
      context.addError(fmt::format("cannot convert '{}' to non negative integer", value));
    }
  }

  inline void TDecimalType::validate(const std::string& value, TValidationContext& context) const
  {
    try {
      (void)convertToDouble(value);
    } catch (const std::exception&) {
      context.addError(fmt::format("cannot convert '{}' to decimal", value));
    }
  }

  inline void TBooleanType::validate(const std::string& value, TValidationContext& context) const
  {
    if (value != "true" && value != "false" && value != "1" && value != "0") {
      context.addError(fmt::format("cannot convert '{}' to bool", value));
    }
  }

  inline void TSequence::addElementRef(const TElement& element, uint64_t min, uint64_t max)
  {
    m_Elements.emplace_back(TElementRef{element, min, max});
  }

  inline void TSequence::addDirectElement(const std::string& name, const TSimpleType& type, uint64_t min, uint64_t max)
  {
    TElement element{name, std::make_unique<TInlineContentType>(name, type)};
    auto [iter, success] = m_DirectElements.emplace(name, std::move(element));
    if (!success) {
      throw TException{fmt::format("element '{}' has already been added to sequence", name)};
    }
    addElementRef(iter->second, min, max);
  }

  inline void TSequence::validate(const pugi::xml_node& node, TValidationContext& context) const
  {
    for (const auto& child : node.children()) {
      const std::string childName = child.name();
      if (childName.empty()) {
        continue;
      }
      if (const auto* element = context.findElement(childName); element == nullptr) {
        auto it = std::find_if(m_DirectElements.begin(), m_DirectElements.end(), [&childName](const auto& item) {
          return item.second.getName() == childName;
        });
        if (it == m_DirectElements.end()) {
          context.addError(fmt::format("unkown element '{}'", childName));
        }
      } else if (!checkContainsElement(childName)) {
        context.addError(fmt::format("element '{}' is not allowed here", childName));
      }
    }
    std::for_each(m_Elements.begin(), m_Elements.end(), [&node, &context](const auto& element) {
      element.validate(node, context);
    });
    std::for_each(m_DirectElements.begin(), m_DirectElements.end(), [&node, &context](const auto& element) {
      element.second.validate(node, context);
    });
  }

  [[nodiscard]] inline bool TSequence::checkContainsElement(const std::string& child) const
  {
    auto it = std::find_if(m_Elements.begin(), m_Elements.end(), [&child](const auto& element) {
      return child == element.getName();
    });

    return it != m_Elements.end();
  }

  inline void TAttribute::validate(const pugi::xml_node& node, TValidationContext& context) const
  {
    auto attribute = node.attribute(m_Name.c_str());
    if (attribute.empty() && m_Required) {
      context.addError(fmt::format("missing required attribute '{}'", m_Name));
    }
    context.pushElement(m_Name);
    if (!attribute.empty()) {
      m_Type.validate(attribute.value(), context);
    }
    context.popElement();
  }


  [[nodiscard]] inline const std::string& TElementRef::getName() const&
  {
    return m_Element.getName();
  }

  inline void TElementRef::validate(const pugi::xml_node& node, TValidationContext& context) const
  {
    auto nodes = node.select_nodes(m_Element.getName().c_str());

    if (nodes.size() < m_Min) {
      context.addError(fmt::format("too few '{}' elements, found {} instead of at least {}", m_Element.getName(),
                                   nodes.size(), m_Min));
    }
    if (nodes.size() > m_Max) {
      context.addError(fmt::format("too many '{}' elements, found {} instead of at most {}", m_Element.getName(),
                                   nodes.size(), m_Max));
    }

    for (const auto& child : nodes) {
      m_Element.validate(child.node(), context);
    }
  }

  inline void TAttributes::setRelaxed()
  {
    m_Relaxed = TAttributeMode::RELAXED;
  }

  inline void TAttributes::addAttribute(const TAttribute& attribute)
  {
    m_Attributes.emplace_back(attribute);
  }

  inline void TAttributes::validate(const pugi::xml_node& node, TValidationContext& context) const
  {
    if (m_Relaxed == TAttributeMode::STRICT) {
      for (const auto& attribute : node.attributes()) {
        if (!containsAttribute(attribute.name())) {
          context.addError(fmt::format("unknown attribute '{}'", attribute.name()));
        }
      }
    }

    for (const auto& attribute : m_Attributes) {
      attribute.validate(node, context);
    }
  }

  inline bool TAttributes::containsAttribute(const std::string& attribute) const
  {
    auto it = std::find_if(m_Attributes.begin(), m_Attributes.end(), [&attribute](const auto& att) {
      return att.getName() == attribute;
    });
    return it != m_Attributes.end();
  }


  inline void TText::validate(const pugi::xml_node& node, TValidationContext& context) const
  {
    // nothing to do here
    (void)node;
    (void)context;
  }


  inline void TComplexType::validate(const pugi::xml_node& node, TValidationContext& context) const
  {
    m_Attributes.validate(node, context);
    m_Sequence.validate(node, context);
    if (m_Text) {
      m_Text->validate(node, context);
    } else {
      if (node.first_child().name()[0] == '\0' && node.first_child().value()[0] != '\0') {
        context.addError("element has value but is not of mixed type");
      }
    }
  }


  inline void TInlineContentType::validate(const pugi::xml_node& node, TValidationContext& context) const
  {
    if (node.first_child().empty()) {
      context.addError(fmt::format("element '{}' does not have a value", node.name()));
    } else {
      m_Type.validate(node.first_child().value(), context);
    }
  }


  inline void TSimpleContentType::validate(const pugi::xml_node& node, TValidationContext& context) const
  {
    if (node.first_child().empty()) {
      context.addError(fmt::format("element '{}' does not have a value", node.name()));
    } else {
      m_Type.validate(node.first_child().value(), context);
    }
  }


  inline void TEnumeration::validate(const std::string& value, TValidationContext& context) const
  {
    auto it = std::find_if(m_EnumValues.begin(), m_EnumValues.end(), [&value](const auto& item) {
      return item == value;
    });
    if (it == m_EnumValues.end()) {
      context.addError(fmt::format("unknown enum value '{}' for type '{}'", value, m_Name));
    }
  }

  inline void TRestrictedType::validate(const std::string& value, TValidationContext& context) const
  {
    if (m_Enumeration) {
      m_Enumeration->validate(value, context);
    } else {
      m_BaseType.validate(value, context);
    }
  }

  inline void TElement::validate(const pugi::xml_node& node, TValidationContext& context) const
  {
    context.pushElement(getName());
    m_Type->validate(node, context);
    context.popElement();
  }

  [[nodiscard]] inline const TElement* TValidationContext::findElement(const std::string& name) const&
  {
    auto it = m_Elements.find(name);
    if (it == m_Elements.end()) {
      return nullptr;
    }
    return &it->second;
  }

  inline void TValidationContext::pushElement(std::string element)
  {
    m_ElementStack.emplace_back(std::move(element));
  }

  inline void TValidationContext::popElement()
  {
    m_ElementStack.pop_back();
  }

  inline std::string TValidationContext::getElementPath() const
  {
    std::stringstream stream;
    stream << "/";
    for (const auto& s : m_ElementStack) {
      stream << s << "/";
    }
    return stream.str();
  }

  inline void TValidationContext::addError(std::string error)
  {
    m_Errors.emplace_back(fmt::format("[{}] {}", getElementPath(), std::move(error)));
  }

  inline bool TValidationContext::hasErrors() const
  {
    return !m_Errors.empty();
  }

  inline void TValidationContext::swap(std::vector<std::string>& errors) noexcept
  {
    errors.swap(m_Errors);
  }

  inline bool TXSDSchemaValidator::validate(const pugi::xml_document& doc, std::vector<std::string>& errors) const
  {
    TValidationContext context{m_Elements};

    for (const auto& node : doc.children()) {
      const auto* element = context.findElement(node.name());
      if (element == nullptr) {
        context.addError(fmt::format("unknown element '{}'", node.name()));
        continue;
      }

      element->validate(node, context);
    }
    bool result = context.hasErrors();
    context.swap(errors);

    return !result;
  }

  inline void TXSDSchemaValidator::init()
  {
    if (auto root = m_Doc.select_node(fmt::format("/{}:schema", xsdSchemaNS).c_str()); !root) {
      throw TException{fmt::format("{}:schema node not found", xsdSchemaNS)};
    }

    initTypes();

    for (const auto& elements : m_Doc.select_nodes(fmt::format("/{0}:schema/{0}:simpleType", xsdSchemaNS).c_str())) {
      // ATTENTION: this is a strong simplification of simple types
      auto restriction = elements.node().select_nodes(fmt::format("{0}:restriction", xsdSchemaNS).c_str());
      if (!restriction.empty()) {
        std::optional<TEnumeration> enumeration;
        const auto& baseType = findType(restriction.first().node().attribute("base").as_string());
        auto children = restriction.first().node().children();

        if (!children.empty()) {
          std::vector<std::string> enumValues;
          for (const auto& enums : children) {
            enumValues.emplace_back(enums.attribute("value").as_string());
          }
          enumeration = TEnumeration{elements.node().attribute("name").as_string(), std::move(enumValues)};
        }

        auto type = std::make_unique<TRestrictedType>(elements.node().attribute("name").as_string(), baseType,
                                                      std::move(enumeration));
        // TODO (lcf): check for duplicates
        m_Types.try_emplace(type->getName(), std::move(type));
      } else {
        throw TException{"unsupported construct"};
      }
    }

    for (const auto& elements : m_Doc.select_nodes(fmt::format("/{0}:schema/{0}:element", xsdSchemaNS).c_str())) {
      // TODO (lcf): check for duplicates
      auto element = parseElement(elements.node());
      m_Elements.try_emplace(element.getName(), std::move(element));
    }
  }

  inline void TXSDSchemaValidator::initTypes()
  {
    auto type1 = std::make_unique<TPodType<TStringType>>(fmt::format("{}:string", xsdSchemaNS));
    m_Types.try_emplace(type1->getName(), std::move(type1));
    auto type2 = std::make_unique<TPodType<TIntegerType>>(fmt::format("{}:integer", xsdSchemaNS));
    m_Types.try_emplace(type2->getName(), std::move(type2));
    auto type3 = std::make_unique<TPodType<TDecimalType>>(fmt::format("{}:decimal", xsdSchemaNS));
    m_Types.try_emplace(type3->getName(), std::move(type3));
    auto type4 = std::make_unique<TPodType<TBooleanType>>(fmt::format("{}:boolean", xsdSchemaNS));
    m_Types.try_emplace(type4->getName(), std::move(type4));
    auto type5 = std::make_unique<TPodType<TNonNegativeIntegerType>>(fmt::format("{}:nonNegativeInteger", xsdSchemaNS));
    m_Types.try_emplace(type5->getName(), std::move(type5));
  }

  inline const TElement* TXSDSchemaValidator::findElement(const std::string& name) const&
  {
    auto it = m_Elements.find(name);
    return it == m_Elements.end() ? nullptr : &(it->second);
  }

  inline const TElement& TXSDSchemaValidator::findOrRegisterElement(const std::string& name)
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

  inline const TSimpleType& TXSDSchemaValidator::findType(const std::string& name) const&
  {
    auto it = m_Types.find(name);
    if (it == m_Types.end()) {
      throw TException{fmt::format("no type '{}' found", name)};
    }
    return *it->second;
  }

  inline TElement TXSDSchemaValidator::parseElement(const pugi::xml_node& node)
  {
    TSequence sequence;

    if (const auto element = node.select_node(fmt::format("{0}:complexType/{0}:simpleContent", xsdSchemaNS).c_str())) {
      if (const auto child = element.node().child(fmt::format("{0}:extension", xsdSchemaNS).c_str()); !child.empty()) {
        const auto* type = child.attribute("base").as_string();
        auto simpleContent = std::make_unique<TSimpleContentType>(node.name(), findType(type));
        return TElement{node.attribute("name").as_string(), std::move(simpleContent)};
      }
    }

    for (const auto& element :
         node.select_nodes(fmt::format("{0}:complexType/{0}:sequence/{0}:element", xsdSchemaNS).c_str())) {
      auto min = convertToUint64(element.node().attribute("minOccurs").as_string());
      auto maxString = std::string(element.node().attribute("maxOccurs").as_string());
      auto max = maxString == "unbounded" ? std::numeric_limits<uint64_t>::max() : convertToUint64(maxString);

      if (const auto refName = element.node().attribute("ref"); !refName.empty()) {
        const TElement& ref = findOrRegisterElement(refName.as_string());
        sequence.addElementRef(ref, min, max);
      } else if (const auto name = element.node().attribute("name"); !name.empty()) {
        const auto type = element.node().attribute("type");
        if (type.empty()) {
          throw TException{fmt::format("element '{}' has no type", name.as_string())};
        }
        sequence.addDirectElement(name.as_string(), findType(type.as_string()), min, max);
      }
    }

    TAttributes attributes;
    for (const auto& attribute : node.select_nodes(fmt::format("{0}:complexType/{0}:attribute", xsdSchemaNS).c_str())) {
      auto use = attribute.node().attribute("use");
      const auto* typeName = attribute.node().attribute("type").as_string();
      const auto& type = findType(typeName);

      attributes.addAttribute(TAttribute{attribute.node().attribute("name").as_string(), type,
                                         use ? std::string(use.as_string()) == "required" : false});
    }
    auto anyAttribute = node.select_node(fmt::format("{0}:complexType/{0}:anyAttribute", xsdSchemaNS).c_str());
    if (!anyAttribute.node().empty()) {
      attributes.setRelaxed();
    }

    std::optional<TText> text;

    if (node.child(fmt::format("{}:complexType", xsdSchemaNS).c_str()).attribute("mixed").as_bool()) {
      text = TText{};
    }

    auto complexType = std::make_unique<TComplexType>(node.attribute("name").as_string(), std::move(sequence),
                                                      std::move(attributes), std::move(text));
    return TElement{node.attribute("name").as_string(), std::move(complexType)};
  }


  inline pugi::xml_document TFileXsdSchemaLoader::load() const
  {
    pugi::xml_document doc;
    if (pugi::xml_parse_result parseResult = doc.load_file(m_XsdFile.string().c_str()); !parseResult) {
      throw TException{
        fmt::format("cannot parse xsd schema file '{}': {}", m_XsdFile.string(), parseResult.description())};
    }

    return doc;
  }


  inline pugi::xml_document TBufferXsdSchemaLoader::load() const
  {
    pugi::xml_document doc;
    if (pugi::xml_parse_result parseResult = doc.load_string(m_XsdSchema.c_str()); !parseResult) {
      throw TException{fmt::format("cannot parse xsd schema: {}", parseResult.description())};
    }

    return doc;
  }
}

#endif
