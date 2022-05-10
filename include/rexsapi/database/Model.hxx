
#ifndef REXSAPI_DATABASE_MODEL_HXX
#define REXSAPI_DATABASE_MODEL_HXX

#include <rexsapi/Exception.hxx>
#include <rexsapi/Format.hxx>
#include <rexsapi/RexsVersion.hxx>
#include <rexsapi/Types.hxx>
#include <rexsapi/database/Attribute.hxx>
#include <rexsapi/database/Component.hxx>
#include <rexsapi/database/Unit.hxx>

#include <unordered_map>

namespace rexsapi::database
{
  enum class TStatus { RELEASED, IN_DEVELOPMENT };

  inline static TStatus statusFromString(const std::string& status)
  {
    if (status == "RELEASED") {
      return TStatus::RELEASED;
    }
    if (status == "IN_DEVELOPMENT") {
      return TStatus::IN_DEVELOPMENT;
    }
    throw TException{fmt::format("status '{}' unkown", status)};
  }

  class TModel
  {
  public:
    TModel(TRexsVersion version, std::string language, std::string date, TStatus status)
    : m_Version{version}
    , m_Language{std::move(language)}
    , m_Date{std::move(date)}
    , m_Status{status}
    {
    }

    ~TModel() = default;

    TModel(const TModel&) = delete;
    TModel(TModel&&) = default;
    TModel& operator=(const TModel&) = delete;
    TModel& operator=(TModel&&) = default;

    [[nodiscard]] const TRexsVersion& getVersion() const
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

    [[nodiscard]] bool isReleased() const
    {
      return m_Status == TStatus::RELEASED;
    }

    bool addUnit(TUnit&& unit);

    [[nodiscard]] const TUnit& findUnitById(uint64_t id) const;

    [[nodiscard]] const TUnit& findUnitByName(const std::string& name) const;

    bool addType(uint64_t id, TValueType type);

    [[nodiscard]] const TValueType& findValueTypeById(uint64_t id) const;

    bool addAttribute(TAttribute&& attribute);

    [[nodiscard]] const TAttribute& findAttributetById(const std::string& id) const;

    bool addComponent(TComponent&& component);

    [[nodiscard]] const TComponent& findComponentById(const std::string& id) const;

  private:
    TRexsVersion m_Version;
    std::string m_Language;
    std::string m_Date;
    TStatus m_Status;
    std::unordered_map<uint64_t, TUnit> m_Units;
    std::unordered_map<uint64_t, TValueType> m_Types;
    std::unordered_map<std::string, TAttribute> m_Attributes;
    std::unordered_map<std::string, TComponent> m_Components;
  };


  /////////////////////////////////////////////////////////////////////////////
  // Implementation
  /////////////////////////////////////////////////////////////////////////////

  inline bool TModel::addUnit(TUnit&& unit)
  {
    auto [_, added] = m_Units.try_emplace(unit.getId(), std::move(unit));
    return added;
  }

  inline const TUnit& TModel::findUnitById(uint64_t id) const
  {
    auto it = m_Units.find(id);
    if (it == m_Units.end()) {
      throw TException{fmt::format("unit with id '{}' not found", std::to_string(id))};
    }

    return it->second;
  }

  inline const TUnit& TModel::findUnitByName(const std::string& name) const
  {
    auto it = std::find_if(m_Units.begin(), m_Units.end(), [&name](const auto& item) {
      const auto& [_, unit] = item;
      return unit.getName() == name;
    });
    if (it == m_Units.end()) {
      throw TException{fmt::format("unit '{}' not found", name)};
    }

    return it->second;
  }

  inline bool TModel::addType(uint64_t id, TValueType type)
  {
    auto [_, added] = m_Types.try_emplace(id, type);
    return added;
  }

  inline const TValueType& TModel::findValueTypeById(uint64_t id) const
  {
    auto it = m_Types.find(id);
    if (it == m_Types.end()) {
      throw TException{fmt::format("value type '{}' not found", std::to_string(id))};
    }

    return it->second;
  }

  inline bool TModel::addAttribute(TAttribute&& attribute)
  {
    auto [_, added] = m_Attributes.try_emplace(attribute.getAttributeId(), std::move(attribute));
    return added;
  }

  inline const TAttribute& TModel::findAttributetById(const std::string& id) const
  {
    auto it = m_Attributes.find(id);
    if (it == m_Attributes.end()) {
      throw TException{fmt::format("attribute '{}' not found", id)};
    }

    return it->second;
  }

  inline bool TModel::addComponent(TComponent&& component)
  {
    auto [_, added] = m_Components.try_emplace(component.getId(), std::move(component));
    return added;
  }

  inline const TComponent& TModel::findComponentById(const std::string& id) const
  {
    auto it = m_Components.find(id);
    if (it == m_Components.end()) {
      throw TException{fmt::format("component '{}' not found", id)};
    }

    return it->second;
  }

}

#endif
