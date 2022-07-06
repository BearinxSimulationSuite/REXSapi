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

#ifndef REXSAPI_XML_VALUE_DECODER_HXX
#define REXSAPI_XML_VALUE_DECODER_HXX

#include <rexsapi/ConversionHelper.hxx>
#include <rexsapi/Types.hxx>
#include <rexsapi/Value.hxx>
#include <rexsapi/Xml.hxx>
#include <rexsapi/XmlUtils.hxx>
#include <rexsapi/database/EnumValues.hxx>

#include <memory>
#include <optional>
#include <unordered_map>

namespace rexsapi
{
  class TXMLDecoder
  {
  public:
    virtual ~TXMLDecoder() = default;

    TXMLDecoder(const TXMLDecoder&) = default;
    TXMLDecoder& operator=(const TXMLDecoder&) = default;
    TXMLDecoder(TXMLDecoder&&) = default;
    TXMLDecoder& operator=(TXMLDecoder&&) = default;

    [[nodiscard]] std::pair<TValue, bool> decode(const std::optional<const database::TEnumValues>& enumValue,
                                                 const pugi::xml_node& node) const
    {
      return onDecode(enumValue, node);
    }

  protected:
    TXMLDecoder() = default;

  private:
    virtual std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>& enumValue,
                                             const pugi::xml_node& node) const = 0;
  };

  class TXMLValueDecoder
  {
  public:
    TXMLValueDecoder();

    [[nodiscard]] std::pair<TValue, bool> decode(TValueType type,
                                                 const std::optional<const database::TEnumValues>& enumValue,
                                                 const pugi::xml_node& node) const;

    [[nodiscard]] std::pair<TValue, TValueType> decodeUnknown(const pugi::xml_node& node) const;

  private:
    std::unordered_map<TValueType, std::unique_ptr<TXMLDecoder>> m_Decoder;
  };

  namespace xml
  {
    class TStringDecoder : public TXMLDecoder
    {
    public:
      using Type = std::string;

    private:
      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>&,
                                       const pugi::xml_node& node) const override
      {
        std::string value = node.child_value();
        return std::make_pair(TValue{value}, !value.empty());
      }
    };

    class TBooleanDecoder : public TXMLDecoder
    {
    public:
      using Type = bool;

    private:
      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>&,
                                       const pugi::xml_node& node) const override
      {
        const auto* value = node.child_value();
        if (std::strcmp("true", value) == 0) {
          return std::make_pair(TValue{true}, true);
        }
        if (std::strcmp("false", value) == 0) {
          return std::make_pair(TValue{false}, true);
        }

        return std::make_pair(TValue{}, false);
      }
    };

    class TIntegerDecoder : public TXMLDecoder
    {
    public:
      using Type = int64_t;

    private:
      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>&,
                                       const pugi::xml_node& node) const override
      {
        try {
          return std::make_pair(TValue{convertToInt64(node.child_value())}, true);
        } catch (const std::exception&) {
          return std::make_pair(TValue{}, false);
        }
      }
    };

    class TFloatDecoder : public TXMLDecoder
    {
    public:
      using Type = double;

    private:
      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>&,
                                       const pugi::xml_node& node) const override
      {
        try {
          return std::make_pair(TValue{convertToDouble(node.child_value())}, true);
        } catch (const std::exception&) {
          return std::make_pair(TValue{}, false);
        }
      }
    };

    class TEnumDecoder : public TXMLDecoder
    {
    public:
      using Type = std::string;

    private:
      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>& enumValue,
                                       const pugi::xml_node& node) const override
      {
        if (enumValue) {
          return std::make_pair(TValue{node.child_value()}, enumValue->check(node.child_value()));
        }
        return std::make_pair(TValue{}, false);
      }
    };

    template<typename ElementDecoder>
    class TArrayDecoder : public TXMLDecoder
    {
    protected:
      using type = typename ElementDecoder::Type;

      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>& enumValue,
                                       const pugi::xml_node& node) const override
      {
        std::vector<type> array;
        ElementDecoder decoder;
        bool result{true};
        for (const auto& arrayNode : node.select_nodes("array/c")) {
          const auto res = decoder.decode(enumValue, arrayNode.node());
          if (res.second) {
            const TValue& val = res.first;
            array.emplace_back(std::move(val.getValue<type>()));
          }
          result &= res.second;
        }
        return std::make_pair(TValue{std::move(array)}, result);
      }
    };

    template<TCodedValueType v>
    struct Enum2type {
      enum { value = static_cast<uint8_t>(v) };
    };

    template<typename T>
    struct TypeForCodedValueType {
      using Type = T;
    };

    template<>
    struct TypeForCodedValueType<Enum2type<TCodedValueType::Int32>> {
      using Type = int32_t;
    };

    template<>
    struct TypeForCodedValueType<Enum2type<TCodedValueType::Float32>> {
      using Type = float;
    };

    template<>
    struct TypeForCodedValueType<Enum2type<TCodedValueType::Float64>> {
      using Type = double;
    };

    template<typename T>
    struct ValueTypeForCodedValueType {
      using Type = T;
    };

    template<>
    struct ValueTypeForCodedValueType<Enum2type<TCodedValueType::Int32>> {
      using Type = int64_t;
    };

    template<>
    struct ValueTypeForCodedValueType<Enum2type<TCodedValueType::Float32>> {
      using Type = double;
    };

    template<>
    struct ValueTypeForCodedValueType<Enum2type<TCodedValueType::Float64>> {
      using Type = double;
    };

    template<typename T1, typename T2>
    struct TCodedValueDecoder {
      static TValue decode(std::string_view value)
      {
        if (!std::is_same_v<T1, typename ValueTypeForCodedValueType<T2>::Type>) {
          throw TException{"coded value type does not correspond to attribute value type"};
        }
        auto result = detail::CodedValueArray<typename TypeForCodedValueType<T2>::Type>::decode(value);
        return TValue{std::vector<T1>{result.begin(), result.end()}};
      }
    };

    template<typename ElementDecoder>
    class TCodedArrayDecoder : public TArrayDecoder<ElementDecoder>
    {
    private:
      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>& enumValue,
                                       const pugi::xml_node& node) const override
      {
        TValue value;
        auto codedType = rexsapi::codedValueFromString(xml::getStringAttribute(node.first_child(), "code"));
        switch (codedType) {
          case TCodedValueType::None:
            return TArrayDecoder<ElementDecoder>::onDecode(enumValue, node);
          case TCodedValueType::Int32: {
            value = TCodedValueDecoder<typename TArrayDecoder<ElementDecoder>::type,Enum2type<TCodedValueType::Int32>>::decode(node.first_child().child_value());
            break;
          }
          case TCodedValueType::Float32: {
            value = TCodedValueDecoder<typename TArrayDecoder<ElementDecoder>::type,Enum2type<TCodedValueType::Float32>>::decode(node.first_child().child_value());
            break;
          }
          case TCodedValueType::Float64: {
            value = TCodedValueDecoder<typename TArrayDecoder<ElementDecoder>::type,Enum2type<TCodedValueType::Float64>>::decode(node.first_child().child_value());
            break;
          }
        }
        return std::make_pair(std::move(value), true);
      }
    };

    class TBoolArrayDecoder : public TXMLDecoder
    {
    private:
      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>& enumValue,
                                       const pugi::xml_node& node) const override
      {
        std::vector<Bool> array;
        TBooleanDecoder decoder;
        bool result{true};
        for (const auto& arrayNode : node.select_nodes("array/c")) {
          const auto res = decoder.decode(enumValue, arrayNode.node());
          if (res.second) {
            const TValue& val = res.first;
            array.emplace_back(val.getValue<bool>());
          }
          result &= res.second;
        }
        return std::make_pair(TValue{std::move(array)}, result);
      }
    };

    template<typename ElementDecoder>
    class TMatrixDecoder : public TXMLDecoder
    {
    private:
      using type = typename ElementDecoder::Type;

      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>& enumValue,
                                       const pugi::xml_node& node) const override
      {
        TMatrix<type> matrix;
        ElementDecoder decoder;
        bool result{true};

        for (const auto& row : node.select_nodes("matrix/r")) {
          std::vector<type> r;

          for (const auto& column : row.node().select_nodes("c")) {
            const auto res = decoder.decode(enumValue, column.node());
            if (res.second) {
              const TValue& val = res.first;
              r.emplace_back(std::move(val.getValue<type>()));
            }
            result &= res.second;
          }

          matrix.m_Values.emplace_back(std::move(r));
        }

        result &= matrix.validate();

        return std::make_pair(TValue{std::move(matrix)}, result);
      }
    };

    template<typename ElementDecoder>
    class TArrayOfArraysDecoder : public TXMLDecoder
    {
    private:
      using type = typename ElementDecoder::Type;

      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>& enumValue,
                                       const pugi::xml_node& node) const override
      {
        std::vector<std::vector<type>> arrays;
        ElementDecoder decoder;
        bool result{true};

        for (const auto& row : node.select_nodes("array_of_arrays/array")) {
          std::vector<type> r;

          for (const auto& column : row.node().select_nodes("c")) {
            const auto res = decoder.decode(enumValue, column.node());
            if (res.second) {
              const TValue& val = res.first;
              r.emplace_back(std::move(val.getValue<type>()));
            }
            result &= res.second;
          }

          arrays.emplace_back(std::move(r));
        }

        return std::make_pair(TValue{std::move(arrays)}, result);
      }
    };
  }


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline TXMLValueDecoder::TXMLValueDecoder()
  {
    m_Decoder[TValueType::BOOLEAN] = std::make_unique<xml::TBooleanDecoder>();
    m_Decoder[TValueType::INTEGER] = std::make_unique<xml::TIntegerDecoder>();
    m_Decoder[TValueType::FLOATING_POINT] = std::make_unique<xml::TFloatDecoder>();
    m_Decoder[TValueType::STRING] = std::make_unique<xml::TStringDecoder>();
    m_Decoder[TValueType::ENUM] = std::make_unique<xml::TEnumDecoder>();
    m_Decoder[TValueType::INTEGER_ARRAY] = std::make_unique<xml::TCodedArrayDecoder<xml::TIntegerDecoder>>();
    m_Decoder[TValueType::FLOATING_POINT_ARRAY] = std::make_unique<xml::TCodedArrayDecoder<xml::TFloatDecoder>>();
    m_Decoder[TValueType::BOOLEAN_ARRAY] = std::make_unique<xml::TBoolArrayDecoder>();
    m_Decoder[TValueType::ENUM_ARRAY] = std::make_unique<xml::TArrayDecoder<xml::TEnumDecoder>>();
    m_Decoder[TValueType::STRING_ARRAY] = std::make_unique<xml::TArrayDecoder<xml::TStringDecoder>>();
    m_Decoder[TValueType::FLOATING_POINT_MATRIX] = std::make_unique<xml::TMatrixDecoder<xml::TFloatDecoder>>();
    m_Decoder[TValueType::STRING_MATRIX] = std::make_unique<xml::TMatrixDecoder<xml::TStringDecoder>>();
    m_Decoder[TValueType::REFERENCE_COMPONENT] = std::make_unique<xml::TIntegerDecoder>();
    m_Decoder[TValueType::FILE_REFERENCE] = std::make_unique<xml::TStringDecoder>();
    m_Decoder[TValueType::ARRAY_OF_INTEGER_ARRAYS] =
      std::make_unique<xml::TArrayOfArraysDecoder<xml::TIntegerDecoder>>();
  }

  inline std::pair<TValue, bool> TXMLValueDecoder::decode(TValueType type,
                                                          const std::optional<const database::TEnumValues>& enumValue,
                                                          const pugi::xml_node& node) const
  {
    if (node.empty()) {
      return std::make_pair(TValue{}, false);
    }

    return m_Decoder.at(type)->decode(enumValue, node);
  }

  inline static bool isArray(const pugi::xml_node& node)
  {
    return !node.select_nodes("array/c").empty();
  }

  inline static bool isMatrix(const pugi::xml_node& node)
  {
    return !node.select_nodes("matrix/r").empty();
  }

  inline static bool isArrayOfArrays(const pugi::xml_node& node)
  {
    return !node.select_nodes("array_of_arrays/array").empty();
  }

  inline std::pair<TValue, TValueType> TXMLValueDecoder::decodeUnknown(const pugi::xml_node& node) const
  {
    if (isArray(node)) {
      auto [value, success] = m_Decoder.at(TValueType::STRING_ARRAY)->decode({}, node);
      return std::make_pair(std::move(value), TValueType::STRING_ARRAY);
    }
    if (isMatrix(node)) {
      auto [value, success] = m_Decoder.at(TValueType::STRING_MATRIX)->decode({}, node);
      return std::make_pair(std::move(value), TValueType::STRING_MATRIX);
    }
    if (isArrayOfArrays(node)) {
      auto [value, success] = m_Decoder.at(TValueType::ARRAY_OF_INTEGER_ARRAYS)->decode({}, node);
      return std::make_pair(std::move(value), TValueType::ARRAY_OF_INTEGER_ARRAYS);
    }

    return std::make_pair(TValue{node.child_value()}, TValueType::STRING);
  }
}

#endif
