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


namespace rexsapi
{
  class TUnit
  {
  public:
    TUnit() = default;

    explicit TUnit(const database::TUnit& unit)
    : m_Unit{unit.getName()}
    , m_IsCustomUnit{false}
    {
    }

    explicit TUnit(std::string unit)
    : m_Unit{std::move(unit)}
    , m_IsCustomUnit{true}
    {
    }

    [[nodiscard]] inline bool isCustomUnit() const
    {
      return m_IsCustomUnit;
    }

    [[nodiscard]] const std::string& getName() const&
    {
      return m_Unit;
    }

    friend bool operator==(const TUnit& lhs, const database::TUnit& rhs)
    {
      return rhs.compare(lhs.m_Unit);
    }

    friend bool operator==(const TUnit& lhs, const TUnit& rhs)
    {
      return lhs.m_Unit == rhs.m_Unit;
    }

  private:
    std::string m_Unit{};
    bool m_IsCustomUnit{true};
  };
}

#endif
