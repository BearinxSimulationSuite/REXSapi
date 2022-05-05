
#include <rexsapi/ModelLoader.hxx>
#include <rexsapi/XMLModelLoader.hxx>

#include <doctest.h>


TEST_CASE("Model loader factory test")
{
  SUBCASE("Load")
  {
    std::string buffer = R"()";

    rexsapi::TBufferLoader<rexsapi::TXMLModelLoader> loader{buffer};
    rexsapi::TLoaderResult result;
    auto model = loader.load(result);
  }
}
