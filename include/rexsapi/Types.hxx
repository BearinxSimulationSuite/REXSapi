
#ifndef REXSAPI_TYPES_HXX
#define REXSAPI_TYPES_HXX

#include <rexsapi/Exception.hxx>
#include <rexsapi/Format.hxx>

namespace rexsapi
{
  enum class TRelationType {
    ASSEMBLY,
    CENTRAL_SHAFT,
    CONNECTION,
    COUPLING,
    FLANK,
    MANUFACTURING_STEP,
    ORDERED_ASSEMBLY,
    ORDERED_REFERENCE,
    PLANET_CARRIER_SHAFT,
    PLANET_PIN,
    PLANET_SHAFT,
    REFERENCE,
    SIDE,
    STAGE,
    STAGE_GEAR_DATA
  };

  static TRelationType relationTypeFromString(const std::string& type);


  enum class TRelationRole {
    ASSEMBLY,
    GEAR,
    GEAR_1,
    GEAR_2,
    INNER_PART,
    LEFT,
    MANUFACTURING_SETTINGS,
    ORIGIN,
    OUTER_PART,
    PART,
    PLANETARY_STAGE,
    REFERENCED,
    RIGHT,
    SHAFT,
    SIDE_1,
    SIDE_2,
    STAGE,
    STAGE_GEAR_DATA,
    TOOL,
    WORKPIECE
  };

  static TRelationRole relationRoleFromString(const std::string& role);


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  static inline TRelationType relationTypeFromString(const std::string& type)
  {
    if (type == "assembly") {
      return TRelationType::ASSEMBLY;
    }
    if (type == "central_shaft") {
      return TRelationType::CENTRAL_SHAFT;
    }
    if (type == "connection") {
      return TRelationType::CONNECTION;
    }
    if (type == "coupling") {
      return TRelationType::COUPLING;
    }
    if (type == "flank") {
      return TRelationType::FLANK;
    }
    if (type == "manufacturing_step") {
      return TRelationType::MANUFACTURING_STEP;
    }
    if (type == "ordered_assembly") {
      return TRelationType::ORDERED_ASSEMBLY;
    }
    if (type == "ordered_reference") {
      return TRelationType::ORDERED_REFERENCE;
    }
    if (type == "planet_carrier_shaft") {
      return TRelationType::PLANET_CARRIER_SHAFT;
    }
    if (type == "planet_pin") {
      return TRelationType::PLANET_PIN;
    }
    if (type == "planet_shaft") {
      return TRelationType::PLANET_SHAFT;
    }
    if (type == "reference") {
      return TRelationType::REFERENCE;
    }
    if (type == "side") {
      return TRelationType::SIDE;
    }
    if (type == "stage") {
      return TRelationType::STAGE;
    }
    if (type == "stage_gear_data") {
      return TRelationType::STAGE_GEAR_DATA;
    }

    throw TException{fmt::format("unknown relation type '{}'", type)};
  }


  static inline TRelationRole relationRoleFromString(const std::string& role)
  {
    if (role == "assembly") {
      return TRelationRole::ASSEMBLY;
    }
    if (role == "gear") {
      return TRelationRole::GEAR;
    }
    if (role == "gear_1") {
      return TRelationRole::GEAR_1;
    }
    if (role == "gear_2") {
      return TRelationRole::GEAR_2;
    }
    if (role == "inner_part") {
      return TRelationRole::INNER_PART;
    }
    if (role == "left") {
      return TRelationRole::LEFT;
    }
    if (role == "manufacturing_settings") {
      return TRelationRole::MANUFACTURING_SETTINGS;
    }
    if (role == "origin") {
      return TRelationRole::ORIGIN;
    }
    if (role == "outer_part") {
      return TRelationRole::OUTER_PART;
    }
    if (role == "part") {
      return TRelationRole::PART;
    }
    if (role == "planetary_stage") {
      return TRelationRole::PLANETARY_STAGE;
    }
    if (role == "referenced") {
      return TRelationRole::REFERENCED;
    }
    if (role == "right") {
      return TRelationRole::RIGHT;
    }
    if (role == "shaft") {
      return TRelationRole::SHAFT;
    }
    if (role == "stage") {
      return TRelationRole::STAGE;
    }
    if (role == "side_1") {
      return TRelationRole::SIDE_1;
    }
    if (role == "side_2") {
      return TRelationRole::SIDE_2;
    }
    if (role == "stage_gear_data") {
      return TRelationRole::STAGE_GEAR_DATA;
    }
    if (role == "tool") {
      return TRelationRole::TOOL;
    }
    if (role == "workpiece") {
      return TRelationRole::WORKPIECE;
    }

    throw TException{fmt::format("unknown relation role '{}'", role)};
  }
}

#endif
