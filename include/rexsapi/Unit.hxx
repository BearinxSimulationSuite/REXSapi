
#ifndef REXSAPI_UNIT_HXX
#define REXSAPI_UNIT_HXX

#include <rexsapi/database/Unit.hxx>

namespace rexsapi
{
  class TUnit
  {
  public:
    explicit TUnit(const database::TUnit& unit)
    : m_Unit{&unit}
    {
    }

    explicit TUnit(std::string unit)
    : m_CustomUnit{std::move(unit)}
    {
    }

    [[nodiscard]] bool isCustomUnit() const
    {
      return m_Unit == nullptr;
    }

    [[nodiscard]] const std::string& getUnit() const
    {
      return m_Unit != nullptr ? m_Unit->getName() : m_CustomUnit;
    }

  private:
    const database::TUnit* m_Unit{nullptr};
    std::string m_CustomUnit;
  };
}

#endif
