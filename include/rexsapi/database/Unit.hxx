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

#ifndef REXSAPI_DATABASE_UNIT_HXX
#define REXSAPI_DATABASE_UNIT_HXX

#include <string>

namespace rexsapi::database
{
  class TUnit
  {
  public:
    TUnit(uint64_t id, std::string name)
    : m_Id{id}
    , m_Name{std::move(name)}
    {
    }

    [[nodiscard]] uint64_t getId() const
    {
      return m_Id;
    }

    [[nodiscard]] const std::string& getName() const
    {
      return m_Name;
    }

    [[nodiscard]] bool compare(std::string_view name) const
    {
      return m_Name == name;
    }

    friend bool operator==(const TUnit& lhs, const TUnit& rhs)
    {
      return lhs.compare(rhs.getName());
    }

    friend bool operator!=(const TUnit& lhs, const TUnit& rhs)
    {
      return !lhs.compare(rhs.getName());
    }

  private:
    uint64_t m_Id{};
    std::string m_Name{};
  };
}

#endif
