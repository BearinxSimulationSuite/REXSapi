
#ifndef REXSAPI_VALIDITY_CHECKER_HXX
#define REXSAPI_VALIDITY_CHECKER_HXX

#include <rexsapi/Attribute.hxx>


namespace rexsapi
{
  class TValidityChecker
  {
  public:
    static bool check(const database::TAttribute& attribute, const TValue& val);
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  static inline bool checkRange(const std::optional<const database::TInterval>& interval, double val)
  {
    if (interval.has_value()) {
      return interval->check(val);
    }
    return true;
  }

  inline bool TValidityChecker::check(const database::TAttribute& attribute, const TValue& val)
  {
    const auto& interval = attribute.getInterval();

    switch (attribute.getValueType()) {
      case TValueType::FLOATING_POINT:
        return checkRange(interval, val.getValue<double>());
      case TValueType::INTEGER:
        return checkRange(interval, static_cast<double>(val.getValue<int64_t>()));
      case TValueType::ENUM:
        return attribute.getEnums()->check(val.getValue<std::string>());
      case TValueType::ENUM_ARRAY: {
        const auto& enums = attribute.getEnums();
        const auto& values = val.getValue<std::vector<std::string>>();
        return std::all_of(values.begin(), values.end(), [&enums](const auto& enum_val) {
          return enums->check(enum_val);
        });
      }
      case TValueType::FLOATING_POINT_ARRAY: {
        const auto& values = val.getValue<std::vector<double>>();
        return std::all_of(values.begin(), values.end(), [&interval](const auto& d) {
          return checkRange(interval, d);
        });
      }
      case TValueType::INTEGER_ARRAY: {
        const auto& values = val.getValue<std::vector<int64_t>>();
        return std::all_of(values.begin(), values.end(), [&interval](const auto& i) {
          return checkRange(interval, static_cast<double>(i));
        });
      }
      case TValueType::ARRAY_OF_INTEGER_ARRAYS: {
        const auto& values = val.getValue<std::vector<std::vector<int64_t>>>();
        return std::all_of(values.begin(), values.end(), [&interval](const auto& v) {
          return std::all_of(v.begin(), v.end(), [&interval](const auto& d) {
            return checkRange(interval, d);
          });
        });
      }
      case TValueType::FLOATING_POINT_MATRIX:
      case TValueType::BOOLEAN:
      case TValueType::STRING:
      case TValueType::FILE_REFERENCE:
      case TValueType::BOOLEAN_ARRAY:
      case TValueType::REFERENCE_COMPONENT:
        return true;
    }

    return true;
  }
}

#endif
