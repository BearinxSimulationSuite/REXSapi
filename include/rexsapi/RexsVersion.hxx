
#ifndef REXS_REXS_VERSION_HXX
#define REXS_REXS_VERSION_HXX

#include <rexsapi/Exception.hxx>
#include <rexsapi/Format.hxx>

#include <regex>

namespace rexsapi
{
  class TRexsVersion
  {
  public:
    explicit TRexsVersion(const std::string& version)
    {
      static std::regex regExpr{R"(^(\d+)\.(\d+)$)"};
      std::smatch match;
      if (std::regex_search(version, match, regExpr)) {
        m_Major = static_cast<uint32_t>(std::stoul(match[1]));
        m_Minor = static_cast<uint32_t>(std::stoul(match[2]));
      } else {
        throw TException{fmt::format("not a valid version: '{}'", version)};
      }
    }

    TRexsVersion(uint32_t major, uint32_t minor)
    : m_Major{major}
    , m_Minor{minor}
    {
    }

    friend bool operator==(const TRexsVersion& lhs, const TRexsVersion& rhs)
    {
      return lhs.getMajor() == rhs.getMajor() && lhs.getMinor() == rhs.getMinor();
    }

    friend bool operator!=(const TRexsVersion& lhs, const TRexsVersion& rhs)
    {
      return !(rhs == lhs);
    }

    friend bool operator<(const TRexsVersion& lhs, const TRexsVersion& rhs)
    {
      return lhs.getMajor() < rhs.getMajor() || (lhs.getMajor() == rhs.getMajor() && lhs.getMinor() < rhs.getMinor());
    }

    friend bool operator>(const TRexsVersion& lhs, const TRexsVersion& rhs)
    {
      return lhs.getMajor() > rhs.getMajor() || (lhs.getMajor() == rhs.getMajor() && lhs.getMinor() > rhs.getMinor());
    }

    friend bool operator<=(const TRexsVersion& lhs, const TRexsVersion& rhs)
    {
      return lhs < rhs || lhs == rhs;
    }

    friend bool operator>=(const TRexsVersion& lhs, const TRexsVersion& rhs)
    {
      return lhs > rhs || lhs == rhs;
    }

    uint32_t getMajor() const
    {
      return m_Major;
    }

    uint32_t getMinor() const
    {
      return m_Minor;
    }

    std::string string() const
    {
      return fmt::format("{}.{}", m_Major, m_Minor);
    }

  private:
    uint32_t m_Major;
    uint32_t m_Minor;
  };
}

#endif
