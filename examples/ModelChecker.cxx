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
  std::cout << "model_checker [--mode-strict|--mode-relaxed] [-w] -d <model database path> <model file> [<model file>]"
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
    bool showWarnings{false};

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
      } else if (args[n] == "-w") {
        showWarnings = true;
      } else {
        models.emplace_back(args[n]);
      }
    }
    if (modelDatabasePath.empty() || models.empty()) {
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

      std::cout << "File " << modelFile;
      if (!result) {
        std::cout << std::endl << fmt::format("  Found {} issues", result.getErrors().size()) << std::endl;
      } else {
        std::cout << " processed successfully" << std::endl;
        if (result.hasIssues() && showWarnings) {
          std::cout << fmt::format("  But has the following {} warnings", result.getErrors().size()) << std::endl;
        }
      }
      for (const auto& error : result.getErrors()) {
        if (error.isWarning() && !showWarnings) {
          continue;
        }
        std::cout << "  " << error.getMessage() << std::endl;
      }
    }
  } catch (const std::exception& ex) {
    std::cerr << "Exception caught: " << ex.what() << std::endl;
  }

  return 0;
}
