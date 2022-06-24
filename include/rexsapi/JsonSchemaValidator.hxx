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

#ifndef REXSAPI_JSON_SCHEMA_VALIDATOR_HXX
#define REXSAPI_JSON_SCHEMA_VALIDATOR_HXX

#include <rexsapi/Exception.hxx>
#include <rexsapi/FileUtils.hxx>
#include <rexsapi/Format.hxx>
#include <rexsapi/Json.hxx>

#include <filesystem>
#define VALIJSON_USE_EXCEPTIONS 1
#include <valijson/adapters/nlohmann_json_adapter.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/utils/nlohmann_json_utils.hpp>
#include <valijson/validation_results.hpp>
#include <valijson/validator.hpp>
#undef VALIJSON_USE_EXCEPTIONS

namespace rexsapi
{
  class TFileJsonSchemaLoader
  {
  public:
    explicit TFileJsonSchemaLoader(std::filesystem::path jsonFile)
    : m_JsonFile{std::move(jsonFile)}
    {
    }

    [[nodiscard]] json load() const;

  private:
    std::filesystem::path m_JsonFile;
  };


  class TBufferJsonSchemaLoader
  {
  public:
    explicit TBufferJsonSchemaLoader(std::string jsonSchema)
    : m_JsonSchema{std::move(jsonSchema)}
    {
    }

    explicit TBufferJsonSchemaLoader(const char* jsonSchema)
    : m_JsonSchema{jsonSchema}
    {
    }

    [[nodiscard]] json load() const;

  private:
    std::string m_JsonSchema;
  };


  class TJsonSchemaValidator
  {
  public:
    template<typename TJsonSchemaLoader>
    explicit TJsonSchemaValidator(const TJsonSchemaLoader& loader)
    {
      const auto doc = loader.load();
      valijson::SchemaParser parser;
      valijson::adapters::NlohmannJsonAdapter schemaDocumentAdapter(doc);

      try {
        parser.populateSchema(schemaDocumentAdapter, m_Schema);
      } catch (const std::exception& ex) {
        throw TException{"Cannot populate schema"};
      }
    }

    [[nodiscard]] bool validate(const json& doc, std::vector<std::string>& errors) const;

  private:
    valijson::Schema m_Schema;
  };

  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline json TFileJsonSchemaLoader::load() const
  {
    TResult result;
    auto buffer = loadFile(result, m_JsonFile);
    if (!result) {
      throw TException{
        fmt::format("Cannot load json schema '{}': {}", m_JsonFile.string(), result.getErrors()[0].getMessage())};
    }

    json doc;
    try {
      doc = json::parse(buffer);
    } catch (const std::exception& ex) {
      throw TException{fmt::format("Cannot load json schema '{}': {}", m_JsonFile.string(), ex.what())};
    }

    return doc;
  }

  inline json TBufferJsonSchemaLoader::load() const
  {
    json doc;

    try {
      doc = json::parse(m_JsonSchema);
    } catch (const std::exception& ex) {
      throw TException{fmt::format("Cannot parse json schema: {}", ex.what())};
    }

    return doc;
  }

  inline bool TJsonSchemaValidator::validate(const json& doc, std::vector<std::string>& errors) const
  {
    valijson::Validator validator(valijson::Validator::kStrongTypes);
    valijson::ValidationResults results;
    valijson::adapters::NlohmannJsonAdapter targetDocumentAdapter(doc);
    if (!validator.validate(m_Schema, targetDocumentAdapter, &results)) {
      valijson::ValidationResults::Error error;
      unsigned int errorNum = 0;
      while (results.popError(error)) {
        std::string context;
        std::vector<std::string>::iterator it = error.context.begin();
        for (; it != error.context.end(); it++) {
          context += *it;
        }
        errors.emplace_back(fmt::format("Error #{} context: {} desc: {}", ++errorNum, context, error.description));
        ++errorNum;
      }
    }

    return errors.empty();
  }
}

#endif
