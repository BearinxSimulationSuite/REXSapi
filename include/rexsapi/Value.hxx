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

#ifndef REXSAPI_VALUE_HXX
#define REXSAPI_VALUE_HXX

#include <rexsapi/ConversionHelper.hxx>
#include <rexsapi/Types.hxx>
#include <rexsapi/Value_Details.hxx>
#include <rexsapi/database/EnumValues.hxx>

#include <functional>

namespace rexsapi
{
  enum class TCodeType { None, Default, Optimized };


  class TValue
  {
  public:
    TValue() = default;

    template<typename T>
    explicit TValue(T&& val)
    : m_Value(std::forward<T>(val))
    {
    }

    explicit TValue(const char* val)
    : m_Value(std::string(val))
    {
    }

    explicit TValue(int val)
    : m_Value(static_cast<int64_t>(val))
    {
    }

    explicit TValue(Bool val)
    : m_Value(val.m_Value)
    {
    }

    template<typename T>
    TValue& operator=(T&& val)
    {
      m_Value = std::forward<T>(val);
      return *this;
    }

    TValue& operator=(int val)
    {
      m_Value = static_cast<int64_t>(val);
      return *this;
    }

    TValue& operator=(const char* val)
    {
      m_Value = std::string(val);
      return *this;
    }

    bool isEmpty() const
    {
      return m_Value.index() == 0;
    }

    template<typename T>
    const auto& getValue() const&
    {
      return detail::value_getter<T>(m_Value);
    }

    template<typename T>
    const auto& getValue(const T& def) const&
    {
      if (m_Value.index() == 0) {
        return def;
      }
      return detail::value_getter<T>(m_Value);
    }

    friend bool operator==(const TValue& lhs, const TValue& rhs)
    {
      // ATTENTION: will currently compare floating point values with ==
      return lhs.m_Value == rhs.m_Value;
    }

    std::string asString() const;

    void coded(TCodeType type)
    {
      m_CodeType = type;
    }

    TCodeType coded() const
    {
      return m_CodeType;
    }

    bool matchesValueType(TValueType type) const
    {
      switch (type) {
        case TValueType::FLOATING_POINT:
          return std::holds_alternative<TFloatType>(m_Value);
        case TValueType::INTEGER:
          return std::holds_alternative<TIntType>(m_Value);
        case TValueType::BOOLEAN:
          return std::holds_alternative<TBoolType>(m_Value);
        case TValueType::ENUM:
          return std::holds_alternative<TEnumType>(m_Value);
        case TValueType::STRING:
          return std::holds_alternative<TStringType>(m_Value);
        case TValueType::FILE_REFERENCE:
          return std::holds_alternative<TFileReferenceType>(m_Value);
        case TValueType::FLOATING_POINT_ARRAY:
          return std::holds_alternative<TFloatArrayType>(m_Value);
        case TValueType::BOOLEAN_ARRAY:
          return std::holds_alternative<TBoolArrayType>(m_Value);
        case TValueType::INTEGER_ARRAY:
          return std::holds_alternative<TIntArrayType>(m_Value);
        case TValueType::ENUM_ARRAY:
          return std::holds_alternative<TEnumArrayType>(m_Value);
        case TValueType::STRING_ARRAY:
          return std::holds_alternative<TStringArrayType>(m_Value);
        case TValueType::REFERENCE_COMPONENT:
          return std::holds_alternative<TReferenceComponentType>(m_Value);
        case TValueType::FLOATING_POINT_MATRIX:
          return std::holds_alternative<TFloatMatrixType>(m_Value);
        case TValueType::STRING_MATRIX:
          return std::holds_alternative<TStringMatrixType>(m_Value);
        case TValueType::ARRAY_OF_INTEGER_ARRAYS:
          return std::holds_alternative<TArrayOfIntArraysType>(m_Value);
      }
      return false;
    }

  private:
    detail::Variant m_Value;
    TCodeType m_CodeType{TCodeType::None};
  };


  template<typename R>
  using DispatcherFuncs = std::tuple<
    std::function<R(FloatTag, const TFloatType&)>, std::function<R(BoolTag, const bool&)>,
    std::function<R(IntTag, const TIntType&)>, std::function<R(EnumTag, const std::string&)>,
    std::function<R(StringTag, const TStringType&)>, std::function<R(FileReferenceTag, const TFileReferenceType&)>,
    std::function<R(FloatArrayTag, const TFloatArrayType&)>, std::function<R(BoolArrayTag, const TBoolArrayType&)>,
    std::function<R(IntArrayTag, const TIntArrayType&)>, std::function<R(EnumArrayTag, const TEnumArrayType&)>,
    std::function<R(StringArrayTag, const TStringArrayType&)>,
    std::function<R(ReferenceComponentTag, const TReferenceComponentType&)>,
    std::function<R(FloatMatrixTag, const TFloatMatrixType&)>,
    std::function<R(StringMatrixTag, const TStringMatrixType&)>,
    std::function<R(ArrayOfIntArraysTag, const TArrayOfIntArraysType&)>>;

  template<typename R>
  auto dispatch(TValueType type, const TValue& value, DispatcherFuncs<R> funcs);


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline std::string TValue::asString() const

  {
    return std::visit(overload{[](const std::monostate&) -> std::string {
                                 return "";
                               },
                               [](const std::string& s) -> std::string {
                                 return s;
                               },
                               [](const bool& b) -> std::string {
                                 return fmt::format("{}", b);
                               },
                               [](const double& d) -> std::string {
                                 return format(d);
                               },
                               [](const int64_t& i) -> std::string {
                                 return fmt::format("{}", i);
                               },
                               [](const std::vector<double>&) -> std::string {
                                 throw TException{"cannot convert vector to string"};
                               },
                               [](const std::vector<Bool>&) -> std::string {
                                 throw TException{"cannot convert vector to string"};
                               },
                               [](const std::vector<int64_t>&) -> std::string {
                                 throw TException{"cannot convert vector to string"};
                               },
                               [](const std::vector<std::string>&) -> std::string {
                                 throw TException{"cannot convert vector to string"};
                               },
                               [](const std::vector<std::vector<int64_t>>&) -> std::string {
                                 throw TException{"cannot convert vector to string"};
                               },
                               [](const TMatrix<double>&) -> std::string {
                                 throw TException{"cannot convert matrix to string"};
                               },
                               [](const TMatrix<std::string>&) -> std::string {
                                 throw TException{"cannot convert matrix to string"};
                               }},
                      m_Value);
  }

  template<typename R>
  inline auto dispatch(TValueType type, const TValue& value, DispatcherFuncs<R> funcs)
  {
    try {
      switch (type) {
        case TValueType::FLOATING_POINT: {
          auto c = std::get<std::function<R(FloatTag, const TFloatType&)>>(funcs);
          if (c) {
            return c(FloatTag(), value.getValue<TFloatType>());
          }
          break;
        }
        case TValueType::BOOLEAN: {
          auto c = std::get<std::function<R(BoolTag, const bool&)>>(funcs);
          if (c) {
            return c(BoolTag(), value.getValue<TBoolType>());
          }
          break;
        }
        case TValueType::INTEGER: {
          auto c = std::get<std::function<R(IntTag, const TIntType&)>>(funcs);
          if (c) {
            return c(IntTag(), value.getValue<TIntType>());
          }
          break;
        }
        case TValueType::ENUM: {
          auto c = std::get<std::function<R(EnumTag, const TEnumType&)>>(funcs);
          if (c) {
            return c(EnumTag(), value.getValue<TEnumType>());
          }
          break;
        }
        case TValueType::STRING: {
          auto c = std::get<std::function<R(StringTag, const TStringType&)>>(funcs);
          if (c) {
            return c(StringTag(), value.getValue<TStringType>());
          }
          break;
        }
        case TValueType::FILE_REFERENCE: {
          auto c = std::get<std::function<R(FileReferenceTag, const TFileReferenceType&)>>(funcs);
          if (c) {
            return c(FileReferenceTag(), value.getValue<TFileReferenceType>());
          }
          break;
        }
        case TValueType::FLOATING_POINT_ARRAY: {
          auto c = std::get<std::function<R(FloatArrayTag, const TFloatArrayType&)>>(funcs);
          if (c) {
            return c(FloatArrayTag(), value.getValue<TFloatArrayType>());
          }
          break;
        }
        case TValueType::BOOLEAN_ARRAY: {
          auto c = std::get<std::function<R(BoolArrayTag, const TBoolArrayType&)>>(funcs);
          if (c) {
            return c(BoolArrayTag(), value.getValue<TBoolArrayType>());
          }
          break;
        }
        case TValueType::INTEGER_ARRAY: {
          auto c = std::get<std::function<R(IntArrayTag, const TIntArrayType&)>>(funcs);
          if (c) {
            return c(IntArrayTag(), value.getValue<TIntArrayType>());
          }
          break;
        }
        case TValueType::ENUM_ARRAY: {
          auto c = std::get<std::function<R(EnumArrayTag, const TEnumArrayType&)>>(funcs);
          if (c) {
            return c(EnumArrayTag(), value.getValue<TEnumArrayType>());
          }
          break;
        }
        case TValueType::STRING_ARRAY: {
          auto c = std::get<std::function<R(StringArrayTag, const TStringArrayType&)>>(funcs);
          if (c) {
            return c(StringArrayTag(), value.getValue<TStringArrayType>());
          }
          break;
        }
        case TValueType::REFERENCE_COMPONENT: {
          auto c = std::get<std::function<R(ReferenceComponentTag, const TReferenceComponentType&)>>(funcs);
          if (c) {
            return c(ReferenceComponentTag(), value.getValue<TReferenceComponentType>());
          }
          break;
        }
        case TValueType::FLOATING_POINT_MATRIX: {
          auto c = std::get<std::function<R(FloatMatrixTag, const TFloatMatrixType&)>>(funcs);
          if (c) {
            return c(FloatMatrixTag(), value.getValue<TFloatMatrixType>());
          }
          break;
        }
        case TValueType::STRING_MATRIX: {
          auto c = std::get<std::function<R(StringMatrixTag, const TStringMatrixType&)>>(funcs);
          if (c) {
            return c(StringMatrixTag(), value.getValue<TStringMatrixType>());
          }
          break;
        }
        case TValueType::ARRAY_OF_INTEGER_ARRAYS: {
          auto c = std::get<std::function<R(ArrayOfIntArraysTag, const TArrayOfIntArraysType&)>>(funcs);
          if (c) {
            return c(ArrayOfIntArraysTag(), value.getValue<TArrayOfIntArraysType>());
          }
          break;
        }
      }
    } catch (const std::bad_variant_access&) {
      throw TException{fmt::format("wrong value {} for type {}", value.asString(), toTypeString(type))};
    }
    throw TException{fmt::format("no function set for {}", toTypeString(type))};
  }
}

#endif
