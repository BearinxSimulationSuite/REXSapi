
#include <rexsapi/Types.hxx>

#include <doctest.h>


TEST_CASE("Relation type test")
{
  SUBCASE("From string")
  {
    CHECK(rexsapi::relationTypeFromString("assembly") == rexsapi::TRelationType::ASSEMBLY);
    CHECK(rexsapi::relationTypeFromString("central_shaft") == rexsapi::TRelationType::CENTRAL_SHAFT);
    CHECK(rexsapi::relationTypeFromString("connection") == rexsapi::TRelationType::CONNECTION);
    CHECK(rexsapi::relationTypeFromString("coupling") == rexsapi::TRelationType::COUPLING);
    CHECK(rexsapi::relationTypeFromString("flank") == rexsapi::TRelationType::FLANK);
    CHECK(rexsapi::relationTypeFromString("manufacturing_step") == rexsapi::TRelationType::MANUFACTURING_STEP);
    CHECK(rexsapi::relationTypeFromString("ordered_assembly") == rexsapi::TRelationType::ORDERED_ASSEMBLY);
    CHECK(rexsapi::relationTypeFromString("ordered_reference") == rexsapi::TRelationType::ORDERED_REFERENCE);
    CHECK(rexsapi::relationTypeFromString("planet_carrier_shaft") == rexsapi::TRelationType::PLANET_CARRIER_SHAFT);
    CHECK(rexsapi::relationTypeFromString("planet_pin") == rexsapi::TRelationType::PLANET_PIN);
    CHECK(rexsapi::relationTypeFromString("planet_shaft") == rexsapi::TRelationType::PLANET_SHAFT);
    CHECK(rexsapi::relationTypeFromString("reference") == rexsapi::TRelationType::REFERENCE);
    CHECK(rexsapi::relationTypeFromString("side") == rexsapi::TRelationType::SIDE);
    CHECK(rexsapi::relationTypeFromString("stage") == rexsapi::TRelationType::STAGE);
    CHECK(rexsapi::relationTypeFromString("stage_gear_data") == rexsapi::TRelationType::STAGE_GEAR_DATA);
  }
}

TEST_CASE("Relation role test")
{
  SUBCASE("From string")
  {
    CHECK(rexsapi::relationRoleFromString("assembly") == rexsapi::TRelationRole::ASSEMBLY);
    CHECK(rexsapi::relationRoleFromString("gear") == rexsapi::TRelationRole::GEAR);
    CHECK(rexsapi::relationRoleFromString("gear_1") == rexsapi::TRelationRole::GEAR_1);
    CHECK(rexsapi::relationRoleFromString("gear_2") == rexsapi::TRelationRole::GEAR_2);
    CHECK(rexsapi::relationRoleFromString("inner_part") == rexsapi::TRelationRole::INNER_PART);
    CHECK(rexsapi::relationRoleFromString("left") == rexsapi::TRelationRole::LEFT);
    CHECK(rexsapi::relationRoleFromString("manufacturing_settings") == rexsapi::TRelationRole::MANUFACTURING_SETTINGS);
    CHECK(rexsapi::relationRoleFromString("origin") == rexsapi::TRelationRole::ORIGIN);
    CHECK(rexsapi::relationRoleFromString("outer_part") == rexsapi::TRelationRole::OUTER_PART);
    CHECK(rexsapi::relationRoleFromString("part") == rexsapi::TRelationRole::PART);
    CHECK(rexsapi::relationRoleFromString("planetary_stage") == rexsapi::TRelationRole::PLANETARY_STAGE);
    CHECK(rexsapi::relationRoleFromString("referenced") == rexsapi::TRelationRole::REFERENCED);
    CHECK(rexsapi::relationRoleFromString("right") == rexsapi::TRelationRole::RIGHT);
    CHECK(rexsapi::relationRoleFromString("shaft") == rexsapi::TRelationRole::SHAFT);
    CHECK(rexsapi::relationRoleFromString("side_1") == rexsapi::TRelationRole::SIDE_1);
    CHECK(rexsapi::relationRoleFromString("side_2") == rexsapi::TRelationRole::SIDE_2);
    CHECK(rexsapi::relationRoleFromString("stage") == rexsapi::TRelationRole::STAGE);
    CHECK(rexsapi::relationRoleFromString("stage_gear_data") == rexsapi::TRelationRole::STAGE_GEAR_DATA);
    CHECK(rexsapi::relationRoleFromString("tool") == rexsapi::TRelationRole::TOOL);
    CHECK(rexsapi::relationRoleFromString("workpiece") == rexsapi::TRelationRole::WORKPIECE);
  }
}
