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

#ifndef REXSAPI_DATABASE_ENUM_VALUES
#define REXSAPI_DATABASE_ENUM_VALUES

#include <algorithm>
#include <string>
#include <vector>

namespace rexsapi::database
{
  struct TEnumValue {
    std::string m_Value;
    std::string m_Name;
  };

  class TEnumValues
  {
  public:
    explicit TEnumValues(std::vector<TEnumValue>&& values)
    : m_Values{std::move(values)}
    {
    }

    [[nodiscard]] bool check(const std::string& value) const;

  private:
    std::vector<TEnumValue> m_Values;
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline bool TEnumValues::check(const std::string& value) const
  {
    auto it = std::find_if(m_Values.begin(), m_Values.end(), [&value](const auto& entry) {
      return entry.m_Value == value;
    });

    return it != m_Values.end();
  }
}

#endif
