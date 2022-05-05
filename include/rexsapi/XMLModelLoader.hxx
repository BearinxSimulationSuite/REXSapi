
#ifndef REXSAPI_XML_MODEL_LOADER_HXX
#define REXSAPI_XML_MODEL_LOADER_HXX

#include <rexsapi/LoaderResult.hxx>
#include <rexsapi/Model.hxx>

namespace rexsapi
{
  class TXMLModelLoader
  {
  public:
    std::optional<TModel> load(TLoaderResult& result, const std::vector<uint8_t>& buffer) const
    {
      (void)result;
      (void)buffer;
      return {};
    }
  };
}

#endif
