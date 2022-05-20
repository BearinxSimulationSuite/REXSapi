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

#ifndef REXSAPI_MODEL_LOADER_FACTORY_HXX
#define REXSAPI_MODEL_LOADER_FACTORY_HXX

#include <rexsapi/LoaderResult.hxx>
#include <rexsapi/Model.hxx>
#include <rexsapi/XMLModelLoader.hxx>
#include <rexsapi/database/ModelRegistry.hxx>

#include <filesystem>
#include <fstream>
#include <sstream>

namespace rexsapi
{

  class TFileModelLoader
  {
  public:
    explicit TFileModelLoader(const xml::TXSDSchemaValidator& validator, std::filesystem::path path)
    : m_Validator{validator}
    , m_Path{std::move(path)}
    {
    }

    [[nodiscard]] std::optional<TModel> load(TLoaderResult& result, const rexsapi::database::TModelRegistry& registry);

  private:
    const xml::TXSDSchemaValidator& m_Validator;
    std::filesystem::path m_Path;
  };


  template<typename TSchemaValidator, typename TLoader>
  class TBufferModelLoader
  {
  public:
    explicit TBufferModelLoader(const TSchemaValidator& validator, const std::string& buffer)
    : m_Validator{validator}
    , m_Buffer{buffer.begin(), buffer.end()}
    {
    }

    explicit TBufferModelLoader(const TSchemaValidator& validator, std::vector<uint8_t> buffer)
    : m_Validator{validator}
    , m_Buffer{std::move(buffer)}
    {
    }

    [[nodiscard]] std::optional<TModel> load(TLoaderResult& result, const rexsapi::database::TModelRegistry& registry)
    {
      TLoader loader{m_Validator};
      return loader.load(result, registry, m_Buffer);
    }

  private:
    const TSchemaValidator& m_Validator;
    std::vector<uint8_t> m_Buffer;
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline std::optional<TModel> TFileModelLoader::load(TLoaderResult& result,
                                                      const rexsapi::database::TModelRegistry& registry)
  {
    if (!std::filesystem::exists(m_Path)) {
      result.addError(TResourceError{fmt::format("'{}' does not exist", m_Path.string())});
      return {};
    }
    if (!std::filesystem::is_regular_file(m_Path)) {
      result.addError(TResourceError{fmt::format("'{}' is not a regular file", m_Path.string())});
      return {};
    }
    // TODO (lcf): use extension mapper
    if (m_Path.extension() != ".rexs") {
      result.addError(TResourceError{fmt::format("'{}' is not a model file", m_Path.string())});
      return {};
    }

    std::ifstream file{m_Path};
    if (!file.good()) {
      result.addError(TResourceError{fmt::format("'{}' cannot be loaded", m_Path.string())});
      return {};
    }
    std::stringstream ss;
    ss << file.rdbuf();
    auto buffer = ss.str();

    std::vector<uint8_t> buf{buffer.begin(), buffer.end()};
    return TXMLModelLoader{m_Validator}.load(result, registry, buf);
  }
}

#endif
