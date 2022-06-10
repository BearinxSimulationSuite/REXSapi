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

#ifndef REXSAPI_TYPES_HXX
#define REXSAPI_TYPES_HXX

#include <rexsapi/Exception.hxx>
#include <rexsapi/Format.hxx>

#include <vector>

namespace rexsapi
{
  enum class TValueType : uint8_t {
    FLOATING_POINT,
    BOOLEAN,
    INTEGER,
    ENUM,
    STRING,
    FILE_REFERENCE,
    FLOATING_POINT_ARRAY,
    BOOLEAN_ARRAY,
    INTEGER_ARRAY,
    STRING_ARRAY,
    ENUM_ARRAY,
    REFERENCE_COMPONENT,
    FLOATING_POINT_MATRIX,
    STRING_MATRIX,
    ARRAY_OF_INTEGER_ARRAYS
  };

  static TValueType typeFromString(const std::string& type);
  static std::string toTypeString(TValueType type);

  struct Bool {
    Bool() = default;

    Bool(bool value)
    : m_Value{value}
    {
    }

    explicit operator bool() const
    {
      return m_Value;
    }

    friend bool operator==(const Bool& lhs, const Bool& rhs)
    {
      return lhs.m_Value == rhs.m_Value;
    }

    bool m_Value{false};
  };

  template<typename T>
  struct TMatrix {
    bool validate() const
    {
      if (m_Values.size()) {
        size_t n = m_Values[0].size();
        for (const auto& row : m_Values) {
          if (row.size() != n) {
            return false;
          }
        }
      }
      return true;
    }

    friend bool operator==(const TMatrix<T>& lhs, const TMatrix<T>& rhs)
    {
      return lhs.m_Values == rhs.m_Values;
    }

    std::vector<std::vector<T>> m_Values;
  };

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
  static std::string toRealtionTypeString(TRelationType type);


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
  static std::string toRealtionRoleString(TRelationRole type);


  enum class TRelationRoleType { TOP_LEVEL, SUB_LEVEL };
  static TRelationRoleType getRoleType(TRelationRole role);


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  static inline TValueType typeFromString(const std::string& type)
  {
    if (type == "floating_point") {
      return TValueType::FLOATING_POINT;
    }
    if (type == "boolean") {
      return TValueType::BOOLEAN;
    }
    if (type == "integer") {
      return TValueType::INTEGER;
    }
    if (type == "enum") {
      return TValueType::ENUM;
    }
    if (type == "string") {
      return TValueType::STRING;
    }
    if (type == "file_reference") {
      return TValueType::FILE_REFERENCE;
    }
    if (type == "boolean_array") {
      return TValueType::BOOLEAN_ARRAY;
    }
    if (type == "floating_point_array") {
      return TValueType::FLOATING_POINT_ARRAY;
    }
    if (type == "integer_array") {
      return TValueType::INTEGER_ARRAY;
    }
    if (type == "enum_array") {
      return TValueType::ENUM_ARRAY;
    }
    if (type == "string_array") {
      return TValueType::STRING_ARRAY;
    }
    if (type == "reference_component") {
      return TValueType::REFERENCE_COMPONENT;
    }
    if (type == "floating_point_matrix") {
      return TValueType::FLOATING_POINT_MATRIX;
    }
    if (type == "string_matrix") {
      return TValueType::STRING_MATRIX;
    }
    if (type == "array_of_integer_arrays") {
      return TValueType::ARRAY_OF_INTEGER_ARRAYS;
    }
    throw TException{fmt::format("unknown value type '{}'", type)};
  }

  static inline std::string toTypeString(TValueType type)
  {
    switch (type) {
      case TValueType::FLOATING_POINT:
        return "floating_point";
      case TValueType::BOOLEAN:
        return "boolean";
      case TValueType::INTEGER:
        return "integer";
      case TValueType::STRING:
        return "string";
      case TValueType::ENUM:
        return "enum";
      case TValueType::FILE_REFERENCE:
        return "file_reference";
      case TValueType::FLOATING_POINT_ARRAY:
        return "floating_point_array";
      case TValueType::BOOLEAN_ARRAY:
        return "boolean_array";
      case TValueType::INTEGER_ARRAY:
        return "integer_array";
      case TValueType::ENUM_ARRAY:
        return "enum_array";
      case TValueType::STRING_ARRAY:
        return "string_array";
      case TValueType::REFERENCE_COMPONENT:
        return "reference_component";
      case TValueType::FLOATING_POINT_MATRIX:
        return "floating_point_matrix";
      case TValueType::STRING_MATRIX:
        return "string_matrix";
      case TValueType::ARRAY_OF_INTEGER_ARRAYS:
        return "array_of_integer_arrays";
    }
    throw TException{fmt::format("unknown value type '{}'", static_cast<int64_t>(type))};
  }

  static inline std::string toRealtionTypeString(TRelationType type)
  {
    switch (type) {
      case TRelationType::ASSEMBLY:
        return "assembly";
      case TRelationType::CENTRAL_SHAFT:
        return "central_shaft";
      case TRelationType::CONNECTION:
        return "connection";
      case TRelationType::COUPLING:
        return "coupling";
      case TRelationType::FLANK:
        return "flank";
      case TRelationType::MANUFACTURING_STEP:
        return "manufacturing_step";
      case TRelationType::ORDERED_ASSEMBLY:
        return "ordered_assembly";
      case TRelationType::ORDERED_REFERENCE:
        return "ordered_reference";
      case TRelationType::PLANET_CARRIER_SHAFT:
        return "planet_carrier_shaft";
      case TRelationType::PLANET_PIN:
        return "planet_pin";
      case TRelationType::PLANET_SHAFT:
        return "planet_shaft";
      case TRelationType::REFERENCE:
        return "reference";
      case TRelationType::SIDE:
        return "side";
      case TRelationType::STAGE:
        return "stage";
      case TRelationType::STAGE_GEAR_DATA:
        return "stage_gear_data";
    }
    throw TException{"unknown relation type"};
  }


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


  static inline std::string toRealtionRoleString(TRelationRole role)
  {
    switch (role) {
      case TRelationRole::ASSEMBLY:
        return "assembly";
      case TRelationRole::GEAR:
        return "gear";
      case TRelationRole::GEAR_1:
        return "gear_1";
      case TRelationRole::GEAR_2:
        return "gear_2";
      case TRelationRole::INNER_PART:
        return "inner_part";
      case TRelationRole::LEFT:
        return "left";
      case TRelationRole::MANUFACTURING_SETTINGS:
        return "manufacturing_settings";
      case TRelationRole::ORIGIN:
        return "origin";
      case TRelationRole::OUTER_PART:
        return "outer_part";
      case TRelationRole::PART:
        return "part";
      case TRelationRole::PLANETARY_STAGE:
        return "planetary_stage";
      case TRelationRole::REFERENCED:
        return "referenced";
      case TRelationRole::RIGHT:
        return "right";
      case TRelationRole::SHAFT:
        return "shaft";
      case TRelationRole::SIDE_1:
        return "side_1";
      case TRelationRole::SIDE_2:
        return "side_2";
      case TRelationRole::STAGE:
        return "stage";
      case TRelationRole::STAGE_GEAR_DATA:
        return "stage_gear_data";
      case TRelationRole::TOOL:
        return "tool";
      case TRelationRole::WORKPIECE:
        return "workpiece";
    }
    throw TException{"unknown relation role"};
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


  static inline TRelationRoleType getRoleType(TRelationRole role)
  {
    switch (role) {
      case TRelationRole::ASSEMBLY:
      case TRelationRole::GEAR:
      case TRelationRole::ORIGIN:
      case TRelationRole::PLANETARY_STAGE:
      case TRelationRole::STAGE:
      case TRelationRole::WORKPIECE:
        return TRelationRoleType::TOP_LEVEL;
      case TRelationRole::GEAR_1:
      case TRelationRole::GEAR_2:
      case TRelationRole::INNER_PART:
      case TRelationRole::LEFT:
      case TRelationRole::MANUFACTURING_SETTINGS:
      case TRelationRole::OUTER_PART:
      case TRelationRole::PART:
      case TRelationRole::REFERENCED:
      case TRelationRole::RIGHT:
      case TRelationRole::SHAFT:
      case TRelationRole::SIDE_1:
      case TRelationRole::SIDE_2:
      case TRelationRole::STAGE_GEAR_DATA:
      case TRelationRole::TOOL:
        return TRelationRoleType::SUB_LEVEL;
    }
    throw TException{"unknown relation role"};
  }
}

#endif
