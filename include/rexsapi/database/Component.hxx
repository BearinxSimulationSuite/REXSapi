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

#ifndef REXSAPI_DATABASE_COMPONENT_HXX
#define REXSAPI_DATABASE_COMPONENT_HXX

#include <rexsapi/database/Attribute.hxx>

namespace rexsapi::database
{
  class TComponent
  {
  public:
    TComponent(std::string id, std::string name, std::vector<std::reference_wrapper<const TAttribute>>&& attributes)
    : m_ComponentId{std::move(id)}
    , m_Name{std::move(name)}
    , m_Attributes{std::move(attributes)}
    {
    }

    ~TComponent() = default;

    TComponent(const TComponent&) = delete;
    TComponent& operator=(const TComponent&) = delete;
    TComponent(TComponent&&) = default;
    TComponent& operator=(TComponent&&) = delete;

    [[nodiscard]] const std::string& getComponentId() const
    {
      return m_ComponentId;
    }

    [[nodiscard]] const std::string& getName() const
    {
      return m_Name;
    }

    [[nodiscard]] std::vector<std::reference_wrapper<const TAttribute>> getAttributes() const
    {
      return m_Attributes;
    }

    [[nodiscard]] const TAttribute& findAttributeById(const std::string& id) const
    {
      auto it = std::find_if(m_Attributes.begin(), m_Attributes.end(), [&id](const TAttribute& attribute) {
        return attribute.getAttributeId() == id;
      });
      if (it == m_Attributes.end()) {
        throw TException{fmt::format("component '{}' does not contain attribute '{}'", m_ComponentId, id)};
      }

      return *it;
    }

  private:
    std::string m_ComponentId;
    std::string m_Name;
    std::vector<std::reference_wrapper<const TAttribute>> m_Attributes;
  };
}

#endif
