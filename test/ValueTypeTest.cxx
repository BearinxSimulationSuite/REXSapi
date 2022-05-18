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

#include <rexsapi/Types.hxx>
#include <rexsapi/Value.hxx>

#include <doctest.h>
#include <iostream>
#include <type_traits>

namespace rexsapi
{
  template<class T>
  using FloatTag = typename std::is_same<T, detail::Enum2type<TValueType::FLOATING_POINT>>;

  template<class T>
  using IntTag = typename std::is_same<T, detail::Enum2type<TValueType::INTEGER>>;

  template<class T>
  using BoolTag = typename std::is_same<T, detail::Enum2type<TValueType::BOOLEAN>>;

  template<typename T>
  auto dispatcher(const TValue& value)
  {
    if constexpr (FloatTag<T>::value)
      return value.getValue<TFloatType>();
    if constexpr (IntTag<T>::value)
      return value.getValue<TIntType>();
    if constexpr (BoolTag<T>::value)
      return value.getValue<TBoolType>();
  }

  using DispatcherFuncs = std::tuple<std::function<void(const TFloatType&)>, std::function<void(const TIntType&)>,
                                     std::function<void(const bool&)>>;

  template<typename T>
  T dispatch(TValueType type, const TValue& value, DispatcherFuncs funcs)
  {
    switch (type) {
      case TValueType::FLOATING_POINT:
        return std::get<0>(funcs)(value.getValue<TFloatType>());
      case TValueType::INTEGER:
        return std::get<1>(funcs)(value.getValue<TIntType>());
      case TValueType::BOOLEAN:
        return std::get<2>(funcs)(value.getValue<TBoolType>().m_Value);
      default:
        break;
    }
  }
}


TEST_CASE("Value type test")
{
  SUBCASE("Type from string")
  {
    CHECK(rexsapi::typeFromString("floating_point") == rexsapi::TValueType::FLOATING_POINT);
    CHECK(rexsapi::typeFromString("boolean") == rexsapi::TValueType::BOOLEAN);
    CHECK(rexsapi::typeFromString("integer") == rexsapi::TValueType::INTEGER);
    CHECK(rexsapi::typeFromString("enum") == rexsapi::TValueType::ENUM);
    CHECK(rexsapi::typeFromString("string") == rexsapi::TValueType::STRING);
    CHECK(rexsapi::typeFromString("file_reference") == rexsapi::TValueType::FILE_REFERENCE);
    CHECK(rexsapi::typeFromString("boolean_array") == rexsapi::TValueType::BOOLEAN_ARRAY);
    CHECK(rexsapi::typeFromString("floating_point_array") == rexsapi::TValueType::FLOATING_POINT_ARRAY);
    CHECK(rexsapi::typeFromString("reference_component") == rexsapi::TValueType::REFERENCE_COMPONENT);
    CHECK(rexsapi::typeFromString("floating_point_matrix") == rexsapi::TValueType::FLOATING_POINT_MATRIX);
    CHECK(rexsapi::typeFromString("integer_array") == rexsapi::TValueType::INTEGER_ARRAY);
    CHECK(rexsapi::typeFromString("enum_array") == rexsapi::TValueType::ENUM_ARRAY);
    CHECK(rexsapi::typeFromString("array_of_integer_arrays") == rexsapi::TValueType::ARRAY_OF_INTEGER_ARRAYS);
  }

  SUBCASE("Type from string")
  {
    CHECK_THROWS_WITH(rexsapi::typeFromString("not existing type"), "unknown value type 'not existing type'");
  }

  SUBCASE("Type mapping")
  {
    rexsapi::TValue v{4711};
    auto val = rexsapi::dispatcher<rexsapi::detail::Enum2type<rexsapi::TValueType::INTEGER>>(v);
    static_assert(std::is_same<decltype(val), int64_t>::value);

    rexsapi::dispatch<void>(rexsapi::TValueType::INTEGER, v,
                            {[](const auto& d) {
                               std::cout << "float " << d << std::endl;
                             },
                             [](const auto& i) {
                               std::cout << "int " << i << std::endl;
                             },
                             [](const auto& b) {
                               std::cout << "bool " << b << std::endl;
                             }});
  }
}
