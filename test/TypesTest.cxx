/*
 * Copyright Schaeffler Technologies AG & Co. KG (info.de@schaeffler.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
    CHECK_THROWS(rexsapi::relationTypeFromString("unknown relation type"));
  }

  SUBCASE("To string")
  {
    CHECK(rexsapi::toRealtionTypeString(rexsapi::TRelationType::ASSEMBLY) == "assembly");
    CHECK(rexsapi::toRealtionTypeString(rexsapi::TRelationType::CENTRAL_SHAFT) == "central_shaft");
    CHECK(rexsapi::toRealtionTypeString(rexsapi::TRelationType::CONNECTION) == "connection");
    CHECK(rexsapi::toRealtionTypeString(rexsapi::TRelationType::COUPLING) == "coupling");
    CHECK(rexsapi::toRealtionTypeString(rexsapi::TRelationType::FLANK) == "flank");
    CHECK(rexsapi::toRealtionTypeString(rexsapi::TRelationType::MANUFACTURING_STEP) == "manufacturing_step");
    CHECK(rexsapi::toRealtionTypeString(rexsapi::TRelationType::ORDERED_ASSEMBLY) == "ordered_assembly");
    CHECK(rexsapi::toRealtionTypeString(rexsapi::TRelationType::ORDERED_REFERENCE) == "ordered_reference");
    CHECK(rexsapi::toRealtionTypeString(rexsapi::TRelationType::PLANET_CARRIER_SHAFT) == "planet_carrier_shaft");
    CHECK(rexsapi::toRealtionTypeString(rexsapi::TRelationType::PLANET_PIN) == "planet_pin");
    CHECK(rexsapi::toRealtionTypeString(rexsapi::TRelationType::PLANET_SHAFT) == "planet_shaft");
    CHECK(rexsapi::toRealtionTypeString(rexsapi::TRelationType::REFERENCE) == "reference");
    CHECK(rexsapi::toRealtionTypeString(rexsapi::TRelationType::SIDE) == "side");
    CHECK(rexsapi::toRealtionTypeString(rexsapi::TRelationType::STAGE) == "stage");
    CHECK(rexsapi::toRealtionTypeString(rexsapi::TRelationType::STAGE_GEAR_DATA) == "stage_gear_data");
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
    CHECK_THROWS(rexsapi::relationRoleFromString("unknown relation role"));
  }

  SUBCASE("To string")
  {
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::ASSEMBLY) == "assembly");
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::GEAR) == "gear");
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::GEAR_1) == "gear_1");
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::GEAR_2) == "gear_2");
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::INNER_PART) == "inner_part");
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::LEFT) == "left");
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::MANUFACTURING_SETTINGS) == "manufacturing_settings");
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::ORIGIN) == "origin");
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::OUTER_PART) == "outer_part");
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::PART) == "part");
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::PLANETARY_STAGE) == "planetary_stage");
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::REFERENCED) == "referenced");
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::RIGHT) == "right");
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::SHAFT) == "shaft");
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::SIDE_1) == "side_1");
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::SIDE_2) == "side_2");
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::STAGE) == "stage");
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::STAGE_GEAR_DATA) == "stage_gear_data");
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::TOOL) == "tool");
    CHECK(rexsapi::toRealtionRoleString(rexsapi::TRelationRole::WORKPIECE) == "workpiece");
  }
}

TEST_CASE("Coded value test")
{
  SUBCASE("From string")
  {
    CHECK(rexsapi::codedValueFromString("int32") == rexsapi::TCodedValueType::Int32);
    CHECK(rexsapi::codedValueFromString("float32") == rexsapi::TCodedValueType::Float32);
    CHECK(rexsapi::codedValueFromString("float64") == rexsapi::TCodedValueType::Float64);
  }

  SUBCASE("To string")
  {
    CHECK(rexsapi::toCodedValueString(rexsapi::TCodedValueType::None) == "none");
    CHECK(rexsapi::toCodedValueString(rexsapi::TCodedValueType::Int32) == "int32");
    CHECK(rexsapi::toCodedValueString(rexsapi::TCodedValueType::Float32) == "float32");
    CHECK(rexsapi::toCodedValueString(rexsapi::TCodedValueType::Float64) == "float64");
  }
}

TEST_CASE("Bool test")
{
  rexsapi::Bool bTrue{true};
  rexsapi::Bool bFalse{false};

  SUBCASE("Create")
  {
    CHECK(bTrue);
    CHECK_FALSE(bFalse);
  }

  SUBCASE("Compare")
  {
    CHECK_FALSE(bTrue == bFalse);
    CHECK_FALSE(bFalse == bTrue);
    CHECK(bTrue == true);
    CHECK(bFalse == false);
    CHECK(true == bTrue);
    CHECK(false == bFalse);

    CHECK(bTrue != bFalse);
    CHECK_FALSE(bTrue != true);
    CHECK_FALSE(bFalse != false);
    CHECK_FALSE(true != bTrue);
    CHECK_FALSE(false != bFalse);
  }

  SUBCASE("Operators")
  {
    CHECK(*bTrue);
    CHECK_FALSE(*bFalse);
  }
}
