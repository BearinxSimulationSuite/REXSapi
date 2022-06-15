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

    bool checkCustom(TResult& result, const std::string& context, const std::string& attributeId, uint64_t componentId,
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
    TValue getValue(TResult& result, const database::TAttribute& dbAttribute, const std::string& context,
                    const std::string& attributeId, uint64_t componentId, const NodeType& attribute) const
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
    TValue getValue(TResult& result, TValueType valueType, const std::string& context, const std::string& attributeId,
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
      m_ComponentsMapping[componentId] = res;
      return res;
    }

    inline const TComponent* getComponent(uint64_t referenceId, const TComponents& components) const&
    {
      auto it = m_ComponentsMapping.find(referenceId);
      if (it == m_ComponentsMapping.end()) {
        return nullptr;
      }
      auto it_comp = std::find_if(components.begin(), components.end(), [&it](const auto& comp) {
        return comp.getInternalId() == it->second;
      });
      return it_comp.operator->();
    }

  private:
    uint64_t m_InternalComponentId{0};
    std::unordered_map<uint64_t, uint64_t> m_ComponentsMapping;
  };
}

#endif
