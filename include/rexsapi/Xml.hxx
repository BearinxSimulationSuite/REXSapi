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

#ifndef REXSAPI_XML_HXX
#define REXSAPI_XML_HXX

#define PUGIXML_HEADER_ONLY
#include <pugixml.hpp>
#undef PUGIXML_HEADER_ONLY

namespace rexsapi
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
      return val == "true" ? true : false;
    }
    return def;
  }
}

#endif