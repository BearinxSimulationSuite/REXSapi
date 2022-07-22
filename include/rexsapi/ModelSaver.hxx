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

#ifndef REXSAPI_MODEL_SAVER_HXX
#define REXSAPI_MODEL_SAVER_HXX

#include <rexsapi/JsonModelSerializer.hxx>
#include <rexsapi/JsonSerializer.hxx>
#include <rexsapi/Result.hxx>
#include <rexsapi/XMLModelSerializer.hxx>
#include <rexsapi/XMLSerializer.hxx>

namespace rexsapi
{
  enum class TSaveType { JSON, XML };


  class TModelSaver
  {
  public:
    void store(TResult& result, const TModel& model, const std::filesystem::path& path, TSaveType type)
    {
      try {
        switch (type) {
          case TSaveType::JSON: {
            rexsapi::JsonFileSerializer fileSerializer{addExtension(path, ".rexsj")};
            rexsapi::JsonModelSerializer modelSerializer;
            modelSerializer.serialize(model, fileSerializer);
            break;
          }
          case TSaveType::XML: {
            rexsapi::XMLFileSerializer xmlSerializer{addExtension(path, ".rexs")};
            rexsapi::XMLModelSerializer modelSerializer;
            modelSerializer.serialize(model, xmlSerializer);
            break;
          }
        }
      } catch (const std::exception& ex) {
        result.addError(TError{TErrorLevel::CRIT, fmt::format("cannot store model to {}: {}", ex.what())});
      }
    }

  private:
    static std::filesystem::path addExtension(std::filesystem::path path, std::string_view extension)
    {
      return path.has_extension() ? path : path.concat(extension);
    }
  };
}

#endif
