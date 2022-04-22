
#ifndef REXSCXX_CONVERSION_HELPER_HXX
#define REXSCXX_CONVERSION_HELPER_HXX

#include <rexsapi/Exception.hxx>


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
        throw Exception{"cannot convert string '" + s + "' to unsigned integer: invalid argument"};
      } catch (const std::out_of_range&) {
        throw Exception{"cannot convert string '" + s + "' to unsigned integer: out of range"};
      }
    }

    throw Exception{"cannot convert string to unsigned integer: " + s};
  }
}

#endif
