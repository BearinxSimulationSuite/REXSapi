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

#include <rexsapi/Rexsapi.hxx>

#include <iostream>

static void usage()
{
  std::cout << "model_checker <model database path> <model file> [<model file>]" << std::endl;
}

static rexsapi::database::TModelRegistry createModelRegistry(const std::filesystem::path& path)
{
  // TODO (lcf): check the path
  rexsapi::xml::TFileXsdSchemaLoader schemaLoader{path / "rexs-dbmodel.xsd"};
  rexsapi::database::TFileResourceLoader resourceLoader{path};
  rexsapi::database::TXmlModelLoader modelLoader{resourceLoader, schemaLoader};
  return rexsapi::database::TModelRegistry::createModelRegistry(modelLoader).first;
}

static rexsapi::xml::TXSDSchemaValidator createSchemaValidator(const std::filesystem::path& path)
{
  // TODO (lcf): check the path
  rexsapi::xml::TFileXsdSchemaLoader schemaLoader{path / "rexs-schema.xsd"};
  return rexsapi::xml::TXSDSchemaValidator{schemaLoader};
}


int main(int argc, char** argv)
{
  if (argc < 3) {
    usage();
    return -1;
  }

  std::filesystem::path modelDatabasePath{argv[1]};
  auto modelRegistry = createModelRegistry(modelDatabasePath);
  auto validator = createSchemaValidator(modelDatabasePath);

  for (int n = 2; n < argc; ++n) {
    if (n > 2) {
      std::cout << std::endl;
    }
    rexsapi::TFileModelLoader loader{validator, argv[n]};
    rexsapi::TResult result;
    auto model = loader.load(rexsapi::TMode::STRICT, result, modelRegistry);

    std::cout << "File " << argv[n];
    if (!result) {
      std::cout << std::endl << fmt::format("Found {} issues", result.getErrors().size()) << std::endl;
      for (const auto& error : result.getErrors()) {
        std::cout << error.message() << std::endl;
      }
    } else {
      std::cout << " processed successfully" << std::endl;
    }
  }

  return 0;
}
