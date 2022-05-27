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

#ifndef REXSAPI_ATTRIBUTE_HXX
#define REXSAPI_ATTRIBUTE_HXX

#include <rexsapi/Unit.hxx>
#include <rexsapi/Value.hxx>
#include <rexsapi/database/Attribute.hxx>

namespace rexsapi
{
  class TAttribute
  {
  public:
    TAttribute(const database::TAttribute& attribute, TUnit unit, TValue value)
    : m_Attribute{attribute}
    , m_Unit{std::move(unit)}
    , m_Value{std::move(value)}
    {
      if (!(m_Unit == attribute.getUnit())) {
        throw TException{fmt::format("specified incorrect unit ({}) for attribute '{}'", m_Unit.getName(), getName())};
      }
    }

    [[nodiscard]] const std::string& getAttributeId() const
    {
      return m_Attribute.getAttributeId();
    }

    [[nodiscard]] const std::string& getName() const
    {
      return m_Attribute.getName();
    }

    [[nodiscard]] const TUnit& getUnit() const
    {
      return m_Unit;
    }

    [[nodiscard]] TValueType getValueType() const
    {
      return m_Attribute.getValueType();
    }

    [[nodiscard]] bool hasValue() const
    {
      return !m_Value.isEmpty();
    }

    [[nodiscard]] const TValue& getValue() const
    {
      return m_Value;
    }

    template<typename T>
    [[nodiscard]] const T& getValue() const
    {
      return m_Value.getValue<T>();
    }

    [[nodiscard]] std::string getValueAsString() const
    {
      return m_Value.asString();
    }

  private:
    const database::TAttribute& m_Attribute;
    TUnit m_Unit;
    TValue m_Value;
  };

  using TAttributes = std::vector<TAttribute>;
}

#endif
