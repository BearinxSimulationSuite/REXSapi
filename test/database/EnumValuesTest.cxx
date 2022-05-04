
#include <rexsapi/database/EnumValues.hxx>

#include <doctest.h>


TEST_CASE("Enum values test")
{
  std::vector<rexsapi::database::TEnumValue> values;
  values.emplace_back(rexsapi::database::TEnumValue{"both_directions", "Both directions"});
  values.emplace_back(rexsapi::database::TEnumValue{"negative", "Negative u-direction"});
  values.emplace_back(rexsapi::database::TEnumValue{"no_direction", "No direction"});
  values.emplace_back(rexsapi::database::TEnumValue{"positive", "Positive u-direction"});

  rexsapi::database::TEnumValues enumValues{std::move(values)};

  SUBCASE("Check existing value")
  {
    CHECK(enumValues.check("both_directions"));
    CHECK(enumValues.check("negative"));
    CHECK(enumValues.check("no_direction"));
    CHECK(enumValues.check("positive"));
  }

  SUBCASE("Check non existing value")
  {
    CHECK_FALSE(enumValues.check("this_is_no_value"));
    CHECK_FALSE(enumValues.check(""));
  }
}
