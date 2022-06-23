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

#include <rexsapi/JsonSchemaValidator.hxx>

#include <doctest.h>

TEST_CASE("Json schema validator test")
{
  const auto* schema = R"({
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "Products",
    "type": "array",
    "items": {
      "title": "Product",
      "type": "object",
      "properties": {
        "id": {
          "type": "integer"
        },
        "name": {
          "type": "string"
        },
        "description": {
          "type": "string"
        },
        "price": {
          "type": "number",
          "exclusiveMinimum": 0.0
        }
      },
      "required": ["id", "name", "price"]
    }
  })";

  const auto* invalidSchema = R"({
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "Products",
    "type": "array",
    "items": {
      "title": "Product",
      "type": "object",
      "properties": {
        "id": {
          "type": "puschel"
        },
        "name": {
          "type": "string"
        },
        "description": {
          "type": "string"
        },
        "price": {
          "type": "number",
          "exclusiveMinimum": 0.0
        }
      },
      "required": ["id", "name", "price"]
    }
  })";

  const auto* complexSchema = R"({
    "$schema": "http://json-schema.org/draft-07/schema#",
    "oneOf": [
      {
        "type": "object",
        "boolean": {
          "title": "boolean",
          "type": "boolean"
        },
        "required": ["boolean"]
      },
      {
        "string": {
          "title": "string",
          "type": "string",
          "pattern": "^.*$"
        },
        "required": ["string"]
      }
    ],

    "properties": {
      "id": {
        "title": "id",
        "type": "string",
        "pattern": "^.*$"
      },
      "unit": {
        "title": "unit",
        "type": "string",
        "pattern": "^.*$"
      }
    },
    "required": ["id"],
    "unevaluatedProperties": false
  })";

  SUBCASE("Valid schema")
  {
    rexsapi::TBufferJsonSchemaLoader loader{schema};
    rexsapi::TJsonSchemaValidator validator{loader};

    const auto* validValue = R"([
      {
        "id": 4711,
        "name": "hutzli",
        "price": 15.0
      },
      {
        "id": 815,
        "name": "puschel",
        "description": "The one and only Puschel!",
        "price": 47.11
      }
    ]
    )";

    const auto* invalidValue = R"([
      {
        "id": 4711,
        "price": 15.0
      },
      {
        "id": "815",
        "name": "puschel",
        "description": "The one and only Puschel!"
      }
    ]
    )";

    std::vector<std::string> errors;
    CHECK(validator.validate(rexsapi::json::parse(validValue), errors));
    CHECK(errors.empty());

    CHECK_FALSE(validator.validate(rexsapi::json::parse(invalidValue), errors));
    CHECK(errors.size() == 5);
  }

  SUBCASE("Valid complex schema")
  {
    const auto* value = R"({
      "id": "puschel",
      "unit": "m^2",
      "string": "hutzli"
    }
    )";
    const auto* invalidValue = R"({
      "id": "puschel",
      "unit": "m^2",
      "string": "hutzli",
      "boolean": false
    }
    )";

    rexsapi::TBufferJsonSchemaLoader loader{complexSchema};
    rexsapi::TJsonSchemaValidator validator{loader};

    std::vector<std::string> errors;
    CHECK(validator.validate(rexsapi::json::parse(value), errors));
    CHECK(errors.empty());

    CHECK_FALSE(validator.validate(rexsapi::json::parse(invalidValue), errors));
    CHECK(errors.size() == 1);
  }

  SUBCASE("Invalid schema")
  {
    rexsapi::TBufferJsonSchemaLoader loader{invalidSchema};
    CHECK_THROWS(rexsapi::TJsonSchemaValidator{loader});
  }
}
