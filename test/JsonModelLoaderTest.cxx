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
            "id":"reference_temperature",
            "unit":"C",
            "floating_point":17.0
          },
          {
            "id":"gear_shift_index",
            "unit":"none",
            "integer":7
          },
          {
            "id":"number_of_gears",
            "integer":3
          },
          {
            "id":"account_for_gravity",
            "boolean":true
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
    ],
    "load_spectrum": {
      "id": 1,
      "load_cases": [
        {
          "id": 1,
          "components": [
            {
              "id": 1,
              "attributes": [
                { "id": "load_duration_fraction", "unit": "%", "floating_point": 15 }
              ]
            },
            {
              "id": 2,
              "attributes": [
                { "id": "load_duration_fraction", "unit": "%", "floating_point": 21 }
              ]
            }
          ]
        }
      ]
    }
  }
})";

    rexsapi::TBufferModelLoader<rexsapi::TJsonModelValidator, rexsapi::TJsonModelLoader> loader{validator, buffer};
    rexsapi::TResult result;
    auto model = loader.load(rexsapi::TMode::RELAXED_MODE, result, registry);
    CHECK(result);
    CHECK_FALSE(result.isCritical());
    REQUIRE(model);
    REQUIRE(model->getComponents().size() == 2);
    REQUIRE(model->getRelations().size() == 2);
    REQUIRE(model->getLoadSpectrum().hasLoadCases());
  }

  SUBCASE("load invalid json document")
  {
    std::string buffer = R"({
  "model":{
    "applicationId":"Bearinx",
    "applicationVersion":"12.0.8823",
    "date":"2021-07-01T12:18:38+01:00",
    "version":"1.4",
    "relations":[
    ]})";

    rexsapi::TBufferModelLoader<rexsapi::TJsonModelValidator, rexsapi::TJsonModelLoader> loader{validator, buffer};
    rexsapi::TResult result;
    auto model = loader.load(rexsapi::TMode::RELAXED_MODE, result, registry);
    CHECK_FALSE(result);
    CHECK(result.isCritical());
    CHECK_FALSE(model);
  }
}
