
#ifndef REXSAPI_TYPES_HXX
#define REXSAPI_TYPES_HXX

#include <rexsapi/Exception.hxx>

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

  static inline TRelationType relationTypeFromString(const std::string& type)
  {
    if (type == "assembly") {
      return TRelationType::ASSEMBLY;
    }

    throw TException{"unknown relation type"};
  }

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

  static inline TRelationRole relationRoleFromString(const std::string& role)
  {
    if (role == "origin") {
      return TRelationRole::ORIGIN;
    }
    if (role == "assembly") {
      return TRelationRole::ASSEMBLY;
    }
    if (role == "part") {
      return TRelationRole::PART;
    }

    throw TException{"unknown relation role"};
  }
}

#endif
