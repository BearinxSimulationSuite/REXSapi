
#include <rexsapi/RexsVersion.hxx>

#include <doctest.h>

TEST_CASE("Rexs version test")
{
  SUBCASE("Create from string")
  {
    rexsapi::TRexsVersion version{"1.4"};
    CHECK(version.getMajor() == 1);
    CHECK(version.getMinor() == 4);
  }

  SUBCASE("Create from string fail")
  {
    CHECK_THROWS(rexsapi::TRexsVersion{"14"});
    CHECK_THROWS(rexsapi::TRexsVersion{" 1.4"});
    CHECK_THROWS(rexsapi::TRexsVersion{"hutzli"});
  }

  SUBCASE("Create from integer")
  {
    rexsapi::TRexsVersion version{1, 4};
    CHECK(version.getMajor() == 1);
    CHECK(version.getMinor() == 4);
  }

  SUBCASE("To string")
  {
    CHECK(rexsapi::TRexsVersion{"1.4"}.string() == "1.4");
    CHECK(rexsapi::TRexsVersion{"21.99"}.string() == "21.99");
  }

  SUBCASE("Compare")
  {
    CHECK(rexsapi::TRexsVersion{"1.4"} == rexsapi::TRexsVersion{"1.4"});
    CHECK(rexsapi::TRexsVersion{"1.4"} != rexsapi::TRexsVersion{"1.3"});
    CHECK_FALSE(rexsapi::TRexsVersion{"1.4"} != rexsapi::TRexsVersion{"1.4"});
    CHECK_FALSE(rexsapi::TRexsVersion{"1.4"} == rexsapi::TRexsVersion{"1.3"});

    CHECK(rexsapi::TRexsVersion{"1.4"} > rexsapi::TRexsVersion{"1.3"});
    CHECK(rexsapi::TRexsVersion{"1.3"} < rexsapi::TRexsVersion{"1.4"});
    CHECK(rexsapi::TRexsVersion{"2.1"} > rexsapi::TRexsVersion{"1.9"});
    CHECK(rexsapi::TRexsVersion{"1.9"} < rexsapi::TRexsVersion{"2.0"});

    CHECK_FALSE(rexsapi::TRexsVersion{"1.4"} < rexsapi::TRexsVersion{"1.4"});
    CHECK_FALSE(rexsapi::TRexsVersion{"1.4"} > rexsapi::TRexsVersion{"1.4"});

    CHECK(rexsapi::TRexsVersion{"1.4"} <= rexsapi::TRexsVersion{"1.4"});
    CHECK(rexsapi::TRexsVersion{"1.4"} >= rexsapi::TRexsVersion{"1.4"});
    CHECK(rexsapi::TRexsVersion{"1.4"} <= rexsapi::TRexsVersion{"2.4"});
    CHECK(rexsapi::TRexsVersion{"2.0"} >= rexsapi::TRexsVersion{"1.4"});

    CHECK_FALSE(rexsapi::TRexsVersion{"1.0"} >= rexsapi::TRexsVersion{"1.4"});
    CHECK_FALSE(rexsapi::TRexsVersion{"17.0"} <= rexsapi::TRexsVersion{"1.4"});
  }
}
