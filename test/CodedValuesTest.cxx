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

#include <rexsapi/CodedValue.hxx>

#include <doctest.h>


TEST_CASE("Coded values test")
{
  SUBCASE("int32 array")
  {
    std::vector<int32_t> ints{1, 2, 3, 4, 5, 6, 7, 8};
    const auto encoded = rexsapi::detail::TCodedValueArray<int32_t>::encode(ints);
    const auto decoded = rexsapi::detail::TCodedValueArray<int32_t>::decode(encoded);
    REQUIRE(decoded.size() == 8);
    CHECK(encoded == "AQAAAAIAAAADAAAABAAAAAUAAAAGAAAABwAAAAgAAAA=");
    CHECK(decoded[0] == 1);
    CHECK(decoded[4] == 5);
    CHECK(decoded[7] == 8);
  }

  SUBCASE("float32 array")
  {
    const char* value = "MveeQZ6hM0I=";
    const auto decoded = rexsapi::detail::TCodedValueArray<float>::decode(value);

    REQUIRE(decoded.size() == 2);
    CHECK(decoded[0] == doctest::Approx(19.8707));
    CHECK(decoded[1] == doctest::Approx(44.9078));

    const auto encoded = rexsapi::detail::TCodedValueArray<float>::encode(decoded);
    CHECK(encoded == value);
  }

  SUBCASE("float64 matrix")
  {
    rexsapi::TMatrix<double> matrix{{{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0}}};
    const auto encoded = rexsapi::detail::TCodedValueMatrix<double>::encode(matrix);
    const auto decoded = rexsapi::detail::TCodedValueMatrix<double>::decode(encoded);
    REQUIRE(decoded.m_Values.size() == 3);
    REQUIRE(decoded.m_Values[0].size() == 3);
    CHECK(decoded.m_Values[0][0] == doctest::Approx{1.0});
    CHECK(decoded.m_Values[0][1] == doctest::Approx{2.0});
    CHECK(decoded.m_Values[0][2] == doctest::Approx{3.0});
    REQUIRE(decoded.m_Values[1].size() == 3);
    CHECK(decoded.m_Values[1][0] == doctest::Approx{4.0});
    CHECK(decoded.m_Values[1][1] == doctest::Approx{5.0});
    CHECK(decoded.m_Values[1][2] == doctest::Approx{6.0});
    REQUIRE(decoded.m_Values[2].size() == 3);
    CHECK(decoded.m_Values[2][0] == doctest::Approx{7.0});
    CHECK(decoded.m_Values[2][1] == doctest::Approx{8.0});
    CHECK(decoded.m_Values[2][2] == doctest::Approx{9.0});
    CHECK(encoded ==
          "AAAAAAAA8D8AAAAAAAAAQAAAAAAAAAhAAAAAAAAAEEAAAAAAAAAUQAAAAAAAABhAAAAAAAAAHEAAAAAAAAAgQAAAAAAAACJA");
  }
}

TEST_CASE("Coded value enum test")
{
  SUBCASE("From string")
  {
    CHECK(rexsapi::detail::codedValueFromString("int32") == rexsapi::detail::TCodedValueType::Int32);
    CHECK(rexsapi::detail::codedValueFromString("float32") == rexsapi::detail::TCodedValueType::Float32);
    CHECK(rexsapi::detail::codedValueFromString("float64") == rexsapi::detail::TCodedValueType::Float64);
  }

  SUBCASE("To string")
  {
    CHECK(rexsapi::detail::toCodedValueString(rexsapi::detail::TCodedValueType::None) == "none");
    CHECK(rexsapi::detail::toCodedValueString(rexsapi::detail::TCodedValueType::Int32) == "int32");
    CHECK(rexsapi::detail::toCodedValueString(rexsapi::detail::TCodedValueType::Float32) == "float32");
    CHECK(rexsapi::detail::toCodedValueString(rexsapi::detail::TCodedValueType::Float64) == "float64");
  }
}