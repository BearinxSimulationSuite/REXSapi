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

#include <filesystem>
#include <set>
#include <vector>


static void processDirectory(bool recurse, const std::filesystem::path& path, std::set<std::filesystem::path>& models)
{
  if (std::filesystem::is_directory(path)) {
    for (const auto& entry : std::filesystem::directory_iterator{path}) {
      if (std::filesystem::is_regular_file(entry.path()) &&
          rexsapi::TExtensionChecker::getFileType(entry.path()) != rexsapi::TFileType::UNKOWN) {
        models.emplace(entry.path());
      } else if (recurse && std::filesystem::is_directory(entry.path())) {
        processDirectory(true, entry.path(), models);
      }
    }
  }
}

static std::vector<std::filesystem::path> getModels(bool recurse, const std::vector<std::filesystem::path>& paths)
{
  std::set<std::filesystem::path> models;

  for (const auto& path : paths) {
    if (std::filesystem::is_regular_file(path) &&
        rexsapi::TExtensionChecker::getFileType(path) != rexsapi::TFileType::UNKOWN) {
      models.emplace(path);
    }
    processDirectory(recurse, path, models);
  }
  return std::vector<std::filesystem::path>{models.begin(), models.end()};
}
