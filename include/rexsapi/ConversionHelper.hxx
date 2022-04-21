
#ifndef REXSCXX_CONVERSION_HELPER_HXX
#define REXSCXX_CONVERSION_HELPER_HXX

#include <rexsapi/Exception.hxx>


namespace rexsapi
{
  static inline uint64_t convertToUint64(const std::string& s)
  {
    try {
      size_t pos = 0;
      auto val = std::stoull(s, &pos);
      if (pos != s.length()) {
        throw Exception{"cannot convert string to integer: " + s};
      }

      return val;
    } catch (const std::exception& ex) {
      throw Exception{"cannot convert string '" + s + "' to integer: " + ex.what()};
    }
  }
}

#endif
