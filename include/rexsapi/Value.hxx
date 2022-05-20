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
  class TValue
  {
  public:
    TValue() = default;
    ~TValue() = default;

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

    explicit TValue(bool val)
    : m_Value(Bool{val})
    {
    }

    TValue(const TValue&) = default;
    TValue(TValue&&) = default;
    TValue& operator=(const TValue&) = delete;
    TValue& operator=(TValue&&) = delete;

    bool isEmpty() const
    {
      return m_Value.index() == 0;
    }

    template<typename T>
    const T& getValue() const
    {
      return detail::value_getter<T>(m_Value);
    }

    template<typename T>
    const T& getValue(const T& def) const
    {
      if (m_Value.index() == 0) {
        return def;
      }
      return detail::value_getter<T>(m_Value);
    }

    std::string asString() const;

  private:
    detail::Variant m_Value;
  };


  template<typename R>
  using DispatcherFuncs = std::tuple<
    std::function<R(FloatTag, const TFloatType&)>, std::function<R(BoolTag, const bool&)>,
    std::function<R(IntTag, const TIntType&)>, std::function<R(EnumTag, const std::string&)>,
    std::function<R(StringTag, const TStringType&)>, std::function<R(FileReferenceTag, const TFileReferenceType&)>,
    std::function<R(FloatArrayTag, const TFloatArrayType&)>, std::function<R(BoolArrayTag, const TBoolArrayType&)>,
    std::function<R(IntArrayTag, const TIntArrayType&)>, std::function<R(EnumArrayTag, const TEnumArrayType&)>,
    std::function<R(ReferenceComponentTag, const TReferenceComponentType&)>,
    std::function<R(FloatMatrixTag, const TFloatMatrixType&)>,
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
                               [](const std::string& s) {
                                 return s;
                               },
                               [](const Bool& b) -> std::string {
                                 return fmt::format("{}", static_cast<bool>(b));
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
                               }},
                      m_Value);
  }

  template<typename R>
  inline auto dispatch(TValueType type, const TValue& value, DispatcherFuncs<R> funcs)
  {
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
          return c(BoolTag(), value.getValue<TBoolType>().m_Value);
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
      case TValueType::ARRAY_OF_INTEGER_ARRAYS: {
        auto c = std::get<std::function<R(ArrayOfIntArraysTag, const TArrayOfIntArraysType&)>>(funcs);
        if (c) {
          return c(ArrayOfIntArraysTag(), value.getValue<TArrayOfIntArraysType>());
        }
        break;
      }
    }
    throw TException{fmt::format("no function set for {}", rexsapi::toTypeString(type))};
  }
}

#endif
