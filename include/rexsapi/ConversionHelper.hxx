
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
