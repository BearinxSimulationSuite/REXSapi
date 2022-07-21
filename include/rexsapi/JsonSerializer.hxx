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

#ifndef REXSAPI_JSON_SERIALIZER_HXX
#define REXSAPI_JSON_SERIALIZER_HXX

#include <rexsapi/Json.hxx>

#include <filesystem>
#include <fstream>

namespace rexsapi
{
  class JsonStringSerializer
  {
  public:
    void serialize(const ordered_json& doc)
    {
      m_Model = doc.dump(2);
    }

    const std::string& getModel() const&
    {
      return m_Model;
    }

  private:
    std::string m_Model;
  };


  class JsonFileSerializer
  {
  public:
    explicit JsonFileSerializer(std::filesystem::path file)
    : m_File{std::move(file)}
    {
      auto directory = m_File.parent_path();
      if (!std::filesystem::is_directory(directory)) {
        throw TException{fmt::format("{} is not a directory or does not exist", directory.string())};
      }
    }

    void serialize(const ordered_json& doc)
    {
      std::ofstream stream{m_File};
      stream << doc.dump(2);
      stream.flush();
      if (!stream) {
        throw TException{fmt::format("Could not serialize model to {}", m_File.string())};
      }
    }

  private:
    std::filesystem::path m_File;
  };
}

#endif
