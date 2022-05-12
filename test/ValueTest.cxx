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

#include <rexsapi/Value.hxx>

#include <doctest.h>
#include <vector>

TEST_CASE("Value test")
{
  SUBCASE("empty value")
  {
    rexsapi::TValue val{};
    CHECK(val.isEmpty());
    CHECK(val.asString() == "");
  }

  SUBCASE("boolean true value")
  {
    rexsapi::TValue val{true};
    CHECK_FALSE(val.isEmpty());
    CHECK(val.getValue<rexsapi::Bool>());
    CHECK(val.getValue<bool>());
    CHECK(val.asString() == "true");
  }

  SUBCASE("double value")
  {
    rexsapi::TValue val{47.11};
    CHECK_FALSE(val.isEmpty());
    CHECK(val.getValue<double>() == doctest::Approx{47.11});
    CHECK(val.asString() == "47.11");
  }

  SUBCASE("integer value")
  {
    rexsapi::TValue val{4711};
    CHECK_FALSE(val.isEmpty());
    CHECK(val.getValue<int64_t>() == 4711);
    CHECK(val.getValue<int64_t>(815) == 4711);
    CHECK(val.asString() == "4711");
  }

  SUBCASE("integer default value")
  {
    rexsapi::TValue val{};
    CHECK(val.isEmpty());
    CHECK(val.getValue<int64_t>(4711) == 4711);
    CHECK(val.asString() == "");
  }

  SUBCASE("char string value")
  {
    rexsapi::TValue val{"My String!"};
    CHECK_FALSE(val.isEmpty());
    CHECK(val.getValue<std::string>() == "My String!");
    CHECK(val.asString() == "My String!");
  }

  SUBCASE("string value")
  {
    std::string s{"My std String!"};
    rexsapi::TValue val{s};
    CHECK_FALSE(val.isEmpty());
    CHECK(val.getValue<std::string>() == "My std String!");
    CHECK(val.asString() == "My std String!");
  }

  SUBCASE("vector of integer")
  {
    std::vector<int64_t> aofi{42, 815, 4711};
    rexsapi::TValue val{aofi};
    CHECK_FALSE(val.isEmpty());
    CHECK(val.getValue<std::vector<int64_t>>().size() == 3);
    CHECK_THROWS(val.asString());
  }

  SUBCASE("vector of bool")
  {
    std::vector<rexsapi::Bool> aofb{true, true, false, true};
    rexsapi::TValue val{aofb};
    CHECK_FALSE(val.isEmpty());
    CHECK(val.getValue<std::vector<rexsapi::Bool>>().size() == 4);
    CHECK_THROWS(val.asString());
  }

  SUBCASE("vector of double")
  {
    std::vector<double> aofd{42.0, 8.15, 47.11};
    rexsapi::TValue val{aofd};
    CHECK_FALSE(val.isEmpty());
    CHECK(val.getValue<std::vector<double>>().size() == 3);
    CHECK_THROWS(val.asString());
  }

  SUBCASE("vector of strings")
  {
    std::vector<std::string> aofs{"puschel", "hutzli", "putzli"};
    rexsapi::TValue val{aofs};
    CHECK_FALSE(val.isEmpty());
    CHECK(val.getValue<std::vector<std::string>>().size() == 3);
    CHECK_THROWS(val.asString());
  }

  SUBCASE("matrix of double")
  {
    rexsapi::Matrix<double> matrix{{{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0}}};
    rexsapi::TValue val{matrix};
    CHECK_FALSE(val.isEmpty());
    CHECK_THROWS(val.asString());
  }
}
