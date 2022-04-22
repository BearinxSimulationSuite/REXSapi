
#include <rexsapi/ConversionHelper.hxx>

#include <doctest.h>
#include <limits>

TEST_CASE("Conversion helper test")
{
  SUBCASE("Convert integer string")
  {
    CHECK(rexsapi::convertToUint64("4711") == 4711);
    CHECK(rexsapi::convertToUint64("  4711") == 4711);
    CHECK(rexsapi::convertToUint64("0") == 0);
    CHECK(rexsapi::convertToUint64(std::to_string(std::numeric_limits<uint64_t>::max())) == std::numeric_limits<uint64_t>::max());
    CHECK(rexsapi::convertToUint64(std::to_string(std::numeric_limits<uint64_t>::min())) == std::numeric_limits<uint64_t>::min());
  }

  SUBCASE("Conversion fail")
  {
    CHECK_THROWS_WITH(rexsapi::convertToUint64("a4711"),
                      "cannot convert string 'a4711' to unsigned integer: stoull: no conversion");
    CHECK_THROWS_WITH(rexsapi::convertToUint64("4711puschel"), "cannot convert string to unsigned integer: 4711puschel");
    CHECK_THROWS_WITH(rexsapi::convertToUint64("-4711"), "cannot convert string to unsigned integer: -4711");
    CHECK_THROWS_WITH(rexsapi::convertToUint64(std::to_string(std::numeric_limits<uint64_t>::max()) + "1"),
                      "cannot convert string '184467440737095516151' to unsigned integer: stoull: out of range");
  }
}
