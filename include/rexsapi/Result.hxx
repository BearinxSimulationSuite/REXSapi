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
#include <rexsapi/Exception.hxx>
#include <rexsapi/Format.hxx>

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace rexsapi
{
  enum class TErrorLevel { WARN, ERR, CRIT };

  static inline std::string toErrorLevelString(TErrorLevel level)
  {
    switch (level) {
      case TErrorLevel::WARN:
        return "WARNING";
      case TErrorLevel::ERR:
        return "ERROR";
      case TErrorLevel::CRIT:
        return "CRITICAL";
    }
    throw TException{"unknown error level"};
  }


  class TError
  {
  public:
    explicit TError(TErrorLevel level, std::string message, ssize_t position = -1)
    : m_Level{level}
    , m_Message{std::move(message)}
    , m_Position{position}
    {
    }

    bool isError() const
    {
      return m_Level == TErrorLevel::ERR || m_Level == TErrorLevel::CRIT;
    }

    bool isCritical() const
    {
      return m_Level == TErrorLevel::CRIT;
    }

    std::string message() const
    {
      if (m_Position != -1) {
        return fmt::format("{}: offset {}", m_Message, m_Position);
      }
      return fmt::format("{}", m_Message);
    }

  private:
    TErrorLevel m_Level;
    std::string m_Message;
    ssize_t m_Position;
  };


  class TResult
  {
  public:
    void addError(TError error)
    {
      m_Errors.emplace_back(std::move(error));
    }

    explicit operator bool() const
    {
      return m_Errors.empty() || std::find_if(m_Errors.begin(), m_Errors.end(), [](const auto& error) {
                                   return error.isError();
                                 }) == m_Errors.end();
    }

    bool isCritical() const
    {
      return !m_Errors.empty() && std::find_if(m_Errors.begin(), m_Errors.end(), [](const auto& error) {
                                   return error.isCritical();
                                 }) != m_Errors.end();
    }

    const std::vector<TError>& getErrors() const&
    {
      return m_Errors;
    }

  private:
    std::vector<TError> m_Errors;
  };
}

#endif
