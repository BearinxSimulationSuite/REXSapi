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

#ifndef REXSAPI_CONVERSION_HELPER_HXX
#define REXSAPI_CONVERSION_HELPER_HXX

#include <rexsapi/Exception.hxx>
#include <rexsapi/Format.hxx>

namespace rexsapi
{
  static inline uint64_t convertToUint64(const std::string& s)
  {
    if (s.find_first_of('-') == std::string::npos) {
      try {
        size_t pos = 0;
        auto val = std::stoull(s, &pos);
        if (pos == s.length()) {
          return val;
        }
      } catch (const std::invalid_argument&) {
        throw TException{fmt::format("cannot convert string '{}' to unsigned integer: invalid argument", s)};
      } catch (const std::out_of_range&) {
        throw TException{fmt::format("cannot convert string '{}' to unsigned integer: out of range", s)};
      }
    }

    throw TException{fmt::format("cannot convert string to unsigned integer: {}", s)};
  }


  static inline int64_t convertToInt64(const std::string& s)
  {
    try {
      size_t pos = 0;
      auto val = std::stoll(s, &pos);
      if (pos == s.length()) {
        return val;
      }
    } catch (const std::invalid_argument&) {
      throw TException{fmt::format("cannot convert string '{}' to integer: invalid argument", s)};
    } catch (const std::out_of_range&) {
      throw TException{fmt::format("cannot convert string '{}' to integer: out of range", s)};
    }

    throw TException{fmt::format("cannot convert string to integer: {}", s)};
  }


  static inline double convertToDouble(const std::string& s)
  {
    try {
      std::size_t pos = 0;
      auto val = std::stod(s, &pos);
      if (pos == s.length()) {
        return val;
      }
    } catch (const std::invalid_argument&) {
      throw TException{fmt::format("cannot convert string '{}' to double: invalid argument", s)};
    } catch (const std::out_of_range&) {
      throw TException{fmt::format("cannot convert string '{}' to double: out of range", s)};
    }

    throw TException{fmt::format("cannot convert string to double: {}", s)};
  }
}

#endif
