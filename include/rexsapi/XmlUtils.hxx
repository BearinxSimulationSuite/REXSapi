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

#ifndef REXSAPI_XML_UTILS_HXX
#define REXSAPI_XML_UTILS_HXX

#include <rexsapi/LoaderResult.hxx>
#include <rexsapi/XSDSchemaValidator.hxx>
#include <rexsapi/Xml.hxx>

namespace rexsapi::xml
{
  static inline std::string getStringAttribute(const pugi::xml_node& node, const char* attribute)
  {
    return node.attribute(attribute).value();
  }

  static inline std::string getStringAttribute(const pugi::xpath_node& node, const char* attribute)
  {
    return node.node().attribute(attribute).value();
  }

  static inline std::string getStringAttribute(const pugi::xpath_node& node, const char* attribute,
                                               const std::string& def)
  {
    if (auto att = node.node().attribute(attribute); !att.empty()) {
      return att.value();
    }
    return def;
  }

  static inline bool getBoolAttribute(const pugi::xpath_node& node, const char* attribute, bool def)
  {
    if (auto att = node.node().attribute(attribute); !att.empty()) {
      std::string val{att.value()};
      return val == "true";
    }
    return def;
  }

  static inline pugi::xml_document loadXMLDocument(TLoaderResult& result, std::vector<uint8_t>& buffer,
                                                   const xml::TXSDSchemaValidator& validator)
  {
    pugi::xml_document doc;
    if (pugi::xml_parse_result parseResult = doc.load_buffer_inplace(buffer.data(), buffer.size()); !parseResult) {
      result.addError(TError{rexsapi::TErrorLevel::CRITICAL, parseResult.description(), parseResult.offset});
    } else {
      std::vector<std::string> errors;
      if (!validator.validate(doc, errors)) {
        for (const auto& error : errors) {
          result.addError(TError{rexsapi::TErrorLevel::CRITICAL, error});
        }
      }
    }

    return doc;
  }
}

#endif
