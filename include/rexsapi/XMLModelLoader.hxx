
#ifndef REXSAPI_XML_MODEL_LOADER_HXX
#define REXSAPI_XML_MODEL_LOADER_HXX

#include <rexsapi/LoaderResult.hxx>
#include <rexsapi/Model.hxx>
#include <rexsapi/XMLParser.hxx>

namespace rexsapi
{
  class TXMLModelLoader
  {
  public:
    std::optional<TModel> load(TLoaderResult& result, std::vector<uint8_t>& buffer) const
    {
      pugi::xml_document doc;
      if (pugi::xml_parse_result parseResult = doc.load_buffer_inplace(buffer.data(), buffer.size()); !parseResult) {
        result.addError(TResourceError{parseResult.description(), parseResult.offset});
        return {};
      }

      auto rexsModel = *doc.select_nodes("/model").begin();
      TModelInfo info{rexsModel.node().attribute("applicationId").value(),
                      rexsModel.node().attribute("applicationVersion").value(),
                      rexsModel.node().attribute("date").value(), rexsModel.node().attribute("version").value()};


      return {};
    }
  };
}

#endif
