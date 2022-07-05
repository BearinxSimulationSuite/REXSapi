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

#ifndef REXSAPI_VALUE_DETAILS_HXX
#define REXSAPI_VALUE_DETAILS_HXX

#include <rexsapi/Base64.hxx>
#include <rexsapi/Types.hxx>

#include <variant>

namespace rexsapi
{
  template<class... Ts>
  struct overload : Ts... {
    using Ts::operator()...;
  };
  template<class... Ts>
  overload(Ts...) -> overload<Ts...>;

  namespace detail
  {
    template<typename T,
             typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value>::type* = nullptr>
    class CodedValueArray
    {
    public:
      static std::string encode(const std::vector<T>& array)
      {
        const auto* data = reinterpret_cast<const uint8_t*>(array.data());
        const auto len = array.size() * sizeof(T);
        return base64Encode(data, len);
      }

      static std::vector<T> decode(std::string_view value)
      {
        std::vector<T> array;
        const auto data = base64Decode(value);
        const auto count = data.size() / sizeof(T);
        array.reserve(count);
        const auto values = reinterpret_cast<const T*>(data.data());
        for (size_t n = 0; n < count; ++n) {
          array.emplace_back(values[n]);
        }

        return array;
      }
    };

    template<typename T,
             typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value>::type* = nullptr>
    class CodedValueMatrix
    {
    public:
      static std::string encode(const TMatrix<T>& matrix)
      {
        const auto count = matrix.m_Values.size() * matrix.m_Values.size() * sizeof(T);
        std::vector<T> array;
        array.reserve(count);
        for (const auto& row : matrix.m_Values) {
          for (const auto& column : row) {
            array.emplace_back(column);
          }
        }
        const auto* data = reinterpret_cast<const uint8_t*>(array.data());
        const auto len = array.size() * sizeof(T);
        return base64Encode(data, len);
      }

      static TMatrix<T> decode(std::string_view value)
      {
        TMatrix<T> matrix;
        const auto data = base64Decode(value);
        const auto count = data.size() / sizeof(T);
        const auto elementCount = static_cast<size_t>(::sqrt(count));
        const auto values = reinterpret_cast<const T*>(data.data());
        matrix.m_Values.reserve(elementCount);
        for (size_t row = 0; row < elementCount; ++row) {
          std::vector<T> col;
          col.reserve(elementCount);
          for (size_t column = 0; column < elementCount; ++column) {
            col.emplace_back(values[(row * elementCount) + column]);
          }
          matrix.m_Values.emplace_back(std::move(col));
        }

        return matrix;
      }
    };

    using Variant = std::variant<std::monostate, double, bool, int64_t, std::string, std::vector<double>,
                                 std::vector<Bool>, std::vector<int64_t>, std::vector<std::string>,
                                 std::vector<std::vector<int64_t>>, TMatrix<double>, TMatrix<std::string>>;

    template<typename T>
    inline const auto& value_getter(const Variant& value)
    {
      return std::get<T>(value);
    }

    template<>
    inline const auto& value_getter<Bool>(const Variant& value)
    {
      return std::get<bool>(value);
    }

    template<TValueType v>
    struct Enum2type {
      enum { value = static_cast<uint8_t>(v) };
    };

    template<typename T>
    struct TypeForValueType {
      using Type = T;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::FLOATING_POINT>> {
      using Type = double;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::BOOLEAN>> {
      using Type = bool;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::INTEGER>> {
      using Type = int64_t;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::ENUM>> {
      using Type = std::string;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::STRING>> {
      using Type = std::string;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::FILE_REFERENCE>> {
      using Type = std::string;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::BOOLEAN_ARRAY>> {
      using Type = std::vector<Bool>;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::FLOATING_POINT_ARRAY>> {
      using Type = std::vector<double>;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::INTEGER_ARRAY>> {
      using Type = std::vector<int64_t>;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::ENUM_ARRAY>> {
      using Type = std::vector<std::string>;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::STRING_ARRAY>> {
      using Type = std::vector<std::string>;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::REFERENCE_COMPONENT>> {
      using Type = int64_t;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::FLOATING_POINT_MATRIX>> {
      using Type = TMatrix<double>;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::STRING_MATRIX>> {
      using Type = TMatrix<std::string>;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::ARRAY_OF_INTEGER_ARRAYS>> {
      using Type = std::vector<std::vector<int64_t>>;
    };
  }

  using TFloatType = detail::TypeForValueType<detail::Enum2type<TValueType::FLOATING_POINT>>::Type;
  using TBoolType = detail::TypeForValueType<detail::Enum2type<TValueType::BOOLEAN>>::Type;
  using TIntType = detail::TypeForValueType<detail::Enum2type<TValueType::INTEGER>>::Type;
  using TEnumType = detail::TypeForValueType<detail::Enum2type<TValueType::ENUM>>::Type;
  using TStringType = detail::TypeForValueType<detail::Enum2type<TValueType::STRING>>::Type;
  using TFileReferenceType = detail::TypeForValueType<detail::Enum2type<TValueType::FILE_REFERENCE>>::Type;
  using TBoolArrayType = detail::TypeForValueType<detail::Enum2type<TValueType::BOOLEAN_ARRAY>>::Type;
  using TFloatArrayType = detail::TypeForValueType<detail::Enum2type<TValueType::FLOATING_POINT_ARRAY>>::Type;
  using TIntArrayType = detail::TypeForValueType<detail::Enum2type<TValueType::INTEGER_ARRAY>>::Type;
  using TEnumArrayType = detail::TypeForValueType<detail::Enum2type<TValueType::ENUM_ARRAY>>::Type;
  using TStringArrayType = detail::TypeForValueType<detail::Enum2type<TValueType::STRING_ARRAY>>::Type;
  using TReferenceComponentType = detail::TypeForValueType<detail::Enum2type<TValueType::REFERENCE_COMPONENT>>::Type;
  using TFloatMatrixType = detail::TypeForValueType<detail::Enum2type<TValueType::FLOATING_POINT_MATRIX>>::Type;
  using TStringMatrixType = detail::TypeForValueType<detail::Enum2type<TValueType::STRING_MATRIX>>::Type;
  using TArrayOfIntArraysType = detail::TypeForValueType<detail::Enum2type<TValueType::ARRAY_OF_INTEGER_ARRAYS>>::Type;

  using FloatTag = detail::Enum2type<TValueType::FLOATING_POINT>;
  using IntTag = detail::Enum2type<TValueType::INTEGER>;
  using BoolTag = detail::Enum2type<TValueType::BOOLEAN>;
  using EnumTag = detail::Enum2type<TValueType::ENUM>;
  using StringTag = detail::Enum2type<TValueType::STRING>;
  using FileReferenceTag = detail::Enum2type<TValueType::FILE_REFERENCE>;
  using FloatArrayTag = detail::Enum2type<TValueType::FLOATING_POINT_ARRAY>;
  using BoolArrayTag = detail::Enum2type<TValueType::BOOLEAN_ARRAY>;
  using IntArrayTag = detail::Enum2type<TValueType::INTEGER_ARRAY>;
  using EnumArrayTag = detail::Enum2type<TValueType::ENUM_ARRAY>;
  using StringArrayTag = detail::Enum2type<TValueType::STRING_ARRAY>;
  using ReferenceComponentTag = detail::Enum2type<TValueType::REFERENCE_COMPONENT>;
  using FloatMatrixTag = detail::Enum2type<TValueType::FLOATING_POINT_MATRIX>;
  using StringMatrixTag = detail::Enum2type<TValueType::STRING_MATRIX>;
  using ArrayOfIntArraysTag = detail::Enum2type<TValueType::ARRAY_OF_INTEGER_ARRAYS>;
}

#endif
