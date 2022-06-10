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

#ifndef REXSAPI_JSON_MODEL_LOADER_HXX
#define REXSAPI_JSON_MODEL_LOADER_HXX

#include <rexsapi/ConversionHelper.hxx>
#include <rexsapi/Json.hxx>
#include <rexsapi/Mode.hxx>
#include <rexsapi/Model.hxx>
#include <rexsapi/ValidityChecker.hxx>
#include <rexsapi/database/ModelRegistry.hxx>

#include <set>

namespace rexsapi
{
  class TJsonModelValidator
  {
  public:
    TJsonModelValidator() = default;
  };

  class TJsonModelLoader
  {
  public:
    explicit TJsonModelLoader(TMode mode, const TJsonModelValidator& validator)
    : m_Mode{mode}
    , m_Validator{validator}
    {
    }

    std::optional<TModel> load(TResult& result, const database::TModelRegistry& registry,
                               std::vector<uint8_t>& buffer) const;

  private:
    TMode m_Mode;
    const TJsonModelValidator& m_Validator;
  };

  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline std::optional<TModel> TJsonModelLoader::load(TResult& result, const database::TModelRegistry& registry,
                                                      std::vector<uint8_t>& buffer) const
  {
    (void)registry;

    try {
      json j = json::parse(buffer);
    } catch (json::exception& ex) {
      result.addError(TError{TErrorLevel::CRIT, fmt::format("cannot parse json document: {}", ex.what())});
    }

    return {};
  }
}

#endif
