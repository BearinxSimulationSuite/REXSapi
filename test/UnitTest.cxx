
#include <rexsapi/Unit.hxx>

#include <test/TestModelLoader.hxx>

#include <doctest.h>

TEST_CASE("Unit test")
{
  auto model = loadModel("1.4");

  SUBCASE("Regular unit")
  {
    rexsapi::TUnit unit{model.findUnitById(2)};
    CHECK_FALSE(unit.isCustomUnit());
    CHECK(unit.getUnit() == "mm");
  }

  SUBCASE("Custom unit")
  {
    rexsapi::TUnit unit{"hutzli"};
    CHECK(unit.isCustomUnit());
    CHECK(unit.getUnit() == "hutzli");
  }
}
