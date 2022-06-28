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
    : m_AttributeWrapper{AttributeWrapper{attribute}}
    , m_Unit{std::move(unit)}
    , m_Value{std::move(value)}
    {
      if (!(m_Unit == attribute.getUnit())) {
        throw TException{
          fmt::format("specified incorrect unit ({}) for attribute id={}", m_Unit.getName(), getAttributeId())};
      }
    }

    TAttribute(std::string attributeId, TUnit unit, TValueType type, TValue value)
    : m_CustomAttributeId{std::move(attributeId)}
    , m_CustomValueType{type}
    , m_Unit{std::move(unit)}
    , m_Value{std::move(value)}
    {
      if (m_CustomAttributeId.empty()) {
        throw TException{"a custom value is not allowed to have an empty id"};
      }
    }

    [[nodiscard]] bool isCustomAttribute() const
    {
      return !m_AttributeWrapper.has_value();
    }

    [[nodiscard]] const std::string& getAttributeId() const&
    {
      if (m_AttributeWrapper) {
        return m_AttributeWrapper->m_Attribute.getAttributeId();
      }
      return m_CustomAttributeId;
    }

    [[nodiscard]] const std::string& getName() const&
    {
      if (m_AttributeWrapper) {
        return m_AttributeWrapper->m_Attribute.getName();
      }
      return m_CustomAttributeId;
    }

    [[nodiscard]] const TUnit& getUnit() const&
    {
      return m_Unit;
    }

    [[nodiscard]] TValueType getValueType() const
    {
      if (m_AttributeWrapper) {
        return m_AttributeWrapper->m_Attribute.getValueType();
      }
      return m_CustomValueType;
    }

    [[nodiscard]] bool hasValue() const
    {
      return !m_Value.isEmpty();
    }

    [[nodiscard]] const TValue& getValue() const&
    {
      return m_Value;
    }

    template<typename T>
    [[nodiscard]] const auto& getValue() const&
    {
      return m_Value.getValue<T>();
    }

    [[nodiscard]] std::string getValueAsString() const
    {
      return m_Value.asString();
    }

  private:
    struct AttributeWrapper {
      const database::TAttribute& m_Attribute;
    };
    std::optional<AttributeWrapper> m_AttributeWrapper;

    std::string m_CustomAttributeId{};
    TValueType m_CustomValueType{TValueType::STRING};

    TUnit m_Unit;
    TValue m_Value;
  };

  using TAttributes = std::vector<TAttribute>;
}

#endif
