
#ifndef REXSCXX_DATABASE_INTERVAL_HXX
#define REXSCXX_DATABASE_INTERVAL_HXX

namespace rexsapi::database
{
  enum class TIntervalType { OPEN, CLOSED };

  class TValue
  {
  public:
    TValue() = default;

    TValue(double value, TIntervalType type)
    : m_Set{true}
    , m_Type{type}
    , m_Value{value}
    {
    }

    [[nodiscard]] bool isSet() const
    {
      return m_Set;
    }

    bool operator<=(double value) const
    {
      if (!m_Set) {
        return true;
      }

      switch (m_Type) {
        case TIntervalType::OPEN:
          return m_Value < value;
        case TIntervalType::CLOSED:
          return m_Value <= value;
      }

      return false;
    }

    bool operator>=(double value) const
    {
      if (!m_Set) {
        return true;
      }

      switch (m_Type) {
        case TIntervalType::OPEN:
          return m_Value > value;
        case TIntervalType::CLOSED:
          return m_Value >= value;
      }

      return false;
    }

  private:
    const bool m_Set{false};
    const TIntervalType m_Type{TIntervalType::OPEN};
    const double m_Value{0.0};
  };

  class TInterval
  {
  public:
    TInterval() = default;

    TInterval(TValue min, TValue max)
    : m_Min{min}
    , m_Max{max}
    {
    }

    [[nodiscard]] bool check(double value) const
    {
      return m_Max >= value && m_Min <= value;
    }

  private:
    const TValue m_Min{};
    const TValue m_Max{};
  };
}

#endif
