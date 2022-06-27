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

#ifndef REXSAPI_JSON_VALUE_DECODER_HXX
#define REXSAPI_JSON_VALUE_DECODER_HXX

#include <rexsapi/ConversionHelper.hxx>
#include <rexsapi/Json.hxx>
#include <rexsapi/Types.hxx>
#include <rexsapi/Value.hxx>
#include <rexsapi/database/EnumValues.hxx>

#include <memory>
#include <optional>
#include <unordered_map>

namespace rexsapi
{
  namespace detail::json
  {
    class TJsonDecoder;
  }

  class TJsonValueDecoder
  {
  public:
    TJsonValueDecoder();

    [[nodiscard]] std::pair<TValue, bool>
    decode(TValueType type, const std::optional<const database::TEnumValues>& enumValue, const json& node) const;

  private:
    std::unordered_map<TValueType, std::unique_ptr<detail::json::TJsonDecoder>> m_Decoder;
  };

  namespace detail::json
  {
    class TJsonDecoder
    {
    public:
      virtual ~TJsonDecoder() = default;

      TJsonDecoder(const TJsonDecoder&) = default;
      TJsonDecoder& operator=(const TJsonDecoder&) = default;
      TJsonDecoder(TJsonDecoder&&) = default;
      TJsonDecoder& operator=(TJsonDecoder&&) = default;

      [[nodiscard]] std::pair<TValue, bool> decode(const std::optional<const database::TEnumValues>& enumValue,
                                                   const rexsapi::json& node) const
      {
        return onDecode(enumValue, node);
      }

    protected:
      TJsonDecoder() = default;

    private:
      virtual std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>& enumValue,
                                               const rexsapi::json& node) const = 0;
    };

    class TStringDecoder : public TJsonDecoder
    {
    public:
      using Type = std::string;

    private:
      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>&,
                                       const rexsapi::json& node) const override
      {
        auto value = node.at("string").get<std::string>();
        return std::make_pair(TValue{value}, !value.empty());
      }
    };

    class TFileReferenceDecoder : public TJsonDecoder
    {
    public:
      using Type = std::string;

    private:
      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>&,
                                       const rexsapi::json& node) const override
      {
        auto value = node.at("file_reference").get<std::string>();
        return std::make_pair(TValue{value}, !value.empty());
      }
    };

    class TBooleanDecoder : public TJsonDecoder
    {
    public:
      using Type = bool;

    private:
      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>&,
                                       const rexsapi::json& node) const override
      {
        return std::make_pair(TValue{node.at("boolean").get<bool>()}, true);
      }
    };

    class TIntegerDecoder : public TJsonDecoder
    {
    public:
      using Type = int64_t;

    private:
      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>&,
                                       const rexsapi::json& node) const override
      {
        return std::make_pair(TValue{node.at("integer").get<int64_t>()}, true);
      }
    };

    class TReferenceDecoder : public TJsonDecoder
    {
    public:
      using Type = int64_t;

    private:
      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>&,
                                       const rexsapi::json& node) const override
      {
        return std::make_pair(TValue{node.at("reference_component").get<int64_t>()}, true);
      }
    };

    class TFloatDecoder : public TJsonDecoder
    {
    public:
      using Type = double;

    private:
      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>&,
                                       const rexsapi::json& node) const override
      {
        return std::make_pair(TValue{node.at("floating_point").get<double>()}, true);
      }
    };

    class TEnumDecoder : public TJsonDecoder
    {
    public:
      using Type = std::string;

    private:
      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>& enumValue,
                                       const rexsapi::json& node) const override
      {
        auto value = node.at("enum").get<std::string>();

        if (enumValue) {
          return std::make_pair(TValue{value}, enumValue->check(value));
        }
        return std::make_pair(TValue{value}, true);
      }
    };

    template<typename Type>
    class TArrayDecoder : public TJsonDecoder
    {
    public:
      using type = Type;

      explicit TArrayDecoder(std::string name)
      : m_Name{std::move(name)}
      {
      }

    private:
      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>&,
                                       const rexsapi::json& node) const override
      {
        std::vector<type> array;
        const auto arr = node.at(m_Name);
        for (const auto& element : arr) {
          array.emplace_back(element.template get<type>());
        }
        return std::make_pair(TValue{array}, true);
      }

      std::string m_Name;
    };

    class TBoolArrayDecoder : public TJsonDecoder
    {
    private:
      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>&,
                                       const rexsapi::json& node) const override
      {
        std::vector<Bool> array;
        const auto arr = node.at("boolean_array");
        for (const auto& element : arr) {
          array.emplace_back(element.template get<bool>());
        }
        return std::make_pair(TValue{std::move(array)}, true);
      }
    };

    class TEnumArrayDecoder : public TJsonDecoder
    {
    private:
      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>& enumValue,
                                       const rexsapi::json& node) const override
      {
        if (enumValue.has_value()) {
          std::vector<std::string> array;
          bool result{true};
          const auto arr = node.at("enum_array");
          for (const auto& element : arr) {
            auto value = element.get<std::string>();
            if (enumValue->check(value)) {
              array.emplace_back(value);
            } else {
              result = false;
            }
          }
          return std::make_pair(TValue{std::move(array)}, result);
        }
        return std::make_pair(TValue{}, false);
      }
    };

    template<typename Type>
    class TMatrixDecoder : public TJsonDecoder
    {
    public:
      using type = Type;

      explicit TMatrixDecoder(std::string name)
      : m_Name{std::move(name)}
      {
      }

    private:
      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>&,
                                       const rexsapi::json& node) const override
      {
        TMatrix<type> matrix;
        for (const auto& row : node.at(m_Name)) {
          std::vector<type> r;
          for (const auto& column : row) {
            r.emplace_back(column.template get<type>());
          }
          matrix.m_Values.emplace_back(std::move(r));
        }
        bool result = matrix.validate();
        return std::make_pair(TValue{std::move(matrix)}, result);
      }
      std::string m_Name;
    };

    template<typename Type>
    class TArrayOfArraysDecoder : public TJsonDecoder
    {
    public:
      using type = Type;

      explicit TArrayOfArraysDecoder(std::string name)
      : m_Name{std::move(name)}
      {
      }

    private:
      std::pair<TValue, bool> onDecode(const std::optional<const database::TEnumValues>&,
                                       const rexsapi::json& node) const override
      {
        std::vector<std::vector<type>> arrays;
        for (const auto& row : node.at(m_Name)) {
          std::vector<type> r;
          for (const auto& column : row) {
            r.emplace_back(column.template get<type>());
          }
          arrays.emplace_back(std::move(r));
        }

        return std::make_pair(TValue{std::move(arrays)}, true);
      }
      std::string m_Name;
    };
  }


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline TJsonValueDecoder::TJsonValueDecoder()
  {
    m_Decoder[TValueType::BOOLEAN] = std::make_unique<detail::json::TBooleanDecoder>();
    m_Decoder[TValueType::INTEGER] = std::make_unique<detail::json::TIntegerDecoder>();
    m_Decoder[TValueType::FLOATING_POINT] = std::make_unique<detail::json::TFloatDecoder>();
    m_Decoder[TValueType::STRING] = std::make_unique<detail::json::TStringDecoder>();
    m_Decoder[TValueType::ENUM] = std::make_unique<detail::json::TEnumDecoder>();
    m_Decoder[TValueType::INTEGER_ARRAY] = std::make_unique<detail::json::TArrayDecoder<int64_t>>("integer_array");
    m_Decoder[TValueType::FLOATING_POINT_ARRAY] =
      std::make_unique<detail::json::TArrayDecoder<double>>("floating_point_array");
    m_Decoder[TValueType::BOOLEAN_ARRAY] = std::make_unique<detail::json::TBoolArrayDecoder>();
    m_Decoder[TValueType::STRING_ARRAY] = std::make_unique<detail::json::TArrayDecoder<std::string>>("string_array");
    m_Decoder[TValueType::ENUM_ARRAY] = std::make_unique<detail::json::TEnumArrayDecoder>();
    m_Decoder[TValueType::FLOATING_POINT_MATRIX] =
      std::make_unique<detail::json::TMatrixDecoder<double>>("floating_point_matrix");
    m_Decoder[TValueType::STRING_MATRIX] = std::make_unique<detail::json::TMatrixDecoder<std::string>>("string_matrix");
    m_Decoder[TValueType::REFERENCE_COMPONENT] = std::make_unique<detail::json::TReferenceDecoder>();
    m_Decoder[TValueType::FILE_REFERENCE] = std::make_unique<detail::json::TFileReferenceDecoder>();
    m_Decoder[TValueType::ARRAY_OF_INTEGER_ARRAYS] =
      std::make_unique<detail::json::TArrayOfArraysDecoder<int64_t>>("array_of_integer_arrays");
  }

  inline std::pair<TValue, bool> TJsonValueDecoder::decode(TValueType type,
                                                           const std::optional<const database::TEnumValues>& enumValue,
                                                           const json& node) const
  {
    if (node.empty()) {
      return std::make_pair(TValue{}, false);
    }

    try {
      return m_Decoder.at(type)->decode(enumValue, node);
    } catch (const json::exception&) {
      return std::make_pair(TValue{}, false);
    }
  }
}

#endif
