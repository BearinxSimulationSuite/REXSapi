
#include <rexsapi/Model.hxx>

#include <test/TestModelLoader.hxx>

#include <doctest.h>

TEST_CASE("Model test")
{
  const auto dbModel = loadModel("1.4");

  SUBCASE("Create")
  {
    const auto* dbComponent = &dbModel.findComponentById("gear_casing");

    rexsapi::TAttributes attributes{
      rexsapi::TAttribute{dbComponent->findAttributeById("temperature_lubricant"), rexsapi::TValue{"73.2"}},
      rexsapi::TAttribute{dbComponent->findAttributeById("type_of_gear_casing_construction_vdi_2736_2014"),
                          rexsapi::TValue{"closed"}}};

    rexsapi::TComponents components;
    components.emplace_back(rexsapi::TComponent{"gear_casing", "Gehäuse", std::move(attributes)});

    dbComponent = &dbModel.findComponentById("lubricant");
    attributes.emplace_back(
      rexsapi::TAttribute{dbComponent->findAttributeById("density_at_15_degree_celsius"), rexsapi::TValue{"1.02"}});
    attributes.emplace_back(rexsapi::TAttribute{dbComponent->findAttributeById("lubricant_type_iso_6336_2006"),
                                                rexsapi::TValue{"non_water_soluble_polyglycol"}});
    attributes.emplace_back(rexsapi::TAttribute{dbComponent->findAttributeById("name"), rexsapi::TValue{"PG"}});
    attributes.emplace_back(
      rexsapi::TAttribute{dbComponent->findAttributeById("viscosity_at_100_degree_celsius"), rexsapi::TValue{"37.0"}});
    attributes.emplace_back(
      rexsapi::TAttribute{dbComponent->findAttributeById("viscosity_at_40_degree_celsius"), rexsapi::TValue{"220.0"}});

    components.emplace_back(rexsapi::TComponent{"lubricant", "S2/220", std::move(attributes)});

    rexsapi::TRelationReferences references{
      rexsapi::TRelationReference{rexsapi::TRelationRole::ORIGIN, components[0]},
      rexsapi::TRelationReference{rexsapi::TRelationRole::REFERENCED, components[1]}};


    rexsapi::TRelations relations{rexsapi::TRelation{rexsapi::TRelationType::REFERENCE, std::move(references)}};

    rexsapi::TModelInfo info{"FVA Workbench", "7.1 - DEV gültig bis 30.4.2022", "2021-12-14T15:56:10+01:00", "1.4"};
    rexsapi::TModel model{info, std::move(components), std::move(relations)};

    CHECK(model.getInfo().m_ApplicationId == "FVA Workbench");
    CHECK(model.getInfo().m_Version == "1.4");
    CHECK(model.getComponents().size() == 2);
    CHECK(model.getRelations().size() == 1);
  }
}
