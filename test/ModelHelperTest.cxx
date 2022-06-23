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

#include <rexsapi/JsonValueDecoder.hxx>
#include <rexsapi/ModelHelper.hxx>

#include <test/TestHelper.hxx>
#include <test/TestModelLoader.hxx>

#include <doctest.h>

TEST_CASE("Model helper test")
{
  const auto dbModel = loadModel("1.4");
  rexsapi::TModelHelper<rexsapi::TJsonValueDecoder> helper{rexsapi::TMode::STRICT_MODE};
  rexsapi::TResult result;
  const std::string context{"test"};

  SUBCASE("Check custom")
  {
    CHECK_FALSE(helper.checkCustom(result, context, "account_for_gravity", 42, dbModel.findComponentById("gear_unit")));
    CHECK(result);
    CHECK(helper.checkCustom(result, context, "diameter_of_helix_modification", 42,
                             dbModel.findComponentById("gear_unit")));
    CHECK_FALSE(result);
    REQUIRE(result.getErrors().size() == 1);
    CHECK(result.getErrors()[0].getMessage() ==
          "test: attribute id=diameter_of_helix_modification is not part of component id=42");

    result.reset();
    CHECK(helper.checkCustom(result, context, "custom_my_attribute", 42, dbModel.findComponentById("gear_unit")));
  }

  SUBCASE("Get value success")
  {
    rexsapi::json node = rexsapi::json::parse(R"({
      "id": "account_for_gravity", "unit": "none", "boolean": true
})");
    auto ret = helper.getValue(result, dbModel.findAttributetById("account_for_gravity"), context,
                               "account_for_gravity", 42, node);
    CHECK(ret.getValue<bool>());
    CHECK(result);
  }

  SUBCASE("Get value failure")
  {
    rexsapi::json node = rexsapi::json::parse(R"({
      "id": "account_for_gravity", "unit": "none", "integer": 42
})");
    auto ret = helper.getValue(result, dbModel.findAttributetById("account_for_gravity"), context,
                               "account_for_gravity", 42, node);
    CHECK(ret.isEmpty());
    CHECK_FALSE(result);
    REQUIRE(result.getErrors().size() == 1);
    CHECK(result.getErrors()[0].getMessage() ==
          "test: value of attribute id=account_for_gravity of component id=42 does not have the correct value type");
  }

  SUBCASE("Get value range failure")
  {
    rexsapi::json node = rexsapi::json::parse(R"({
      "id": "shear_modulus", "unit": "N / mm^2", "floating_point": -42.0
})");
    auto ret = helper.getValue(result, dbModel.findAttributetById("shear_modulus"), context, "shear_modulus", 42, node);
    CHECK(ret.getValue<double>() == doctest::Approx{-42.0});
    CHECK(result);
    REQUIRE(result.getErrors().size() == 1);
    CHECK(result.getErrors()[0].getMessage() ==
          "test: value is out of range for attribute id=shear_modulus of component id=42");
    CHECK(result.getErrors()[0].isWarning());
  }

  SUBCASE("Get value success with explicit type")
  {
    rexsapi::json node = rexsapi::json::parse(R"({
      "id": "account_for_gravity", "unit": "none", "boolean": true
})");
    auto ret = helper.getValue(result, rexsapi::TValueType::BOOLEAN, context, "account_for_gravity", 42, node);
    CHECK(ret.getValue<bool>());
    CHECK(result);
  }

  SUBCASE("Get value failure with explicit type")
  {
    rexsapi::json node = rexsapi::json::parse(R"({
      "id": "account_for_gravity", "unit": "none", "integer": 42
})");
    auto ret = helper.getValue(result, rexsapi::TValueType::BOOLEAN, context, "account_for_gravity", 42, node);
    CHECK(ret.isEmpty());
    CHECK_FALSE(result);
    REQUIRE(result.getErrors().size() == 1);
    CHECK(result.getErrors()[0].getMessage() ==
          "test: value of attribute id=account_for_gravity of component id=42 does not have the correct value type");
  }
}

TEST_CASE("Component mapping test")
{
  SUBCASE("Get id")
  {
    rexsapi::ComponentMapping mapping;
    auto component1Id = mapping.addComponent(42);
    auto component2Id = mapping.addComponent(43);
    auto component3Id = mapping.addComponent(44);

    rexsapi::TAttributes attributes;
    rexsapi::TComponents components;
    components.emplace_back(rexsapi::TComponent{component1Id, "gear_unit", "Component 1", attributes});
    components.emplace_back(rexsapi::TComponent{component2Id, "gear_casing", "Component 2", attributes});
    components.emplace_back(rexsapi::TComponent{component3Id, "material", "Component 3", attributes});

    auto component = mapping.getComponent(42, components);
    REQUIRE(component);
    CHECK(component->getInternalId() == component1Id);
    component = mapping.getComponent(43, components);
    REQUIRE(component);
    CHECK(component->getInternalId() == component2Id);
    component = mapping.getComponent(44, components);
    REQUIRE(component);
    CHECK(component->getInternalId() == component3Id);

    CHECK_FALSE(mapping.getComponent(45, components));
  }
}
