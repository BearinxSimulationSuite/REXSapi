
#include <rexsapi/database/ComponentAttributeMapper.hxx>

#include <doctest.h>

#include "TestModelLoader.hxx"

TEST_CASE("Component attribute mapper")
{
  const auto model = loadModel("1.4");

  SUBCASE("Check mappings")
  {
    std::vector<std::pair<std::string, std::string>> attributeMappings = {{"planet_carrier", "mass_of_component"},
                                                                          {"planet_carrier", "strut_inner_diameter"},
                                                                          {"profile_crowning", "profile_crowning_at_tip"}};

    rexsapi::database::TComponentAttributeMapper mapper{model, std::move(attributeMappings)};

    REQUIRE(mapper.getAttributesForComponent("planet_carrier").size() == 2);
    REQUIRE(mapper.getAttributesForComponent("profile_crowning").size() == 1);
    CHECK(mapper.getAttributesForComponent("point_list").empty());
  }

  SUBCASE("Not existing attribute mapping")
  {
    std::vector<std::pair<std::string, std::string>> attributeMappings = {
      {"planet_carrier", "mass_of_component"}, {"planet_carrier", "load_duration"}, {"profile_twist", "not existing attribute"}};

    rexsapi::database::TComponentAttributeMapper mapper{model, std::move(attributeMappings)};

    CHECK_THROWS_WITH((void)mapper.getAttributesForComponent("profile_twist"),
                      "attribute 'not existing attribute' not found for component 'profile_twist'");
  }
}
