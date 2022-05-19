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
#include <rexsapi/XMLModelLoader.hxx>
#include <rexsapi/XMLModelSerializer.hxx>

#include <test/TestHelper.hxx>
#include <test/TestModelLoader.hxx>

#include <doctest.h>

namespace
{
  class XMLSerializer
  {
  public:
    explicit XMLSerializer(std::filesystem::path file)
    : m_File{std::move(file)}
    {
    }

    void serialize(const pugi::xml_document& doc) const
    {
      doc.save_file(m_File.c_str());
    }

  private:
    std::filesystem::path m_File;
  };
}

TEST_CASE("Model loader test")
{
  const auto registry = createModelRegistry();
  rexsapi::xml::TFileXsdSchemaLoader schemaLoader{projectDir() / "models" / "rexs-schema.xsd"};
  rexsapi::xml::TXSDSchemaValidator validator{schemaLoader};
  rexsapi::TFileModelLoader loader{validator, projectDir() / "test" / "example_models" / "FVA_worm_stage_1-4.rexs"};
  rexsapi::TLoaderResult result;
  auto model = loader.load(result, registry);

  SUBCASE("serialize loaded model")
  {
    XMLSerializer xmlSerializer{projectDir() / "test" / "FVA_worm_stage_1-4.rexs"};
    rexsapi::XMLModelSerializer modelSerializer;
    modelSerializer.serialize(*model, xmlSerializer);
  }
}
