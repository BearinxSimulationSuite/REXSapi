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

#ifndef REXSAPI_UNIT_HXX
#define REXSAPI_UNIT_HXX

#include <rexsapi/database/Unit.hxx>

#include <optional>

namespace rexsapi
{
  class TUnit
  {
  public:
    explicit TUnit(const database::TUnit& unit)
    : m_Unit{unit}
    {
    }

    explicit TUnit(std::string unit)
    : m_CustomUnit{std::move(unit)}
    {
    }

    [[nodiscard]] inline bool isCustomUnit() const
    {
      return !m_Unit.has_value();
    }

    [[nodiscard]] const std::string& getName() const
    {
      return isCustomUnit() ? m_CustomUnit : m_Unit->getName();
    }

    friend bool operator==(const TUnit& lhs, const database::TUnit& rhs)
    {
      if (lhs.isCustomUnit()) {
        return rhs.compare(lhs.m_CustomUnit);
      }

      return *lhs.m_Unit == rhs;
    }

  private:
    std::optional<database::TUnit> m_Unit{};
    std::string m_CustomUnit{};
  };
}

#endif
