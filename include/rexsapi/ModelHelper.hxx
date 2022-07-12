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

#ifndef REXSAPI_MODEL_HELPER_HXX
#define REXSAPI_MODEL_HELPER_HXX

#include <rexsapi/Mode.hxx>
#include <rexsapi/Model.hxx>
#include <rexsapi/ValidityChecker.hxx>
#include <rexsapi/database/Component.hxx>

#include <algorithm>
#include <unordered_map>

namespace rexsapi
{
  template<typename ValueDecoderType>
  class TModelHelper
  {
  public:
    explicit TModelHelper(TMode mode)
    : m_Mode{mode}
    {
    }

    bool checkCustom(TResult& result, std::string_view context, const std::string& attributeId, uint64_t componentId,
                     const database::TComponent& componentType) const
    {
      bool isCustom{false};
      if (attributeId.substr(0, 7) == "custom_") {
        isCustom = true;
      } else {
        if (!componentType.hasAttribute(attributeId)) {
          isCustom = true;
          result.addError(
            TError{m_Mode.adapt(TErrorLevel::ERR), fmt::format("{}: attribute id={} is not part of component id={}",
                                                               context, attributeId, componentId)});
        }
      }
      return isCustom;
    }

    template<typename NodeType>
    TValue getValue(TResult& result, std::string_view context, std::string_view attributeId, uint64_t componentId,
                    const database::TAttribute& dbAttribute, const NodeType& attribute) const
    {
      auto value = m_Decoder.decode(dbAttribute.getValueType(), dbAttribute.getEnums(), attribute);
      if (!value.second) {
        result.addError(
          TError{m_Mode.adapt(TErrorLevel::ERR),
                 fmt::format("{}: value of attribute id={} of component id={} does not have the correct value type",
                             context, attributeId, componentId)});
        return TValue{};
      }
      if (!TValidityChecker::check(dbAttribute, value.first)) {
        result.addError(TError{m_Mode.adapt(TErrorLevel::WARN),
                               fmt::format("{}: value is out of range for attribute id={} of component id={}", context,
                                           attributeId, componentId)});
      }

      return value.first;
    }

    template<typename NodeType>
    TValue getValue(TResult& result, TValueType valueType, std::string_view context, std::string_view attributeId,
                    uint64_t componentId, const NodeType& attribute) const
    {
      auto value = m_Decoder.decode(valueType, {}, attribute);
      if (!value.second) {
        result.addError(
          TError{m_Mode.adapt(TErrorLevel::ERR),
                 fmt::format("{}: value of attribute id={} of component id={} does not have the correct value type",
                             context, attributeId, componentId)});
        return TValue{};
      }
      return value.first;
    }

    const ValueDecoderType& getDecoder() const
    {
      return m_Decoder;
    }

  private:
    TModeAdapter m_Mode;
    ValueDecoderType m_Decoder;
  };


  class ComponentMapping
  {
  public:
    uint64_t addComponent(uint64_t componentId)
    {
      auto res = ++m_InternalComponentId;
      // TODO(lcf): check for duplicates
      m_ComponentsMapping[componentId] = res;
      return res;
    }

    inline const TComponent* getComponent(uint64_t referenceId, const TComponents& components) const&
    {
      const auto it = m_ComponentsMapping.find(referenceId);
      if (it == m_ComponentsMapping.end()) {
        return nullptr;
      }
      const auto it_comp = std::find_if(components.begin(), components.end(), [&it](const auto& comp) {
        return comp.getInternalId() == it->second;
      });
      return it_comp.operator->();
    }

  private:
    uint64_t m_InternalComponentId{0};
    std::unordered_map<uint64_t, uint64_t> m_ComponentsMapping;
  };


  class ComponentPostProcessor
  {
  public:
    ComponentPostProcessor(TResult& result, const TModeAdapter& mode, const TComponents& components,
                           const ComponentMapping& componentMapping)
    {
      process(result, mode, components, componentMapping);
    }

    TComponents&& release()
    {
      return std::move(m_Components);
    }

  private:
    void process(TResult& result, const TModeAdapter& mode, const TComponents& components,
                 const ComponentMapping& componentMapping)
    {
      for (const auto& component : components) {
        TAttributes attributes;
        for (const auto& attribute : component.getAttributes()) {
          if (attribute.getValueType() == TValueType::REFERENCE_COMPONENT && attribute.hasValue()) {
            auto id = attribute.getValue<TReferenceComponentType>();
            const auto* comp = componentMapping.getComponent(static_cast<uint64_t>(id), components);
            if (!comp) {
              result.addError(
                TError{mode.adapt(TErrorLevel::ERR), fmt::format("referenced component id={} does not exist", id)});
              continue;
            }
            attributes.emplace_back(TAttribute{attribute, TValue{static_cast<int64_t>(comp->getInternalId())}});
          } else {
            attributes.emplace_back(attribute);
          }
        }

        m_Components.emplace_back(
          TComponent{component.getInternalId(), component.getType(), component.getName(), std::move(attributes)});
      }
    }

    TComponents m_Components;
  };
}

#endif
