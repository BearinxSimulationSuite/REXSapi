
#include <rexsapi/database/Unit.hxx>

#include <doctest.h>


TEST_CASE("Database unit test")
{
  SUBCASE("Create")
  {
    rexsapi::database::TUnit unit{4711, "kg"};
    CHECK(unit.getId() == 4711);
    CHECK(unit.getName() == "kg");
  }

  SUBCASE("Compare")
  {
    rexsapi::database::TUnit unit{47, "N / (mm s^0.5 K)"};
    CHECK(unit.compare("N / (mm s^0.5 K)"));
    CHECK_FALSE(unit.compare("N / mm s^0.5 K"));
    CHECK_FALSE(unit.compare(""));
  }
}
