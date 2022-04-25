
#ifndef REXSCXX_DATABASE_UNIT_HXX
#define REXSCXX_DATABASE_UNIT_HXX

#include <string>

namespace rexsapi::database
{
  class Unit
  {
  public:
    Unit(uint64_t id, std::string name)
    : m_Id{id}
    , m_Name{std::move(name)}
    {
    }

    [[nodiscard]] uint64_t getId() const
    {
      return m_Id;
    }

    [[nodiscard]] const std::string& getName() const
    {
      return m_Name;
    }

  private:
    const uint64_t m_Id;
    const std::string m_Name;
  };
}

#endif
