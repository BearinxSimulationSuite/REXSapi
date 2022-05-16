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
    explicit TUnit(const database::TUnit& unit)
    : m_Unit{&unit}
    {
    }

    explicit TUnit(std::string unit)
    : m_CustomUnit{std::move(unit)}
    {
    }

    [[nodiscard]] bool isCustomUnit() const
    {
      return m_Unit == nullptr;
    }

    [[nodiscard]] const std::string& getName() const
    {
      return m_Unit != nullptr ? m_Unit->getName() : m_CustomUnit;
    }

  private:
    const database::TUnit* m_Unit{nullptr};
    std::string m_CustomUnit;
  };
}

#endif
