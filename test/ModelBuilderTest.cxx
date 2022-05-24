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

#include <rexsapi/ModelBuilder.hxx>

#include <test/TestHelper.hxx>
#include <test/TestModelLoader.hxx>

#include <doctest.h>


TEST_CASE("Component id test")
{
  SUBCASE("Int id")
  {
    rexsapi::ComponentId id{42};
    CHECK(id.asString() == "42");
    CHECK(id == rexsapi::ComponentId{42});
    CHECK_FALSE(id == rexsapi::ComponentId{43});
    CHECK(id < rexsapi::ComponentId{43});
    CHECK_FALSE(rexsapi::ComponentId{43} < id);
    CHECK_FALSE(id < rexsapi::ComponentId{42});
    CHECK(id.hash() == rexsapi::ComponentId{42}.hash());
    CHECK_FALSE(id.hash() == rexsapi::ComponentId{40}.hash());
  }

  SUBCASE("String id")
  {
    rexsapi::ComponentId id{"puschel"};
    CHECK(id.asString() == "puschel");
    CHECK(id == rexsapi::ComponentId{"puschel"});
    CHECK_FALSE(id == rexsapi::ComponentId{"hutzli"});
    CHECK(id < rexsapi::ComponentId{"putzli"});
    CHECK_FALSE(rexsapi::ComponentId{"putzli"} < id);
    CHECK_FALSE(id < rexsapi::ComponentId{"puschel"});
    CHECK(id.hash() == rexsapi::ComponentId{"puschel"}.hash());
    CHECK_FALSE(id.hash() == rexsapi::ComponentId{"putzli"}.hash());
  }

  SUBCASE("String and int id")
  {
    rexsapi::ComponentId nid{815};
    rexsapi::ComponentId sid{"815"};
    CHECK_FALSE(nid == sid);
    CHECK(nid < sid);
    CHECK_FALSE(sid < nid);
  }
}

TEST_CASE("Component builder test")
{
  const auto registry = createModelRegistry();
  rexsapi::xml::TFileXsdSchemaLoader schemaLoader{projectDir() / "models" / "rexs-schema.xsd"};
  rexsapi::xml::TXSDSchemaValidator validator{schemaLoader};
  rexsapi::TComponentBuilder builder{registry.getModel({1, 4}, "de")};

  SUBCASE("Component builder")
  {
    builder.addComponent("cylindrical_gear").name("Zylinder").addAttribute("conversion_factor").value(2.11);
    auto gearId = builder.id();
    builder.addAttribute("display_color").value(rexsapi::TFloatArrayType{30.0, 10.0, 55.0}).unit("%");
    auto casingId =
      builder.addComponent("gear_casing", "my-id").addAttribute("temperature_lubricant").value(128.0).id();
    CHECK(casingId == rexsapi::ComponentId{"my-id"});
    auto components = builder.build();
    REQUIRE(components.size() == 2);
    CHECK(components[0].getName() == "Zylinder");
    CHECK(components[0].getType() == "cylindrical_gear");
    CHECK(components[1].getName() == "");
    CHECK(components[1].getType() == "gear_casing");
    REQUIRE(components[0].getAttributes().size() == 2);
    CHECK(components[0].getAttributes()[0].getAttributeId() == "conversion_factor");
    CHECK(components[0].getAttributes()[0].getValueAsString() == "2.11");
    CHECK(components[0].getInternalId() == builder.getComponentForId(gearId));
    CHECK(components[1].getInternalId() == builder.getComponentForId(casingId));
  }

  SUBCASE("Component builder errors")
  {
    CHECK_THROWS_WITH(builder.addAttribute("temperature_lubricant"), "no components added yet");
    CHECK_THROWS_WITH(builder.name("puschel"), "no components added yet");
    CHECK_THROWS_WITH(builder.addComponent("cylindrical_gear").value(47.11), "no attributes added yet");
    CHECK_THROWS_WITH(builder.unit("%"), "no attributes added yet");
  }
}

TEST_CASE("Model builder test")
{
  const auto registry = createModelRegistry();
  rexsapi::xml::TFileXsdSchemaLoader schemaLoader{projectDir() / "models" / "rexs-schema.xsd"};
  rexsapi::xml::TXSDSchemaValidator validator{schemaLoader};
  rexsapi::TModelBuilder builder{registry.getModel({1, 4}, "de")};

  SUBCASE("Model builder")
  {
    auto id =
      builder.addComponent("cylindrical_gear").name("Zylinder").addAttribute("conversion_factor").value(2.11).id();
    builder.addAttribute("display_color").value(rexsapi::TFloatArrayType{30.0, 10.0, 55.0}).unit("%");
    builder.addComponent("gear_casing", "my-id").addAttribute("temperature_lubricant").value(128.0);
    builder.addRelation(rexsapi::TRelationType::ASSEMBLY).addRef(rexsapi::TRelationRole::GEAR, id);
    builder.addRef(rexsapi::TRelationRole::PART, "my-id").order(1);
    builder.hint("some hint");
    auto model = builder.build("Test Appl", "1.35");
    CHECK(model.getInfo().getApplicationId() == "Test Appl");
    CHECK(model.getInfo().getApplicationVersion() == "1.35");
    CHECK(model.getInfo().getVersion() == rexsapi::TRexsVersion{1, 4});
    CHECK(model.getComponents().size() == 2);
    REQUIRE(model.getRelations().size() == 1);
    CHECK(model.getRelations()[0].getType() == rexsapi::TRelationType::ASSEMBLY);
    CHECK(model.getRelations()[0].getOrder().has_value());
  }

  SUBCASE("Model builder errors")
  {
    CHECK_THROWS(builder.addAttribute("display_color"));
    CHECK_THROWS(builder.name("Puschel"));
    auto gearId = builder.addComponent("cylindrical_gear").id();
    auto casingId = builder.addComponent("gear_casing").id();
    CHECK_THROWS(builder.value(47.11));
    CHECK_THROWS(builder.unit("%"));

    CHECK_THROWS_WITH(builder.order(5), "no relations added yet");
    CHECK_THROWS_WITH(builder.addRef(rexsapi::TRelationRole::PART, "4711"), "no relations added yet");
    builder.addRelation(rexsapi::TRelationType::ASSEMBLY);
    CHECK_THROWS_WITH(builder.hint("hint"), "no references added yet");
    builder.addRef(rexsapi::TRelationRole::GEAR, gearId);
    builder.addRef(rexsapi::TRelationRole::PART, "my-id");
    CHECK_THROWS_WITH(builder.build("Test Appl", "1.35"), "component for id 'my-id' not found");
  }
}