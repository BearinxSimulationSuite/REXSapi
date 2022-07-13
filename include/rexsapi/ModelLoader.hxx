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

#ifndef REXSAPI_MODEL_LOADER_HXX
#define REXSAPI_MODEL_LOADER_HXX

#include <rexsapi/FileUtils.hxx>
#include <rexsapi/JsonModelLoader.hxx>
#include <rexsapi/Model.hxx>
#include <rexsapi/Result.hxx>
#include <rexsapi/XMLModelLoader.hxx>
#include <rexsapi/ZipArchive.hxx>
#include <rexsapi/database/FileResourceLoader.hxx>
#include <rexsapi/database/ModelRegistry.hxx>
#include <rexsapi/database/XMLModelLoader.hxx>

#include <filesystem>
#include <fstream>
#include <sstream>

namespace rexsapi
{
  class TModelLoader
  {
  public:
    explicit TModelLoader(const std::filesystem::path& databasePath)
    : m_Registry{createModelRegistry(databasePath)}
    , m_XMLSchemaValidator{createXMLSchemaValidator(databasePath)}
    , m_JsonValidator{createJsonSchemaValidator(databasePath)}
    {
    }

    std::optional<TModel> load(const std::filesystem::path& path, TResult& result,
                               TMode mode = TMode::STRICT_MODE) const;

  private:
    static database::TModelRegistry createModelRegistry(const std::filesystem::path& path);

    static xml::TXSDSchemaValidator createXMLSchemaValidator(const std::filesystem::path& path);

    static TJsonSchemaValidator createJsonSchemaValidator(const std::filesystem::path& path);

    database::TModelRegistry m_Registry;
    const xml::TXSDSchemaValidator m_XMLSchemaValidator;
    const TJsonSchemaValidator m_JsonValidator;
  };


  template<typename TSchemaValidator, typename TLoader>
  class TFileModelLoader
  {
  public:
    explicit TFileModelLoader(const TSchemaValidator& validator, std::filesystem::path path)
    : m_Validator{validator}
    , m_Path{std::move(path)}
    {
    }

    [[nodiscard]] std::optional<TModel> load(TMode mode, TResult& result,
                                             const rexsapi::database::TModelRegistry& registry);

  private:
    const TSchemaValidator& m_Validator;
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

    [[nodiscard]] std::optional<TModel> load(TMode mode, TResult& result,
                                             const rexsapi::database::TModelRegistry& registry);

  private:
    const TSchemaValidator& m_Validator;
    std::vector<uint8_t> m_Buffer;
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline std::optional<TModel> TModelLoader::load(const std::filesystem::path& path, TResult& result, TMode mode) const
  {
    std::optional<TModel> model;
    result.reset();

    switch (TExtensionChecker::getFileType(path)) {
      case TFileType::XML: {
        TFileModelLoader<xml::TXSDSchemaValidator, TXMLModelLoader> loader{m_XMLSchemaValidator, path};
        model = loader.load(mode, result, m_Registry);
        break;
      }
      case TFileType::JSON: {
        TFileModelLoader<TJsonSchemaValidator, TJsonModelLoader> loader{m_JsonValidator, path};
        model = loader.load(mode, result, m_Registry);
        break;
      }
      case TFileType::COMPRESSED: {
        ZipArchive archive{path};
        auto [buffer, type] = archive.load();
        if (type == TFileType::XML) {
          TBufferModelLoader<xml::TXSDSchemaValidator, TXMLModelLoader> loader{m_XMLSchemaValidator, std::move(buffer)};
          model = loader.load(mode, result, m_Registry);
        } else if (type == TFileType::JSON) {
          TBufferModelLoader<TJsonSchemaValidator, TJsonModelLoader> loader{m_JsonValidator, std::move(buffer)};
          model = loader.load(mode, result, m_Registry);
        }
        break;
      }
      default:
        result.addError(
          TError{TErrorLevel::CRIT, fmt::format("extension {} currently not supported", path.extension().string())});
    }

    return model;
  }

  inline database::TModelRegistry TModelLoader::createModelRegistry(const std::filesystem::path& path)
  {
    xml::TFileXsdSchemaLoader schemaLoader{path / "rexs-dbmodel.xsd"};
    database::TFileResourceLoader resourceLoader{path};
    database::TXmlModelLoader modelLoader{resourceLoader, schemaLoader};
    return database::TModelRegistry::createModelRegistry(modelLoader).first;
  }

  inline xml::TXSDSchemaValidator TModelLoader::createXMLSchemaValidator(const std::filesystem::path& path)
  {
    xml::TFileXsdSchemaLoader schemaLoader{path / "rexs-schema.xsd"};
    return xml::TXSDSchemaValidator{schemaLoader};
  }

  inline TJsonSchemaValidator TModelLoader::createJsonSchemaValidator(const std::filesystem::path& path)
  {
    TFileJsonSchemaLoader schemaLoader{path / "rexs-schema.json"};
    return TJsonSchemaValidator{schemaLoader};
  }

  template<typename TSchemaValidator, typename TLoader>
  inline std::optional<TModel>
  TBufferModelLoader<TSchemaValidator, TLoader>::load(TMode mode, TResult& result,
                                                      const rexsapi::database::TModelRegistry& registry)
  {
    TLoader loader{mode, m_Validator};
    return loader.load(result, registry, m_Buffer);
  }

  template<typename TSchemaValidator, typename TLoader>
  inline std::optional<TModel>
  TFileModelLoader<TSchemaValidator, TLoader>::load(TMode mode, TResult& result,
                                                    const rexsapi::database::TModelRegistry& registry)
  {
    auto buffer = loadFile(result, m_Path);
    if (!result) {
      return {};
    }
    return TLoader{mode, m_Validator}.load(result, registry, buffer);
  }
}

#endif
