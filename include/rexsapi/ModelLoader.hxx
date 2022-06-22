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

#include <rexsapi/JsonModelLoader.hxx>
#include <rexsapi/Model.hxx>
#include <rexsapi/Result.hxx>
#include <rexsapi/XMLModelLoader.hxx>
#include <rexsapi/database/FileResourceLoader.hxx>
#include <rexsapi/database/ModelRegistry.hxx>
#include <rexsapi/database/XMLModelLoader.hxx>

#include <filesystem>
#include <fstream>
#include <sstream>

namespace rexsapi
{
  enum class TFileType { UNKOWN, XML, JSON, COMPRESSED_XML };
  TFileType fileTypeFromString(const std::string& type);

  class TExtensionChecker
  {
  public:
    static TFileType getFileType(const std::filesystem::path& path);
  };


  class TModelLoader
  {
  public:
    explicit TModelLoader(const std::filesystem::path& databasePath)
    : m_Registry{createModelRegistry(databasePath)}
    , m_XMLSchemaValidator{createSchemaValidator(databasePath)}
    {
    }

    std::optional<TModel> load(const std::filesystem::path& path, TResult& result,
                               TMode mode = TMode::STRICT_MODE) const;

  private:
    static database::TModelRegistry createModelRegistry(const std::filesystem::path& path);

    static xml::TXSDSchemaValidator createSchemaValidator(const std::filesystem::path& path);

    database::TModelRegistry m_Registry;
    xml::TXSDSchemaValidator m_XMLSchemaValidator;
    TJsonModelValidator m_JsonValidator;
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
                                             const rexsapi::database::TModelRegistry& registry)
    {
      TLoader loader{mode, m_Validator};
      return loader.load(result, registry, m_Buffer);
    }

  private:
    const TSchemaValidator& m_Validator;
    std::vector<uint8_t> m_Buffer;
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline TFileType fileTypeFromString(const std::string& type)
  {
    if (rexsapi::toupper(type) == "XML") {
      return TFileType::XML;
    }
    if (rexsapi::toupper(type) == "JSON") {
      return TFileType::JSON;
    }
    if (rexsapi::toupper(type) == "COMPRESSED_XML") {
      return TFileType::COMPRESSED_XML;
    }

    throw TException{fmt::format("unknown file type {}", type)};
  }


  inline TFileType TExtensionChecker::getFileType(const std::filesystem::path& path)
  {
    auto extension = path.extension();
    if (path.stem().has_extension()) {
      auto stem_extention = path.stem().extension();
      extension = stem_extention.replace_extension(extension);
    }

    // Attention: deliberately using path.extension() here and *not* extension
    if (path.extension() == ".rexs" || extension == ".rexs.xml") {
      return TFileType::XML;
    }
    if (path.extension() == ".rexsz" || extension == ".rexs.zip") {
      return TFileType::COMPRESSED_XML;
    }
    if (path.extension() == ".rexsj" || extension == ".rexs.json") {
      return TFileType::JSON;
    }

    throw TException{fmt::format("extension {} is not a known rexs extension", extension.string())};
  }


  inline std::optional<TModel> TModelLoader::load(const std::filesystem::path& path, TResult& result, TMode mode) const
  {
    std::optional<TModel> model;
    result.reset();

    switch (TExtensionChecker::getFileType(path)) {
      case TFileType::XML: {
        rexsapi::TFileModelLoader<xml::TXSDSchemaValidator, TXMLModelLoader> loader{m_XMLSchemaValidator, path};
        model = loader.load(mode, result, m_Registry);
        break;
      }
      case TFileType::JSON: {
        TFileModelLoader<TJsonModelValidator, TJsonModelLoader> loader{m_JsonValidator, path};
        model = loader.load(mode, result, m_Registry);
        break;
      }
      default:
        throw TException{fmt::format("extension {} currently not supported", path.extension().string())};
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

  inline xml::TXSDSchemaValidator TModelLoader::createSchemaValidator(const std::filesystem::path& path)
  {
    xml::TFileXsdSchemaLoader schemaLoader{path / "rexs-schema.xsd"};
    return xml::TXSDSchemaValidator{schemaLoader};
  }

  template<typename TSchemaValidator, typename TLoader>
  inline std::optional<TModel>
  TFileModelLoader<TSchemaValidator, TLoader>::load(TMode mode, TResult& result,
                                                    const rexsapi::database::TModelRegistry& registry)
  {
    if (!std::filesystem::exists(m_Path)) {
      result.addError(TError{TErrorLevel::CRIT, fmt::format("'{}' does not exist", m_Path.string())});
      return {};
    }
    if (!std::filesystem::is_regular_file(m_Path)) {
      result.addError(TError{TErrorLevel::CRIT, fmt::format("'{}' is not a regular file", m_Path.string())});
      return {};
    }

    std::ifstream file{m_Path};
    if (!file.good()) {
      result.addError(TError{TErrorLevel::CRIT, fmt::format("'{}' cannot be loaded", m_Path.string())});
      return {};
    }
    std::stringstream ss;
    ss << file.rdbuf();
    auto buffer = ss.str();

    std::vector<uint8_t> buf{buffer.begin(), buffer.end()};
    return TLoader{mode, m_Validator}.load(result, registry, buf);
  }
}

#endif
