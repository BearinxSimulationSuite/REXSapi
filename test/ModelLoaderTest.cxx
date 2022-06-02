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

#include <test/TestHelper.hxx>
#include <test/TestModelLoader.hxx>

#include <doctest.h>


TEST_CASE("Model loader test")
{
  const auto registry = createModelRegistry();
  rexsapi::xml::TFileXsdSchemaLoader schemaLoader{projectDir() / "models" / "rexs-schema.xsd"};
  rexsapi::xml::TXSDSchemaValidator validator{schemaLoader};

  SUBCASE("Load model from buffer")
  {
    std::string buffer = R"(
      <?xml version="1.0" encoding="UTF-8" standalone="no"?>
      <model applicationId="REXSApi Unit Test" applicationVersion="1.0" date="2022-05-05T10:35:00+02:00" version="1.4" applicationLanguage="de">
        <relations>
          <relation id="1" type="assembly">
            <ref hint="gear_unit" id="1" role="assembly"/>
            <ref hint="gear_casing" id="2" role="part"/>
            <ref hint="external_load" id="3" role="part"/>
          </relation>
        </relations>
        <components>
          <component id="1" name="Getriebeeinheit" type="gear_unit">
            <attribute id="account_for_gravity" unit="none">true</attribute>
            <attribute id="account_for_shear_deformation" unit="none">true</attribute>
            <attribute id="direction_vector_gravity_u" unit="none">0.0</attribute>
            <attribute id="direction_vector_gravity_v" unit="none">1.0</attribute>
            <attribute id="direction_vector_gravity_w" unit="none">0.0</attribute>
            <attribute id="gear_shift_index" unit="none">1</attribute>
            <attribute id="gravitational_acceleration" unit="m / s^2">9.81</attribute>
            <attribute id="number_of_gears" unit="none">1</attribute>
            <attribute id="number_of_nodes_in_mesh_width_for_loaddistribution" unit="none">18</attribute>
            <attribute id="operating_time" unit="h">100000.0</attribute>
            <attribute id="operating_time_vdi_2736_2014" unit="h">100000.0</attribute>
            <attribute id="reference_component_for_position" unit="none">1</attribute>
            <attribute id="support_vector" unit="mm">
              <array>
                <c>0.0</c>
                <c>0.0</c>
                <c>0.0</c>
              </array>
            </attribute>
            <attribute id="u_axis_vector" unit="mm">
              <array>
                <c>1.0</c>
                <c>0.0</c>
                <c>0.0</c>
              </array>
            </attribute>
            <attribute id="w_axis_vector" unit="mm">
              <array>
                <c>0.0</c>
                <c>0.0</c>
                <c>1.0</c>
              </array>
            </attribute>
          </component>
          <component id="2" name="Gehäuse" type="gear_casing">
            <attribute id="temperature_lubricant" unit="C">73.2</attribute>
            <attribute id="type_of_gear_casing_construction_vdi_2736_2014" unit="none">closed</attribute>
          </component>
          <component id="3" name="Load" type="external_load">
            <attribute id="u_coordinate_on_shaft" unit="mm">0</attribute>
            <attribute id="transmits_torque" unit="">false</attribute>
          </component>
        </components>
        <load_spectrum id="1">
          <load_case id="1">
            <component id="2" name="Gehäuse" type="gear_casing">
              <attribute id="temperature_lubricant" unit="C">70</attribute>
            </component>
            <component id="3" name="Load" type="external_load">
              <attribute id="force_u_direction" unit="N">0</attribute>
              <attribute id="force_v_direction" unit="N">456</attribute>
              <attribute id="force_w_direction" unit="N">0</attribute>
              <attribute id="torque_around_v_axis" unit="N m">0</attribute>
              <attribute id="torque_around_w_axis" unit="N m">0</attribute>
            </component>
          </load_case>
          <load_case id="2">
            <component id="1" name="Getriebeeinheit" type="gear_unit">
              <attribute id="custom_load_duration_fraction" unit="%">30</attribute>
            </component>
          </load_case>
        </load_spectrum>
      </model>
    )";

    rexsapi::TBufferModelLoader<rexsapi::xml::TXSDSchemaValidator, rexsapi::TXMLModelLoader> loader{validator, buffer};
    rexsapi::TLoaderResult result;
    auto model = loader.load(result, registry);
    CHECK(result);
    REQUIRE(model);
    CHECK(model->getInfo().getApplicationId() == "REXSApi Unit Test");
    REQUIRE(model->getInfo().getApplicationLanguage().has_value());
    CHECK(*model->getInfo().getApplicationLanguage() == "de");
    CHECK(model->getInfo().getApplicationVersion() == "1.0");
    CHECK(model->getInfo().getDate() == "2022-05-05T10:35:00+02:00");
    CHECK(model->getInfo().getVersion() == rexsapi::TRexsVersion{1, 4});
    REQUIRE(model->getComponents().size() == 3);
    const auto& attribute = model->getComponents()[0].getAttributes()[0];
    CHECK(attribute.getAttributeId() == "account_for_gravity");
    CHECK(attribute.getValueType() == rexsapi::TValueType::BOOLEAN);
    CHECK(attribute.getValue<rexsapi::Bool>());
    CHECK(attribute.getValue<bool>());
    REQUIRE(model->getRelations().size() == 1);
    REQUIRE(model->getRelations()[0].getReferences().size() == 3);
    CHECK(model->getRelations()[0].getReferences()[0].getComponent().getType() == "gear_unit");
    CHECK(model->getRelations()[0].getReferences()[0].getComponent().getName() == "Getriebeeinheit");
    CHECK(model->getLoadSpectrum().hasLoadCases());
    REQUIRE(model->getLoadSpectrum().getLoadCases().size() == 2);
    REQUIRE(model->getLoadSpectrum().getLoadCases()[0].getLoadComponents().size() == 2);
    CHECK(model->getLoadSpectrum().getLoadCases()[0].getLoadComponents()[0].getAttributes().size() == 3);
    CHECK(model->getLoadSpectrum().getLoadCases()[0].getLoadComponents()[1].getAttributes().size() == 7);
  }

  SUBCASE("Load model from file")
  {
    rexsapi::TFileModelLoader loader{validator, projectDir() / "test" / "example_models" / "FVA_worm_stage_1-4.rexs"};
    rexsapi::TLoaderResult result;
    auto model = loader.load(result, registry);
    CHECK_FALSE(result);
    REQUIRE(result.getErrors().size() == 5);
    CHECK(result.getErrors()[0].m_Message ==
          "value of attribute 'material_type_din_743_2012' of component '238' does not have the correct value type");
    CHECK(result.getErrors()[1].m_Message ==
          "value is out of range for attribute 'thermal_expansion_coefficient_minus' of component '238'");
    CHECK(result.getErrors()[2].m_Message ==
          "value of attribute 'material_type_din_743_2012' of component '239' does not have the correct value type");
    CHECK(result.getErrors()[3].m_Message ==
          "value is out of range for attribute 'thermal_expansion_coefficient_minus' of component '239'");
    CHECK(result.getErrors()[4].m_Message ==
          "value is out of range for attribute 'throat_radius_worm_wheel' of component '9'");
  }
}
