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

#include <rexsapi/Unit.hxx>

#include <test/TestModelLoader.hxx>

#include <doctest.h>

TEST_CASE("Unit test")
{
  auto model = loadModel("1.4");

  SUBCASE("Regular unit")
  {
    rexsapi::TUnit unit{model.findUnitById(2)};
    CHECK_FALSE(unit.isCustomUnit());
    CHECK(unit.getUnit() == "mm");
  }

  SUBCASE("Custom unit")
  {
    rexsapi::TUnit unit{"hutzli"};
    CHECK(unit.isCustomUnit());
    CHECK(unit.getUnit() == "hutzli");
  }
}
