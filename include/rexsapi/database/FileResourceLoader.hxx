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

#ifndef REXSAPI_DATABASE_FILE_RESOURCE_LOADER_HXX
#define REXSAPI_DATABASE_FILE_RESOURCE_LOADER_HXX

#include <rexsapi/Exception.hxx>
#include <rexsapi/FileUtils.hxx>
#include <rexsapi/Format.hxx>
#include <rexsapi/Result.hxx>

namespace rexsapi::database
{
  class TFileResourceLoader
  {
  public:
    explicit TFileResourceLoader(std::filesystem::path path)
    : m_Path{std::move(path)}
    {
    }

    TResult load(const std::function<void(TResult&, std::vector<uint8_t>&)>& callback) const;

  private:
    [[nodiscard]] std::vector<std::filesystem::path> findResources(TResult& result) const;

    const std::filesystem::path m_Path;
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline TResult TFileResourceLoader::load(const std::function<void(TResult&, std::vector<uint8_t>&)>& callback) const
  {
    if (!callback) {
      throw TException{"callback not set for resource loader"};
    }

    TResult result;

    const auto resources = findResources(result);
    std::for_each(resources.begin(), resources.end(), [&callback, &result](const auto& resource) {
      auto buffer = loadFile(result, resource);
      if (buffer.size()) {
        callback(result, buffer);
      }
    });

    return result;
  }

  inline std::vector<std::filesystem::path> TFileResourceLoader::findResources(TResult& result) const
  {
    if (!std::filesystem::exists(m_Path) || !std::filesystem::is_directory(m_Path)) {
      throw TException{fmt::format("Directory '{}' does not exist or is not a directory", m_Path.string())};
    }

    std::vector<std::filesystem::path> resources;
    for (const auto& p : std::filesystem::directory_iterator(m_Path)) {
      if (p.path().extension() == ".xml") {
        if (!std::filesystem::is_regular_file(p.path())) {
          continue;
        }

        resources.emplace_back(p.path());
      }
    }
    if (resources.empty()) {
      result.addError(TError{TErrorLevel::CRIT, "No model database files found"});
    }

    return resources;
  }
}

#endif
