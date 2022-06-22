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

#include <rexsapi/ModelLoader.hxx>

#include <test/TestHelper.hxx>
#include <test/TestModelHelper.hxx>
#include <test/TestModelLoader.hxx>

#include <doctest.h>


TEST_CASE("File type test")
{
  SUBCASE("Type from string")
  {
    CHECK(rexsapi::fileTypeFromString("xml") == rexsapi::TFileType::XML);
    CHECK(rexsapi::fileTypeFromString("XML") == rexsapi::TFileType::XML);
    CHECK(rexsapi::fileTypeFromString("json") == rexsapi::TFileType::JSON);
    CHECK(rexsapi::fileTypeFromString("JSON") == rexsapi::TFileType::JSON);
    CHECK(rexsapi::fileTypeFromString("compressed_xml") == rexsapi::TFileType::COMPRESSED_XML);

    CHECK_THROWS(rexsapi::fileTypeFromString("puschel"));
  }
}


TEST_CASE("Extension checker test")
{
  SUBCASE("Test good extensions")
  {
    rexsapi::TExtensionChecker checker;
    CHECK(checker.getFileType("model_file.rexs") == rexsapi::TFileType::XML);
    CHECK(checker.getFileType("model_file.some_other_text.rexs") == rexsapi::TFileType::XML);
    CHECK(checker.getFileType("model_file.rexs.xml") == rexsapi::TFileType::XML);
    CHECK(checker.getFileType("model_file.rexsz") == rexsapi::TFileType::COMPRESSED_XML);
    CHECK(checker.getFileType("model_file.rexs.zip") == rexsapi::TFileType::COMPRESSED_XML);
    CHECK(checker.getFileType("model_file.rexsj") == rexsapi::TFileType::JSON);
    CHECK(checker.getFileType("model_file.rexs.json") == rexsapi::TFileType::JSON);
    CHECK(checker.getFileType("model_file.some_other_text.rexs.json") == rexsapi::TFileType::JSON);
  }

  SUBCASE("Test bad extensions")
  {
    rexsapi::TExtensionChecker checker;
    CHECK_THROWS_WITH(checker.getFileType("model_file.rexsx"), "extension .rexsx is not a known rexs extension");
    CHECK_THROWS_WITH(checker.getFileType("model_file.rexs.puschel"),
                      "extension .rexs.puschel is not a known rexs extension");
    CHECK_THROWS_WITH(checker.getFileType("model_file.rexsx"), "extension .rexsx is not a known rexs extension");
  }
}


TEST_CASE("Model loader test")
{
  const rexsapi::TModelLoader loader{projectDir() / "models"};
  rexsapi::TResult result;

  SUBCASE("Load xml model relaxed")
  {
    const auto model = loader.load(projectDir() / "test" / "example_models" / "FVA-Industriegetriebe_2stufig_1-4.rexs",
                                   result, rexsapi::TMode::RELAXED_MODE);
    CHECK(result);
    CHECK(model);
  }

  SUBCASE("Load xml model strict")
  {
    const auto model = loader.load(projectDir() / "test" / "example_models" / "FVA-Industriegetriebe_2stufig_1-4.rexs",
                                   result, rexsapi::TMode::STRICT_MODE);
    CHECK_FALSE(result);
    CHECK(model);
  }

  SUBCASE("Load json model relaxed")
  {
    const auto model = loader.load(projectDir() / "test" / "example_models" / "FVA-Industriegetriebe_2stufig_1-4.rexsj",
                                   result, rexsapi::TMode::RELAXED_MODE);
    CHECK(result);
    CHECK(model);
  }

  SUBCASE("Load json model strict")
  {
    const auto model = loader.load(projectDir() / "test" / "example_models" / "FVA-Industriegetriebe_2stufig_1-4.rexsj",
                                   result, rexsapi::TMode::STRICT_MODE);
    CHECK_FALSE(result);
    CHECK(model);
  }

  SUBCASE("Load non-existent model")
  {
    const auto model = loader.load(projectDir() / "test" / "example_models" / "non-existent-model.rexsj", result,
                                   rexsapi::TMode::RELAXED_MODE);
    CHECK_FALSE(result);
    CHECK_FALSE(model);
  }
}
