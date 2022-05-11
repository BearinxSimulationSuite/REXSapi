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

#ifndef REXSAPI_RESOURCE_LOADER_HXX
#define REXSAPI_RESOURCE_LOADER_HXX

#include <rexsapi/Defines.hxx>

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace rexsapi
{
  struct TResourceError {
    explicit TResourceError(std::string message, ssize_t position = -1)
    : m_Message{std::move(message)}
    , m_Position{position}
    {
    }

    std::string m_Message;
    ssize_t m_Position;
  };

  class TLoaderResult
  {
  public:
    void addError(TResourceError error)
    {
      m_Errors.emplace_back(std::move(error));
    }

    explicit operator bool() const
    {
      return m_Errors.empty();
    }

    const std::vector<TResourceError>& getErrors() const
    {
      return m_Errors;
    }

  private:
    std::vector<TResourceError> m_Errors;
  };
}

#endif
