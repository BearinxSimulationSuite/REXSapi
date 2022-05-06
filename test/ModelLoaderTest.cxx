
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
      <model applicationId="REXSApi Unit Test" applicationVersion="1.0" date="2022-05-05T10:35:00+02:00" version="1.4">
        <relations>
          <relation id="1" type="assembly">
            <ref hint="gear_unit" id="1" role="assembly"/>
            <ref hint="gear_casing" id="2" role="part"/>
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
        </components>
      </model>
    )";

    rexsapi::TBufferModelLoader<rexsapi::xml::TXSDSchemaValidator, rexsapi::TXMLModelLoader> loader{validator, buffer};
    rexsapi::TLoaderResult result;
    auto model = loader.load(result, registry);
    CHECK(result);
    REQUIRE(model);
    CHECK(model->getInfo().getApplicationId() == "REXSApi Unit Test");
    CHECK(model->getComponents().size() == 2);
    CHECK(model->getRelations().size() == 1);
  }

  SUBCASE("Load model from file")
  {
    rexsapi::TFileModelLoader loader{validator, projectDir() / "test" / "example_models" / "FVA_worm_stage_1-4.rexs"};
    rexsapi::TLoaderResult result;
    auto model = loader.load(result, registry);
    CHECK(result);
  }
}
