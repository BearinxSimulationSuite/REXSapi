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

#ifndef REXSAPI_DATABASE_INTERVAL_HXX
#define REXSAPI_DATABASE_INTERVAL_HXX

namespace rexsapi::database
{
  enum class TIntervalType { OPEN, CLOSED };

  class TIntervalEndpoint
  {
  public:
    TIntervalEndpoint() = default;

    TIntervalEndpoint(double value, TIntervalType type)
    : m_Set{true}
    , m_Type{type}
    , m_Value{value}
    {
    }

    [[nodiscard]] bool isSet() const
    {
      return m_Set;
    }

    bool operator<=(double value) const;

    bool operator>=(double value) const;

  private:
    bool m_Set{false};
    TIntervalType m_Type{TIntervalType::OPEN};
    double m_Value{0.0};
  };


  class TInterval
  {
  public:
    TInterval() = default;

    TInterval(TIntervalEndpoint min, TIntervalEndpoint max)
    : m_Min{min}
    , m_Max{max}
    {
    }

    [[nodiscard]] bool check(double value) const
    {
      return m_Min <= value && m_Max >= value;
    }

  private:
    TIntervalEndpoint m_Min{};
    TIntervalEndpoint m_Max{};
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline bool TIntervalEndpoint::operator<=(double value) const
  {
    if (!m_Set) {
      return true;
    }

    switch (m_Type) {
      case TIntervalType::OPEN:
        return m_Value < value;
      case TIntervalType::CLOSED:
        return m_Value <= value;
    }

    return false;
  }

  inline bool TIntervalEndpoint::operator>=(double value) const
  {
    if (!m_Set) {
      return true;
    }

    switch (m_Type) {
      case TIntervalType::OPEN:
        return m_Value > value;
      case TIntervalType::CLOSED:
        return m_Value >= value;
    }

    return false;
  }

}

#endif
