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

#ifndef REXSAPI_FILE_TYPES_HXX
#define REXSAPI_FILE_TYPES_HXX

#include <rexsapi/ConversionHelper.hxx>
#include <rexsapi/Format.hxx>

#include <filesystem>

namespace rexsapi
{
  enum class TFileType { UNKOWN, XML, JSON, COMPRESSED };
  TFileType fileTypeFromString(const std::string& type);

  class TExtensionChecker
  {
  public:
    static TFileType getFileType(const std::filesystem::path& path);
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline TFileType fileTypeFromString(const std::string& type)
  {
    if (rexsapi::toupper(type) == "XML") {
      return TFileType::XML;
    }
    if (rexsapi::toupper(type) == "JSON") {
      return TFileType::JSON;
    }

    throw TException{fmt::format("unknown file type {}", type)};
  }


  inline TFileType TExtensionChecker::getFileType(const std::filesystem::path& path)
  {
    auto extension = path.extension();
    if (path.stem().has_extension()) {
      auto stem_extention = path.stem().extension();
      extension = stem_extention.replace_extension(extension);
    }

    // Attention: deliberately using path.extension() here and *not* extension
    if (path.extension() == ".rexs" || extension == ".rexs.xml") {
      return TFileType::XML;
    }
    if (path.extension() == ".rexsz" || extension == ".rexs.zip") {
      return TFileType::COMPRESSED;
    }
    if (path.extension() == ".rexsj" || extension == ".rexs.json") {
      return TFileType::JSON;
    }

    return TFileType::UNKOWN;
  }
}

#endif
