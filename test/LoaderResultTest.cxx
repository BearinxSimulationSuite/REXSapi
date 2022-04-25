
#include <rexsapi/database/LoaderResult.hxx>

#include <doctest.h>


TEST_CASE("Loader result test")
{
  SUBCASE("No errors")
  {
    rexsapi::database::TLoaderResult result{};
    CHECK(result);
  }

  SUBCASE("With errors")
  {
    rexsapi::database::TLoaderResult result{};
    result.addError(rexsapi::database::TResourceError{"my first message", 17});
    result.addError(rexsapi::database::TResourceError{"my second message", 32});

    CHECK_FALSE(result);
  }
}
