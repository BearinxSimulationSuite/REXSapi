
#include <rexsapi/database/FileResourceLoader.hxx>

#include <test/TestHelper.hxx>

#include <doctest.h>

namespace
{
  void checkBuffer(const std::vector<uint8_t>& buffer)
  {
    std::string v{buffer.begin(), buffer.end()};
    CHECK(v.find("<?xml") != std::string::npos);
    CHECK(v.find("<rexsModel") != std::string::npos);
    CHECK(v.find("</rexsModel>") != std::string::npos);
  }
}

TEST_CASE("File resource loader test")
{
  SUBCASE("Load existing resources")
  {
    rexsapi::database::TFileResourceLoader loader{projectDir() / "models"};

    std::vector<std::vector<uint8_t>> buffers;
    loader.load([&buffers](const rexsapi::TLoaderResult&, std::vector<uint8_t>& buffer) {
      buffers.emplace_back(buffer);
    });

    CHECK(buffers.size() == 10);
    std::for_each(buffers.begin(), buffers.end(), [](const auto& buf) {
      checkBuffer(buf);
    });
  }

  SUBCASE("Load not existing path")
  {
    rexsapi::database::TFileResourceLoader loader{projectDir() / "non-existing-models"};
    CHECK_THROWS(loader.load([](const rexsapi::TLoaderResult&, std::vector<uint8_t>&) {
    }));
  }
}