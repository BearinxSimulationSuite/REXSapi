
#ifndef REXSCXX_DATABASE_MODEL_HXX
#define REXSCXX_DATABASE_MODEL_HXX

#include <rexsapi/Exception.hxx>
#include <rexsapi/Format.h>
#include <rexsapi/database/Attribute.hxx>
#include <rexsapi/database/Component.hxx>
#include <rexsapi/database/Unit.hxx>
#include <rexsapi/database/ValueType.hxx>

#include <unordered_map>

namespace rexsapi::database
{
  enum class TStatus { RELEASED };

  inline static TStatus statusFromString(const std::string& status)
  {
    if (status == "RELEASED") {
      return TStatus::RELEASED;
    }
    throw Exception{fmt::format("status '{}' unkown", status)};
  }

  class TModel
  {
  public:
    TModel(std::string version, std::string language, std::string date, TStatus status)
    : m_Version{std::move(version)}
    , m_Language{std::move(language)}
    , m_Date{std::move(date)}
    , m_Status{status}
    {
    }

    ~TModel() = default;

    TModel(const TModel&) = delete;
    TModel(TModel&&) = default;
    TModel& operator=(const TModel&) = delete;
    TModel& operator=(TModel&&) = delete;

    [[nodiscard]] const std::string& getVersion() const
    {
      return m_Version;
    }

    [[nodiscard]] const std::string& getLanguage() const
    {
      return m_Language;
    }

    [[nodiscard]] const std::string& getDate() const
    {
      return m_Date;
    }

    [[nodiscard]] TStatus getStatus() const
    {
      return m_Status;
    }

    bool addUnit(Unit&& unit)
    {
      auto [_, added] = m_Units.try_emplace(unit.getId(), std::move(unit));
      return added;
    }

    [[nodiscard]] const Unit& findUnitById(uint64_t id) const
    {
      auto it = m_Units.find(id);
      if (it == m_Units.end()) {
        throw Exception{fmt::format("unit '{}' not found", std::to_string(id))};
      }

      return it->second;
    }

    bool addType(uint64_t id, TValueType type)
    {
      auto [_, added] = m_Types.try_emplace(id, type);
      return added;
    }

    [[nodiscard]] const TValueType& findValueTypeById(uint64_t id) const
    {
      auto it = m_Types.find(id);
      if (it == m_Types.end()) {
        throw Exception{fmt::format("value type '{}' not found", std::to_string(id))};
      }

      return it->second;
    }

    bool addAttribute(TAttribute&& attribute)
    {
      auto [_, added] = m_Attributes.try_emplace(attribute.getAttributeId(), std::move(attribute));
      return added;
    }

    [[nodiscard]] const TAttribute& findAttributetById(const std::string& id) const
    {
      auto it = m_Attributes.find(id);
      if (it == m_Attributes.end()) {
        throw Exception{fmt::format("attribute '{}' not found", id)};
      }

      return it->second;
    }

    bool addComponent(TComponent&& component)
    {
      auto [_, added] = m_Components.try_emplace(component.getId(), std::move(component));
      return added;
    }

    [[nodiscard]] const TComponent& findComponentById(const std::string& id) const
    {
      auto it = m_Components.find(id);
      if (it == m_Components.end()) {
        throw Exception{fmt::format("component '{}' not found", id)};
      }

      return it->second;
    }

  private:
    const std::string m_Version;
    const std::string m_Language;
    const std::string m_Date;
    const TStatus m_Status;
    std::unordered_map<uint64_t, Unit> m_Units;
    std::unordered_map<uint64_t, TValueType> m_Types;
    std::unordered_map<std::string, TAttribute> m_Attributes;
    std::unordered_map<std::string, TComponent> m_Components;
  };
}

#endif
