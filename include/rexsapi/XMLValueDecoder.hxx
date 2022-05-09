
#ifndef REXSAPI_XML_VALUE_DECODER_HXX
#define REXSAPI_XML_VALUE_DECODER_HXX

#include <rexsapi/ConversionHelper.hxx>
#include <rexsapi/XMLParser.hxx>
#include <rexsapi/database/EnumValues.hxx>
#include <rexsapi/database/ValueType.hxx>

#include <memory>
#include <optional>
#include <unordered_map>

namespace rexsapi
{
  class TXMLDecoder
  {
  public:
    virtual ~TXMLDecoder() = default;

    [[nodiscard]] bool decode(const std::optional<const database::TEnumValues>& enumValue,
                              const pugi::xml_node& node) const
    {
      return onDecode(enumValue, node);
    }

  private:
    virtual bool onDecode(const std::optional<const database::TEnumValues>& enumValue,
                          const pugi::xml_node& node) const = 0;
  };

  class TXMLValueDecoder
  {
  public:
    TXMLValueDecoder();

    [[nodiscard]] bool decode(database::TValueType type, const std::optional<const database::TEnumValues>& enumValue,
                              const pugi::xml_node& node) const;

  private:
    std::unordered_map<database::TValueType, std::unique_ptr<TXMLDecoder>> m_Decoder;
  };

  namespace xml
  {
    class TStringDecoder : public TXMLDecoder
    {
    private:
      bool onDecode(const std::optional<const database::TEnumValues>&, const pugi::xml_node& node) const override
      {
        const std::string value = node.child_value();
        return !value.empty();
      }
    };

    class TBooleanDecoder : public TXMLDecoder
    {
    private:
      bool onDecode(const std::optional<const database::TEnumValues>&, const pugi::xml_node& node) const override
      {
        const auto* value = node.child_value();
        if (std::strcmp("true", value) == 0) {
          return true;
        }
        if (std::strcmp("false", value) == 0) {
          return true;
        }

        return false;
      }
    };

    class TIntegerDecoder : public TXMLDecoder
    {
    private:
      bool onDecode(const std::optional<const database::TEnumValues>&, const pugi::xml_node& node) const override
      {
        try {
          convertToInt64(node.child_value());
          return true;
        } catch (const std::exception&) {
          return false;
        }
      }
    };

    class TFloatDecoder : public TXMLDecoder
    {
    private:
      bool onDecode(const std::optional<const database::TEnumValues>&, const pugi::xml_node& node) const override
      {
        try {
          convertToDouble(node.child_value());
          return true;
        } catch (const std::exception&) {
          return false;
        }
      }
    };

    class TEnumDecoder : public TXMLDecoder
    {
    private:
      bool onDecode(const std::optional<const database::TEnumValues>& enumValue,
                    const pugi::xml_node& node) const override
      {
        if (enumValue) {
          return enumValue->check(node.child_value());
        }
        return false;
      }
    };

    template<typename ElementDecoder>
    class TArrayDecoder : public TXMLDecoder
    {
    private:
      bool onDecode(const std::optional<const database::TEnumValues>& enumValue,
                    const pugi::xml_node& node) const override
      {
        ElementDecoder decoder;
        bool result{true};
        for (const auto& arrayNode : node.select_nodes("array/c")) {
          result &= decoder.decode(enumValue, arrayNode.node());
        }
        return result;
      }
    };
  }


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline TXMLValueDecoder::TXMLValueDecoder()
  {
    m_Decoder[database::TValueType::BOOLEAN] = std::make_unique<xml::TBooleanDecoder>();
    m_Decoder[database::TValueType::INTEGER] = std::make_unique<xml::TIntegerDecoder>();
    m_Decoder[database::TValueType::FLOATING_POINT] = std::make_unique<xml::TFloatDecoder>();
    m_Decoder[database::TValueType::ENUM] = std::make_unique<xml::TEnumDecoder>();
    m_Decoder[database::TValueType::INTEGER_ARRAY] = std::make_unique<xml::TArrayDecoder<xml::TIntegerDecoder>>();
    m_Decoder[database::TValueType::FLOATING_POINT_ARRAY] = std::make_unique<xml::TArrayDecoder<xml::TFloatDecoder>>();
    m_Decoder[database::TValueType::BOOLEAN_ARRAY] = std::make_unique<xml::TArrayDecoder<xml::TBooleanDecoder>>();
    m_Decoder[database::TValueType::REFERENCE_COMPONENT] = std::make_unique<xml::TIntegerDecoder>();
    m_Decoder[database::TValueType::STRING] = std::make_unique<xml::TStringDecoder>();
  }

  inline bool TXMLValueDecoder::decode(database::TValueType type,
                                       const std::optional<const database::TEnumValues>& enumValue,
                                       const pugi::xml_node& node) const
  {
    if (node.empty()) {
      return false;
    }

    try {
      return m_Decoder.at(type)->decode(enumValue, node);
    } catch (const std::exception&) {
      return false;
    }
  }
}

#endif
