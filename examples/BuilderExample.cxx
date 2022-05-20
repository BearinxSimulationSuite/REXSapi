
#include <rexsapi/Rexsapi.hxx>

#include <test/TestHelper.hxx>

static rexsapi::TModel createModel(const rexsapi::database::TModelRegistry& registry)
{
  const rexsapi::database::TModel& databaseModel = registry.getModel(rexsapi::TRexsVersion{"1.4"}, "de");

  rexsapi::TComponentBuilder componentBuilder{databaseModel};

  auto casingId = componentBuilder.addComponent("gear_casing").name("Gehäuse").id();
  componentBuilder.addAttribute("temperature_lubricant").unit("C").value(73.2);
  componentBuilder.addAttribute("type_of_gear_casing_construction_vdi_2736_2014").value("closed");

  auto lubricantId = componentBuilder.addComponent("lubricant").name("S2/220").id();
  componentBuilder.addAttribute("density_at_15_degree_celsius").unit("kg / dm^3").value(1.02);
  componentBuilder.addAttribute("lubricant_type_iso_6336_2006").value("non_water_soluble_polyglycol");
  componentBuilder.addAttribute("name").value("PG");
  componentBuilder.addAttribute("viscosity_at_100_degree_celsius").unit("mm^2 / s").value(37.0);
  componentBuilder.addAttribute("viscosity_at_40_degree_celsius").unit("mm^2 / s").value(220.0);

  rexsapi::TModelBuilder modelBuilder{std::move(componentBuilder)};

  modelBuilder.addComponent("concept_bearing", "my-bearing-id").name("Wälzlager");
  modelBuilder.addAttribute("axial_force_absorption").value("negative");
  modelBuilder.addAttribute("inner_diameter").unit("mm").value(30.0);
  modelBuilder.addAttribute("outer_diameter").unit("mm").value(62.0);
  modelBuilder.addAttribute("radial_force_absorption").value("true");
  modelBuilder.addAttribute("width").unit("mm").value(16.0);
  modelBuilder.addAttribute("misalignment_in_v_direction").unit("mum").value(0.0);
  modelBuilder.addAttribute("misalignment_in_w_direction").unit("mum").value(0.0);
  modelBuilder.addAttribute("reference_component_for_position").value(1);
  modelBuilder.addAttribute("support_vector").unit("mm").value(rexsapi::TFloatArrayType{70.0, 0.0, 0.0});
  modelBuilder.addAttribute("u_axis_vector").unit("mm").value(rexsapi::TFloatArrayType{1.0, 0.0, 0.0});
  modelBuilder.addAttribute("u_coordinate_on_shaft_inner_side").unit("mm").value(70.0);
  modelBuilder.addAttribute("u_coordinate_on_shaft_outer_side").unit("mm").value(70.0);
  modelBuilder.addAttribute("w_axis_vector").unit("mm").value(rexsapi::TFloatArrayType{0.0, 0.0, 1.0});
  modelBuilder.addAttribute("axial_stiffness").unit("mN / mm").value(1.0E20);
  modelBuilder.addAttribute("radial_stiffness").unit("mN / mm").value(1.0E20);
  modelBuilder.addAttribute("bending_stiffness").unit("mN / rad").value(0.0);

  modelBuilder.addRelation(rexsapi::TRelationType::REFERENCE)
    .addRef(rexsapi::TRelationRole::ORIGIN, casingId)
    .addRef(rexsapi::TRelationRole::REFERENCED, lubricantId);
  modelBuilder.addRelation(rexsapi::TRelationType::REFERENCE)
    .addRef(rexsapi::TRelationRole::ORIGIN, "my-bearing-id")
    .addRef(rexsapi::TRelationRole::REFERENCED, lubricantId);

  return modelBuilder.build("REXSApi Model Builder", "1.0");
}

static rexsapi::database::TModelRegistry createModelRegistry()
{
  rexsapi::xml::TFileXsdSchemaLoader schemaLoader{projectDir() / "models" / "rexs-dbmodel.xsd"};
  rexsapi::database::TFileResourceLoader resourceLoader{projectDir() / "models"};
  rexsapi::database::TXmlModelLoader modelLoader{resourceLoader, schemaLoader};
  return rexsapi::database::TModelRegistry::createModelRegistry(modelLoader).first;
}


int main(int, char**)
{
  auto model = createModel(createModelRegistry());

  return 0;
}