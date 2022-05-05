
#include <rexsapi/LoaderResult.hxx>

#include <doctest.h>


TEST_CASE("Loader result test")
{
  SUBCASE("No errors")
  {
    rexsapi::TLoaderResult result{};
    CHECK(result);
  }

  SUBCASE("With errors")
  {
    rexsapi::TLoaderResult result{};
    result.addError(rexsapi::TResourceError{"my first message", 17});
    result.addError(rexsapi::TResourceError{"my second message", 32});

    CHECK_FALSE(result);
  }
}
