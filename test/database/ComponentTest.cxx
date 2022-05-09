
#include <rexsapi/database/Component.hxx>

#include <doctest.h>


TEST_CASE("Component test")
{
  rexsapi::database::TUnit unitDeg{58, "deg"};
  rexsapi::database::TUnit unitMum{11, "mum"};

  rexsapi::database::TAttribute attribute1{
    "chamfer_angle_worm_wheel", "Chamfer ange", rexsapi::TValueType::FLOATING_POINT, unitDeg, "ϑ", {}, {}};
  rexsapi::database::TAttribute attribute2{"arithmetic_average_roughness_root",
                                           "Arithmetic average roughness root",
                                           rexsapi::TValueType::FLOATING_POINT,
                                           unitMum,
                                           "R_aF",
                                           {},
                                           {}};

  SUBCASE("Create component")
  {
    std::vector<std::reference_wrapper<const rexsapi::database::TAttribute>> attributes;
    attributes.emplace_back(std::ref(attribute1));
    attributes.emplace_back(std::ref(attribute2));

    rexsapi::database::TComponent component{"cylindrical_gear", "Cylindrical gear", std::move(attributes)};

    CHECK(component.getId() == "cylindrical_gear");
    CHECK(component.getName() == "Cylindrical gear");
    auto atts = component.getAttributes();
    CHECK(atts.size() == 2);
    const auto& attribute = component.findAttributeById("arithmetic_average_roughness_root");
    CHECK(attribute.getAttributeId() == "arithmetic_average_roughness_root");
    CHECK_THROWS_WITH((void)component.findAttributeById("not-available-attribute"),
                      "component 'cylindrical_gear' does not contain attribute 'not-available-attribute'");
  }
}
