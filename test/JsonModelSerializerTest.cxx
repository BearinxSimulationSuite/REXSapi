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

#include <rexsapi/JsonModelLoader.hxx>
#include <rexsapi/JsonModelSerializer.hxx>
#include <rexsapi/JsonSerializer.hxx>
#include <rexsapi/ModelLoader.hxx>

#include <test/TemporaryDirectory.hxx>
#include <test/TestHelper.hxx>
#include <test/TestModelLoader.hxx>

#include <doctest.h>


namespace
{
  class StringLoader
  {
  public:
    rexsapi::TModel load(const std::string& buffer)
    {
      rexsapi::TBufferModelLoader<rexsapi::TJsonModelValidator, rexsapi::TJsonModelLoader> loader{m_Validator, buffer};
      rexsapi::TResult result;
      auto model = loader.load(rexsapi::TMode::STRICT_MODE, result, m_Registry);
      if (!model) {
        throw rexsapi::TException{"cannot load model"};
      }
      return std::move(*model);
    }

  private:
    rexsapi::database::TModelRegistry m_Registry{createModelRegistry()};
    rexsapi::TJsonModelValidator m_Validator;
  };
}

TEST_CASE("Json model file serializer test")
{
  SUBCASE("Serialize loaded model")
  {
  }
}

TEST_CASE("Json serialize new model")
{
  const auto dbModel = loadModel("1.4");

  uint64_t componentId = 1;
  rexsapi::TComponents components;
  const auto& gearUnitComponent = dbModel.findComponentById("gear_unit");
  rexsapi::TAttributes attributes;
  // BOOLEAN
  attributes.emplace_back(rexsapi::TAttribute{gearUnitComponent.findAttributeById("account_for_gravity"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")}, rexsapi::TValue{true}});
  // INTEGER
  attributes.emplace_back(rexsapi::TAttribute{gearUnitComponent.findAttributeById("gear_shift_index"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")}, rexsapi::TValue{5}});

  components.emplace_back(
    rexsapi::TComponent{componentId++, gearUnitComponent.getComponentId(), "Getriebe", std::move(attributes)});

  attributes = rexsapi::TAttributes{};
  const auto& couplingComponent = dbModel.findComponentById("coupling");
  // FLOATING_POINT
  attributes.emplace_back(rexsapi::TAttribute{couplingComponent.findAttributeById("mass_of_component"),
                                              rexsapi::TUnit{dbModel.findUnitByName("kg")}, rexsapi::TValue{3.52}});
  // FLOATING_POINT_ARRAY
  attributes.emplace_back(rexsapi::TAttribute{couplingComponent.findAttributeById("display_color"),
                                              rexsapi::TUnit{dbModel.findUnitByName("%")},
                                              rexsapi::TValue{rexsapi::TFloatArrayType{30.0, 10.0, 55.0}}});
  // REFERENCE_COMPONENT
  attributes.emplace_back(rexsapi::TAttribute{couplingComponent.findAttributeById("reference_component_for_position"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")},
                                              rexsapi::TValue{rexsapi::TReferenceComponentType{1}}});
  components.emplace_back(
    rexsapi::TComponent{componentId++, couplingComponent.getComponentId(), "Kupplung 1", std::move(attributes)});

  attributes = rexsapi::TAttributes{};
  const auto& switchableCouplingComponent = dbModel.findComponentById("switchable_coupling");
  // BOOLEAN_ARRAY
  attributes.emplace_back(rexsapi::TAttribute{switchableCouplingComponent.findAttributeById("is_engaged"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")},
                                              rexsapi::TValue{rexsapi::TBoolArrayType{true, false}}});
  components.emplace_back(rexsapi::TComponent{componentId++, switchableCouplingComponent.getComponentId(), "Kupplung 2",
                                              std::move(attributes)});

  attributes = rexsapi::TAttributes{};
  const auto& conceptBearingComponent = dbModel.findComponentById("concept_bearing");
  // ENUM
  attributes.emplace_back(rexsapi::TAttribute{conceptBearingComponent.findAttributeById("axial_force_absorption"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")},
                                              rexsapi::TValue{"no_direction"}});
  components.emplace_back(
    rexsapi::TComponent{componentId++, conceptBearingComponent.getComponentId(), "Lager", std::move(attributes)});

  attributes = rexsapi::TAttributes{};
  const auto& elementListComponent = dbModel.findComponentById("element_list");
  // ENUM_ARRAY
  attributes.emplace_back(rexsapi::TAttribute{elementListComponent.findAttributeById("element_types"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")},
                                              rexsapi::TValue{rexsapi::TEnumArrayType{"line3", "pyramid12"}}});
  // ARRAY_OF_INTEGER_ARRAYS
  attributes.emplace_back(rexsapi::TAttribute{elementListComponent.findAttributeById("element_structure"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")},
                                              rexsapi::TValue{rexsapi::TArrayOfIntArraysType{{1, 2, 3}, {4, 5}, {6}}}});
  // INTEGER_ARRAY
  attributes.emplace_back(rexsapi::TAttribute{elementListComponent.findAttributeById("element_ids"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")},
                                              rexsapi::TValue{rexsapi::TIntArrayType{{1, 2, 3}}}});
  components.emplace_back(
    rexsapi::TComponent{componentId++, elementListComponent.getComponentId(), "Element Typ", std::move(attributes)});

  attributes = rexsapi::TAttributes{};
  const auto& assemblyGroupComponent = dbModel.findComponentById("assembly_group");
  // FILE_REFERENCE
  attributes.emplace_back(rexsapi::TAttribute{assemblyGroupComponent.findAttributeById("folder"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")},
                                              rexsapi::TValue{rexsapi::TFileReferenceType("./out")}});
  // STRING
  attributes.emplace_back(rexsapi::TAttribute{assemblyGroupComponent.findAttributeById("fem_file_format"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")},
                                              rexsapi::TValue{"puschel"}});
  // FLOATING_POINT_MATRIX
  attributes.emplace_back(rexsapi::TAttribute{
    assemblyGroupComponent.findAttributeById("reduced_static_stiffness_matrix"),
    rexsapi::TUnit{dbModel.findUnitByName("none")},
    rexsapi::TValue{rexsapi::TFloatMatrixType{{{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0}}}}});
  components.emplace_back(
    rexsapi::TComponent{componentId++, assemblyGroupComponent.getComponentId(), "Assembly", std::move(attributes)});

  // Relations
  rexsapi::TRelations relations;
  relations.emplace_back(
    rexsapi::TRelation{rexsapi::TRelationType::ASSEMBLY,
                       {},
                       rexsapi::TRelationReferences{
                         rexsapi::TRelationReference{rexsapi::TRelationRole::GEAR, "hint0", components[0]},
                         rexsapi::TRelationReference{rexsapi::TRelationRole::OUTER_PART, "hint1", components[1]}}});
  relations.emplace_back(rexsapi::TRelation{
    rexsapi::TRelationType::CONNECTION,
    {},
    rexsapi::TRelationReferences{rexsapi::TRelationReference{rexsapi::TRelationRole::LEFT, "hint2", components[2]},
                                 rexsapi::TRelationReference{rexsapi::TRelationRole::RIGHT, "hint3", components[3]}}});
  relations.emplace_back(rexsapi::TRelation{
    rexsapi::TRelationType::MANUFACTURING_STEP,
    {},
    rexsapi::TRelationReferences{rexsapi::TRelationReference{rexsapi::TRelationRole::PART, "hint4", components[4]},
                                 rexsapi::TRelationReference{rexsapi::TRelationRole::PART, "hint5", components[5]}}});

  // Load Spectrum
  rexsapi::TAttributes loadAttributes;
  loadAttributes.emplace_back(rexsapi::TAttribute{gearUnitComponent.findAttributeById("gravitational_acceleration"),
                                                  rexsapi::TUnit{dbModel.findUnitByName("m / s^2")},
                                                  rexsapi::TValue{10.5}});
  loadAttributes.emplace_back(rexsapi::TAttribute{gearUnitComponent.findAttributeById("reference_temperature"),
                                                  rexsapi::TUnit{dbModel.findUnitByName("C")}, rexsapi::TValue{55.5}});

  rexsapi::TLoadComponents loadComponents;
  loadComponents.emplace_back(rexsapi::TLoadComponent{components[0], std::move(loadAttributes)});

  loadAttributes = rexsapi::TAttributes{};
  loadAttributes.emplace_back(rexsapi::TAttribute{couplingComponent.findAttributeById("mass_of_component"),
                                                  rexsapi::TUnit{dbModel.findUnitByName("kg")}, rexsapi::TValue{5.5}});
  loadComponents.emplace_back(rexsapi::TLoadComponent{components[1], std::move(loadAttributes)});
  rexsapi::TLoadCase loadCase{std::move(loadComponents)};
  rexsapi::TLoadCases loadCases{std::move(loadCase)};
  rexsapi::TLoadSpectrum loadSpectrum{std::move(loadCases)};

  rexsapi::TModelInfo info{"REXSApi Unit Test", "1.0", "2022-05-20T08:59:10+01:00", rexsapi::TRexsVersion{"1.4"}, "en"};

  rexsapi::TModel model{info, std::move(components), std::move(relations), std::move(loadSpectrum)};

  rexsapi::JsonStringSerializer stringSerializer;
  rexsapi::JsonModelSerializer modelSerializer;

  SUBCASE("Serialize model")
  {
    modelSerializer.serialize(model, stringSerializer);
    CHECK_FALSE(stringSerializer.getModel().empty());
    std::cout << stringSerializer.getModel() << std::endl;
    /*
    StringLoader loader;
    auto roundtripModel = loader.load(stringSerializer.getModel());
    CHECK(roundtripModel.getInfo().getApplicationId() == "REXSApi Unit Test");
    CHECK(roundtripModel.getInfo().getApplicationVersion() == "1.0");
    CHECK(roundtripModel.getInfo().getDate() == "2022-05-20T08:59:10+01:00");
    CHECK(roundtripModel.getInfo().getVersion() == rexsapi::TRexsVersion{1, 4});
    REQUIRE(roundtripModel.getInfo().getApplicationLanguage().has_value());
    CHECK(*roundtripModel.getInfo().getApplicationLanguage() == "en");
    CHECK(roundtripModel.getComponents().size() == 6);
    CHECK(roundtripModel.getRelations().size() == 3);
    CHECK(roundtripModel.getLoadSpectrum().hasLoadCases());
    CHECK(roundtripModel.getLoadSpectrum().getLoadCases().size() == 1);
    */
  }
}
