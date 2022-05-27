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

#ifndef REXSAPI_DATABASE_ATTRIBUTE_HXX
#define REXSAPI_DATABASE_ATTRIBUTE_HXX

#include <rexsapi/Format.hxx>
#include <rexsapi/Types.hxx>
#include <rexsapi/database/EnumValues.hxx>
#include <rexsapi/database/Interval.hxx>
#include <rexsapi/database/Unit.hxx>

#include <optional>

namespace rexsapi::database
{
  class TAttribute
  {
  public:
    TAttribute(std::string attributeId, std::string name, TValueType type, TUnit unit, std::string symbol,
               std::optional<const TInterval> interval, std::optional<const TEnumValues> enumValues)
    : m_AttributeId{std::move(attributeId)}
    , m_Name{std::move(name)}
    , m_Type{type}
    , m_Unit{std::move(unit)}
    , m_Symbol{std::move(symbol)}
    , m_Interval{std::move(interval)}
    , m_EnumValues{std::move(enumValues)}
    {
      if (m_Type == TValueType::ENUM && !m_EnumValues) {
        throw TException{fmt::format("enum with id '{}' does not have any enum values", m_AttributeId)};
      }
    }

    ~TAttribute() = default;

    TAttribute(const TAttribute&) = delete;
    TAttribute& operator=(const TAttribute&) = delete;
    TAttribute(TAttribute&&) = default;
    TAttribute& operator=(TAttribute&&) = delete;

    [[nodiscard]] const std::string& getAttributeId() const
    {
      return m_AttributeId;
    }

    [[nodiscard]] const std::string& getName() const
    {
      return m_Name;
    }

    [[nodiscard]] TValueType getValueType() const
    {
      return m_Type;
    }

    [[nodiscard]] const TUnit& getUnit() const
    {
      return m_Unit;
    }

    [[nodiscard]] const std::string& getSymbol() const
    {
      return m_Symbol;
    }

    [[nodiscard]] const std::optional<const TInterval>& getInterval() const
    {
      return m_Interval;
    }

    [[nodiscard]] const std::optional<const TEnumValues>& getEnums() const
    {
      return m_EnumValues;
    }

  private:
    std::string m_AttributeId;
    std::string m_Name;
    TValueType m_Type;
    TUnit m_Unit;
    std::string m_Symbol;
    std::optional<const TInterval> m_Interval;
    std::optional<const TEnumValues> m_EnumValues;
  };
}

#endif
