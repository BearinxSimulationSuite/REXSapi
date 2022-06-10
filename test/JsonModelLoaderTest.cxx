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
#include <rexsapi/ModelLoader.hxx>

#include <test/TestHelper.hxx>
#include <test/TestModelLoader.hxx>

#include <doctest.h>

TEST_CASE("Json model loader test")
{
  const auto registry = createModelRegistry();
  rexsapi::TJsonModelValidator validator;

  SUBCASE("Load valid document from buffer")
  {
    std::string buffer = R"({
  "model":{
    "applicationId":"Bearinx",
    "applicationVersion":"12.0.8823",
    "date":"2021-07-01T12:18:38+01:00",
    "version":"1.4",
    "relations":[
      {
        "id":48,
        "type":"assembly",
        "refs":[
          {
            "hint":"gear_unit",
            "id":1,
            "role":"assembly"
          },
          {
            "hint":"gear_casing",
            "id":2,
            "role":"part"
          }
        ]
      },
      {
        "id":49,
        "type":"side",
        "refs":[
          {
            "hint":"concept_bearing",
            "id":40,
            "role":"assembly"
          },
          {
            "hint":"shaft",
            "id":13,
            "role":"inner_part"
          },
          {
            "hint":"gear_casing",
            "id":2,
            "role":"outer_part"
          }
        ]
      }
    ],
    "components":[
      {
        "id":1,
        "name":"Transmission unit",
        "type":"gear_unit",
        "attributes":[
          {
            "id":"name",
            "string":"30203-A"
          },
          {
            "id":"normal_module",
            "unit":"mm",
            "floating_point":2.0
          },
          {
            "id":"addendum_modification_coefficient",
            "unit":"none",
            "floating_point":0.1
          },
          {
            "id":"number_of_gears",
            "integer":3
          },
          {
            "id":"is_driving_gear",
            "boolean":false
          },
          {
            "id":"axial_force_absorption",
            "enum":"both_directions"
          },
          {
            "id":"u_axis_vector",
            "unit":"mm",
            "floating_point_array":[1.0, 0.0, 0.0]
          },
          {
            "id":"matrix_correction",
            "unit":"mm",
            "floating_point_matrix":[
              [1.0, 0.0, 0.0],
              [0.0, 1.0, 0.0],
              [0.0, 0.0, 1.0]
            ]
          },
          {
            "id":"example_attribute_id",
            "unit":"mm",
            "floating_point_array_coded":{
              "code":"float32",
              "value":"MveeQZ6hM0I"
            }
          },
          {
            "id":"element_structure",
            "array_of_integer_arrays":[
              [108, 2, 1, 107, 7],
              [0, 1, 0],
              [0, 7, 0, 1]
            ]
          }
        ]
      },
      {
        "id":2,
        "name":"Welle 1",
        "type":"shaft",
        "attributes":[
          {
            "id":"display_color",
            "unit":"%",
            "floating_point_array":[0.90, 0.80, 0.70]
          },
          {
            "id":"u_axis_vector",
            "unit":"mm",
            "floating_point_array":[1.0,0.0,0.0]
          }
        ]
      }
    ]
  }
})";

    rexsapi::TBufferModelLoader<rexsapi::TJsonModelValidator, rexsapi::TJsonModelLoader> loader{validator, buffer};
    rexsapi::TResult result;
    auto model = loader.load(rexsapi::TMode::STRICT_MODE, result, registry);
    CHECK(result);
    // REQUIRE(model);
  }
}
