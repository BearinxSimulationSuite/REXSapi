
#ifndef REXSAPI_VALUE_HXX
#define REXSAPI_VALUE_HXX

#include <rexsapi/database/EnumValues.hxx>

#include <variant>

namespace rexsapi
{
  using Variant =
    std::variant<std::monostate, double, bool, int64_t, std::string, std::vector<double>, std::vector<bool>,
                 std::vector<int64_t>, std::vector<std::string>, std::vector<std::vector<int64_t>>>;

  class TValue
  {
  public:
    TValue() = default;

    template<typename T>
    explicit TValue(T&& val)
    : m_Value{std::forward<T>(val)}
    {
    }

    bool isEmpty() const
    {
      return m_Value.index() == 0;
    }

    template<typename T>
    const T& getValue() const
    {
      return std::get<T>(m_Value);
    }

    template<typename T>
    const T& getValue(const T& def) const
    {
      if (m_Value.index() == 0) {
        return def;
      }
      return std::get<T>(m_Value);
    }

  private:
    Variant m_Value;
  };
}

#endif
