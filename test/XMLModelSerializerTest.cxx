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

#include <rexsapi/ModelLoader.hxx>
#include <rexsapi/XMLModelLoader.hxx>
#include <rexsapi/XMLModelSerializer.hxx>
#include <rexsapi/XMLSerializer.hxx>

#include <test/TemporaryDirectory.hxx>
#include <test/TestHelper.hxx>
#include <test/TestModelLoader.hxx>

#include <doctest.h>

namespace
{
  class FileLoader
  {
  public:
    rexsapi::TModel load(const std::filesystem::path& modelFile)
    {
      rexsapi::TFileModelLoader loader{m_Validator, modelFile};
      rexsapi::TLoaderResult result;
      auto model = loader.load(result, m_Registry);
      if (!model) {
        throw rexsapi::TException{"cannot load model"};
      }
      return std::move(*model);
    }

  private:
    rexsapi::database::TModelRegistry m_Registry{createModelRegistry()};
    rexsapi::xml::TFileXsdSchemaLoader m_SchemaLoader{projectDir() / "models" / "rexs-schema.xsd"};
    rexsapi::xml::TXSDSchemaValidator m_Validator{m_SchemaLoader};
  };

  class StringLoader
  {
  public:
    rexsapi::TModel load(const std::string& buffer)
    {
      rexsapi::TBufferModelLoader<rexsapi::xml::TXSDSchemaValidator, rexsapi::TXMLModelLoader> loader{m_Validator,
                                                                                                      buffer};
      rexsapi::TLoaderResult result;
      auto model = loader.load(result, m_Registry);
      if (!model) {
        throw rexsapi::TException{"cannot load model"};
      }
      return std::move(*model);
    }

  private:
    rexsapi::database::TModelRegistry m_Registry{createModelRegistry()};
    rexsapi::xml::TFileXsdSchemaLoader m_SchemaLoader{projectDir() / "models" / "rexs-schema.xsd"};
    rexsapi::xml::TXSDSchemaValidator m_Validator{m_SchemaLoader};
  };
}

TEST_CASE("Model file serializer test")
{
  FileLoader loader;
  auto model = loader.load(projectDir() / "test" / "example_models" / "FVA_worm_stage_1-4.rexs");

  SUBCASE("serialize loaded model")
  {
    TemporaryDirectory tmpDir;
    rexsapi::XMLFileSerializer xmlSerializer{tmpDir.getTempDirectoryPath() / "FVA_worm_stage_1-4.rexs"};
    rexsapi::XMLModelSerializer modelSerializer;
    modelSerializer.serialize(model, xmlSerializer);
    CHECK(std::filesystem::exists(tmpDir.getTempDirectoryPath() / "FVA_worm_stage_1-4.rexs"));
    auto roundtripModel = loader.load(tmpDir.getTempDirectoryPath() / "FVA_worm_stage_1-4.rexs");
    CHECK(roundtripModel.getComponents().size() == model.getComponents().size());
    CHECK(roundtripModel.getRelations().size() == model.getRelations().size());
  }
}

TEST_CASE("Serialize new model")
{
  const auto dbModel = loadModel("1.4");

  uint64_t componentId = 1;
  rexsapi::TComponents components;
  const auto* dbComponent = &dbModel.findComponentById("gear_unit");
  rexsapi::TAttributes attributes;
  // BOOLEAN
  attributes.emplace_back(rexsapi::TAttribute{dbComponent->findAttributeById("account_for_gravity"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")}, rexsapi::TValue{true}});
  // INTEGER
  attributes.emplace_back(rexsapi::TAttribute{dbComponent->findAttributeById("gear_shift_index"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")}, rexsapi::TValue{5}});

  components.emplace_back(
    rexsapi::TComponent{componentId++, dbComponent->getComponentId(), "Getriebe", std::move(attributes)});

  attributes = rexsapi::TAttributes{};
  dbComponent = &dbModel.findComponentById("coupling");
  // FLOATING_POINT
  attributes.emplace_back(rexsapi::TAttribute{dbComponent->findAttributeById("mass_of_component"),
                                              rexsapi::TUnit{dbModel.findUnitByName("kg")}, rexsapi::TValue{3.52}});
  // FLOATING_POINT_ARRAY
  attributes.emplace_back(rexsapi::TAttribute{dbComponent->findAttributeById("display_color"),
                                              rexsapi::TUnit{dbModel.findUnitByName("%")},
                                              rexsapi::TValue{rexsapi::TFloatArrayType{30.0, 10.0, 55.0}}});
  // REFERENCE_COMPONENT
  attributes.emplace_back(rexsapi::TAttribute{dbComponent->findAttributeById("reference_component_for_position"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")},
                                              rexsapi::TValue{rexsapi::TReferenceComponentType{1}}});
  components.emplace_back(
    rexsapi::TComponent{componentId++, dbComponent->getComponentId(), "Kupplung 1", std::move(attributes)});

  attributes = rexsapi::TAttributes{};
  dbComponent = &dbModel.findComponentById("switchable_coupling");
  // BOOLEAN_ARRAY
  attributes.emplace_back(rexsapi::TAttribute{dbComponent->findAttributeById("is_engaged"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")},
                                              rexsapi::TValue{rexsapi::TBoolArrayType{true, false}}});
  components.emplace_back(
    rexsapi::TComponent{componentId++, dbComponent->getComponentId(), "Kupplung 2", std::move(attributes)});

  attributes = rexsapi::TAttributes{};
  dbComponent = &dbModel.findComponentById("concept_bearing");
  // ENUM
  attributes.emplace_back(rexsapi::TAttribute{dbComponent->findAttributeById("axial_force_absorption"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")},
                                              rexsapi::TValue{"no_direction"}});
  components.emplace_back(
    rexsapi::TComponent{componentId++, dbComponent->getComponentId(), "Lager", std::move(attributes)});

  attributes = rexsapi::TAttributes{};
  dbComponent = &dbModel.findComponentById("element_list");
  // ENUM_ARRAY
  attributes.emplace_back(rexsapi::TAttribute{dbComponent->findAttributeById("element_types"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")},
                                              rexsapi::TValue{rexsapi::TEnumArrayType{"line3", "pyramid12"}}});
  // ARRAY_OF_INTEGER_ARRAYS
  attributes.emplace_back(rexsapi::TAttribute{dbComponent->findAttributeById("element_structure"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")},
                                              rexsapi::TValue{rexsapi::TArrayOfIntArraysType{{1, 2, 3}, {4, 5}, {6}}}});
  // INTEGER_ARRAY
  attributes.emplace_back(rexsapi::TAttribute{dbComponent->findAttributeById("element_ids"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")},
                                              rexsapi::TValue{rexsapi::TIntArrayType{{1, 2, 3}}}});
  components.emplace_back(
    rexsapi::TComponent{componentId++, dbComponent->getComponentId(), "Element Typ", std::move(attributes)});

  attributes = rexsapi::TAttributes{};
  dbComponent = &dbModel.findComponentById("assembly_group");
  // FILE_REFERENCE
  attributes.emplace_back(rexsapi::TAttribute{dbComponent->findAttributeById("folder"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")},
                                              rexsapi::TValue{rexsapi::TFileReferenceType("./out")}});
  // STRING
  attributes.emplace_back(rexsapi::TAttribute{dbComponent->findAttributeById("fem_file_format"),
                                              rexsapi::TUnit{dbModel.findUnitByName("none")},
                                              rexsapi::TValue{"puschel"}});
  // FLOATING_POINT_MATRIX
  attributes.emplace_back(rexsapi::TAttribute{
    dbComponent->findAttributeById("reduced_static_stiffness_matrix"), rexsapi::TUnit{dbModel.findUnitByName("none")},
    rexsapi::TValue{rexsapi::TFloatMatrixType{{{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0}}}}});
  components.emplace_back(
    rexsapi::TComponent{componentId++, dbComponent->getComponentId(), "Assembly", std::move(attributes)});

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

  rexsapi::TModelInfo info{"REXSApi Unit Test", "1.0", "2022-05-20T08:59:10+01:00", rexsapi::TRexsVersion{"1.4"}};
  rexsapi::TModel model{info, std::move(components), std::move(relations)};

  rexsapi::XMLStringSerializer stringSerializer;
  rexsapi::XMLModelSerializer modelSerializer;

  SUBCASE("Serialize model")
  {
    modelSerializer.serialize(model, stringSerializer);
    CHECK_FALSE(stringSerializer.getModel().empty());
    StringLoader loader;
    auto roundtripModel = loader.load(stringSerializer.getModel());
    CHECK(roundtripModel.getComponents().size() == 6);
    CHECK(roundtripModel.getRelations().size() == 3);
  }
}
