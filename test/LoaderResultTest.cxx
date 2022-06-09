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

#include <rexsapi/LoaderResult.hxx>

#include <doctest.h>


TEST_CASE("Loader result test")
{
  SUBCASE("No errors")
  {
    rexsapi::TLoaderResult result{};
    CHECK(result);
  }

  SUBCASE("With errors")
  {
    rexsapi::TLoaderResult result{};
    result.addError(rexsapi::TError{"my first message"});
    result.addError(rexsapi::TError{"my second message", 32});

    CHECK_FALSE(result);
    REQUIRE(result.getErrors().size() == 2);
    CHECK(result.getErrors()[0].message() == "my first message");
    CHECK(result.getErrors()[1].message() == "my second message: offset 32");
  }
}
