
#ifndef REXSAPI_VALUE_HXX
#define REXSAPI_VALUE_HXX

#include <rexsapi/database/EnumValues.hxx>

#include <variant>

namespace rexsapi
{
  template<typename T>
  struct Matrix {
    bool validate() const
    {
      if (m_Values.size()) {
        size_t n = m_Values[0].size();
        for (const auto& row : m_Values) {
          if (row.size() != n) {
            return false;
          }
        }
      }
      return true;
    }

    std::vector<std::vector<T>> m_Values;
  };


  class TValue
  {
  public:
    TValue() = default;
    ~TValue() = default;

    template<typename T>
    explicit TValue(T&& val)
    : m_Value(std::forward<T>(val))
    {
    }

    explicit TValue(const char* val)
    : m_Value(std::string(val))
    {
    }

    explicit TValue(int val)
    : m_Value(static_cast<int64_t>(val))
    {
    }

    TValue(const TValue&) = default;
    TValue(TValue&&) = default;
    TValue& operator=(const TValue&) = delete;
    TValue& operator=(TValue&&) = delete;

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
    using Variant =
      std::variant<std::monostate, double, bool, int64_t, std::string, std::vector<double>, std::vector<bool>,
                   std::vector<int64_t>, std::vector<std::string>, std::vector<std::vector<int64_t>>, Matrix<double>>;

    Variant m_Value;
  };
}

#endif
