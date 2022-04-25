
#include <rexsapi/database/ComponentAttributeMapper.hxx>

#include <doctest.h>

#include "TestModelLoader.hxx"

TEST_CASE("Component attribute mapper")
{
  const auto models = loadModels();

  SUBCASE("Check mappings")
  {
    std::vector<std::pair<std::string, std::string>> attributeMappings = {
      {"gear_unit", "gear_shift_index"}, {"gear_unit", "load_duration"}, {"lubricant", "kind"}};

    rexsapi::database::TComponentAttributeMapper mapper{models.back(), std::move(attributeMappings)};

    REQUIRE(mapper.getAttributesForComponent("gear_unit").size() == 2);
    REQUIRE(mapper.getAttributesForComponent("lubricant").size() == 1);
    CHECK(mapper.getAttributesForComponent("point_list").empty());
  }

  SUBCASE("Not existing attribute mapping")
  {
    std::vector<std::pair<std::string, std::string>> attributeMappings = {
      {"gear_unit", "gear_shift_index"}, {"gear_unit", "load_duration"}, {"lubricant", "not existing attribute"}};

    rexsapi::database::TComponentAttributeMapper mapper{models.back(), std::move(attributeMappings)};

    CHECK_THROWS_WITH((void)mapper.getAttributesForComponent("lubricant"),
                      "attribute 'not existing attribute' not found for component 'lubricant'");
  }
}
