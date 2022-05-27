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

#ifndef REXSAPI_COMPONENT_HXX
#define REXSAPI_COMPONENT_HXX

#include <rexsapi/Attribute.hxx>

namespace rexsapi
{
  class TComponent
  {
  public:
    TComponent(uint64_t internalId, std::string type, std::string name, TAttributes&& attributes)
    : m_InternalId{internalId}
    , m_Type{std::move(type)}
    , m_Name{std::move(name)}
    , m_Attributes{std::move(attributes)}
    {
    }

    uint64_t getInternalId() const
    {
      return m_InternalId;
    }

    const std::string& getType() const&
    {
      return m_Type;
    }

    const std::string& getName() const&
    {
      return m_Name;
    }

    const TAttributes& getAttributes() const&
    {
      return m_Attributes;
    }

  private:
    uint64_t m_InternalId;
    std::string m_Type;
    std::string m_Name;
    TAttributes m_Attributes;
  };

  using TComponents = std::vector<TComponent>;
}

#endif
