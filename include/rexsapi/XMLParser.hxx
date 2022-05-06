
#ifndef REXSAPI_XML_PARSER_HXX
#define REXSAPI_XML_PARSER_HXX

#define PUGIXML_HEADER_ONLY
#include <pugixml.hpp>
#undef PUGIXML_HEADER_ONLY

namespace rexsapi
{
  static inline std::string getAttribute(const pugi::xml_node& node, const char* attribute)
  {
    return node.attribute(attribute).value();
  }

  static inline std::string getAttribute(const pugi::xpath_node& node, const char* attribute)
  {
    return node.node().attribute(attribute).value();
  }

  static inline std::string getAttribute(const pugi::xpath_node& node, const char* attribute, const std::string& def)
  {
    if (auto att = node.node().attribute(attribute); !att.empty()) {
      return att.value();
    }
    return def;
  }
}

#endif
