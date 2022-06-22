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
  std::cout << "model_converter [--mode-strict|--mode-relaxed] -f|--format <xml|json> -o|--output <output path for "
               "converted models> -d "
               "<model database path> <model file> [<model file>]"
            << std::endl;
}

static std::vector<std::string> getArgs(int argc, char** argv)
{
  std::vector<std::string> args;

  for (auto n = 1; n < argc; ++n) {
    args.emplace_back(std::string{argv[n]});
  }

  return args;
}


int main(int argc, char** argv)
{
  try {
    rexsapi::TMode mode{rexsapi::TMode::STRICT_MODE};
    std::filesystem::path modelDatabasePath;
    std::vector<std::filesystem::path> models;
    std::filesystem::path outputPath;
    rexsapi::TFileType type{rexsapi::TFileType::UNKOWN};

    const auto& args = getArgs(argc, argv);
    for (size_t n = 0; n < args.size(); ++n) {
      if (args[n] == "--mode-strict") {
        mode = rexsapi::TMode::STRICT_MODE;
      } else if (args[n] == "--mode-relaxed") {
        mode = rexsapi::TMode::RELAXED_MODE;
      } else if (args[n] == "-d") {
        if (args.size() < n + 1) {
          usage();
          return -1;
        }
        modelDatabasePath = args[++n];
      } else if (args[n] == "--output" || args[n] == "-o") {
        if (args.size() < n + 1) {
          usage();
          return -1;
        }
        outputPath = args[++n];
      } else if (args[n] == "--format" || args[n] == "-f") {
        if (args.size() < n + 1) {
          usage();
          return -1;
        }
        type = rexsapi::fileTypeFromString(args[++n]);
      } else {
        models.emplace_back(args[n]);
      }
    }
    if (modelDatabasePath.empty() || outputPath.empty() || models.empty() || type == rexsapi::TFileType::UNKOWN) {
      usage();
      return -1;
    }

    const rexsapi::TModelLoader loader{modelDatabasePath};

    bool start{true};
    for (const auto& modelFile : models) {
      if (start) {
        start = false;
      } else {
        std::cout << std::endl;
      }
      rexsapi::TResult result;
      const auto model = loader.load(modelFile, result, mode);
      if (!model) {
        std::cerr << "Error: could not load model " << modelFile << std::endl;
      } else {
        auto file{modelFile.filename()};
        switch (type) {
          case rexsapi::TFileType::JSON: {
            rexsapi::JsonFileSerializer fileSerializer{outputPath / file.replace_extension(".rexsj")};
            rexsapi::JsonModelSerializer modelSerializer;
            modelSerializer.serialize(*model, fileSerializer);
            break;
          }
          case rexsapi::TFileType::XML: {
            rexsapi::XMLFileSerializer fileSerializer{outputPath / file.replace_extension(".rexs")};
            rexsapi::XMLModelSerializer modelSerializer;
            modelSerializer.serialize(*model, fileSerializer);
            break;
          }
          default:
            throw rexsapi::TException{"Format is not implemented"};
        }
        std::cout << fmt::format("Converted {} to {}", modelFile.string(), file.string()) << std::endl;
      }
    }
  } catch (const std::exception& ex) {
    std::cerr << "Exception caught: " << ex.what() << std::endl;
  }

  return 0;
}
