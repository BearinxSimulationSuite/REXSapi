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

#define REXSAPI_MINIZ_IMPL
#include <rexsapi/Rexsapi.hxx>

#include "Cli11.hxx"


struct Options {
  rexsapi::TMode mode{rexsapi::TMode::STRICT_MODE};
  std::filesystem::path modelDatabasePath;
  std::vector<std::filesystem::path> models;
  bool showWarnings{false};
};

static std::string getVersion()
{
  return fmt::format("model_checker version {}\n", REXSAPI_VERSION_STRING);
}

static std::optional<Options> getOptions(int argc, char** argv)
{
  Options options;
  std::filesystem::path modelsPath;

  CLI::App app{getVersion()};
  auto* strictFlag = app.add_flag(
    "--mode-strict",
    [&options](auto) {
      options.mode = rexsapi::TMode::STRICT_MODE;
    },
    "Strict standard handling");
  app
    .add_flag(
      "--mode-relaxed",
      [&options](auto) {
        options.mode = rexsapi::TMode::RELAXED_MODE;
      },
      "Relaxed standard handling")
    ->excludes(strictFlag);
  app.add_flag("-w,--warnings", options.showWarnings, "Show all warnings");
  app.add_option("-d,--database", options.modelDatabasePath, "The model database path")
    ->check(CLI::ExistingDirectory)
    ->required();

  auto* group = app.add_option_group("models", "Specify the models to check")->required();
  group->add_option("models", options.models, "The model files to check")->check(CLI::ExistingFile);
  group->add_option("--models", modelsPath, "Models directory to check")->check(CLI::ExistingDirectory);

  try {
    app.parse(argc, argv);
  } catch (const CLI::Success& e) {
    app.exit(e);
    return {};
  } catch (const CLI::ParseError& e) {
    std::cerr << getVersion() << std::endl;
    app.exit(e);
    return {};
  }

  if (!modelsPath.empty()) {
    for (auto const& entry : std::filesystem::directory_iterator{modelsPath}) {
      if (rexsapi::TExtensionChecker::getFileType(entry.path()) != rexsapi::TFileType::UNKOWN) {
        options.models.emplace_back(entry.path());
      }
    }
  }

  return options;
}


int main(int argc, char** argv)
{
  try {
    auto options = getOptions(argc, argv);
    if (!options) {
      return EXIT_FAILURE;
    }

    const rexsapi::TModelLoader loader{options->modelDatabasePath};

    bool start{true};
    for (const auto& modelFile : options->models) {
      if (start) {
        start = false;
      } else {
        std::cout << std::endl;
      }
      rexsapi::TResult result;
      const auto model = loader.load(modelFile, result, options->mode);

      std::cout << "File " << modelFile;
      if (!result) {
        std::cout << std::endl << fmt::format("  Found {} issues", result.getErrors().size()) << std::endl;
      } else {
        std::cout << " processed";
        if (result.hasIssues() && options->showWarnings) {
          std::cout << fmt::format(", but has the following {} warnings", result.getErrors().size());
        } else if (result.hasIssues() && !options->showWarnings) {
          std::cout << " with warnings";
        } else {
          std::cout << " successfully";
        }
        std::cout << std::endl;
      }
      for (const auto& error : result.getErrors()) {
        if (error.isWarning() && !options->showWarnings) {
          continue;
        }
        std::cout << "  " << error.getMessage() << std::endl;
      }
    }
  } catch (const std::exception& ex) {
    std::cerr << "Exception caught: " << ex.what() << std::endl;
  }

  return EXIT_SUCCESS;
}
