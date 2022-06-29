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

#ifndef REXSAPI_LOAD_SPECTRUM_HXX
#define REXSAPI_LOAD_SPECTRUM_HXX

#include <rexsapi/Component.hxx>

namespace rexsapi
{
  class TLoadComponent
  {
  public:
    TLoadComponent(const TComponent& component, TAttributes attributes)
    : m_Component{component}
    , m_Attributes{attributes}
    , m_LoadAttributes{std::move(attributes)}
    {
      std::for_each(m_Component.getAttributes().begin(), m_Component.getAttributes().end(),
                    [this](const auto& attribute) {
                      m_Attributes.emplace_back(attribute);
                    });
    }

    const TComponent& getComponent() const&
    {
      return m_Component;
    }

    const TAttributes& getAttributes() const&
    {
      return m_Attributes;
    }

    const TAttributes& getLoadAttributes() const&
    {
      return m_LoadAttributes;
    }

  private:
    const TComponent& m_Component;
    TAttributes m_Attributes;
    TAttributes m_LoadAttributes;
  };

  using TLoadComponents = std::vector<TLoadComponent>;


  class TLoadCase
  {
  public:
    explicit TLoadCase(TLoadComponents components)
    : m_Components{std::move(components)}
    {
    }

    const TLoadComponents& getLoadComponents() const&
    {
      return m_Components;
    }

  private:
    TLoadComponents m_Components;
  };

  using TLoadCases = std::vector<TLoadCase>;

  class TAccumulation
  {
  public:
    explicit TAccumulation(TLoadComponents components)
    : m_Components{std::move(components)}
    {
    }

    const TLoadComponents& getLoadComponents() const&
    {
      return m_Components;
    }

  private:
    TLoadComponents m_Components;
  };


  class TLoadSpectrum
  {
  public:
    explicit TLoadSpectrum(TLoadCases loadCases, std::optional<TAccumulation> accumulation)
    : m_LoadCases{std::move(loadCases)}
    , m_Accumulation{std::move(accumulation)}
    {
    }

    [[nodiscard]] bool hasLoadCases() const
    {
      return !m_LoadCases.empty();
    }

    const TLoadCases& getLoadCases() const&
    {
      return m_LoadCases;
    }

    [[nodiscard]] bool hasAccumulation() const
    {
      return m_Accumulation.has_value();
    }

    const TAccumulation& getAccumulation() const&
    {
      if (m_Accumulation.has_value()) {
        return m_Accumulation.value();
      }
      throw TException{"model does not have an accumulation"};
    }

  private:
    TLoadCases m_LoadCases;
    std::optional<TAccumulation> m_Accumulation;
  };
}

#endif
