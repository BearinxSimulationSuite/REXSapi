
#include <rexsapi/Value.hxx>

#include <doctest.h>
#include <vector>

TEST_CASE("Value test")
{
  SUBCASE("empty value")
  {
    rexsapi::TValue val{};
    CHECK(val.isEmpty());
  }

  SUBCASE("boolean true value")
  {
    rexsapi::TValue val{true};
    CHECK_FALSE(val.isEmpty());
    CHECK(val.getValue<bool>());
  }

  SUBCASE("double value")
  {
    rexsapi::TValue val{47.11};
    CHECK_FALSE(val.isEmpty());
    CHECK(val.getValue<double>() == doctest::Approx{47.11});
  }

  SUBCASE("integer value")
  {
    rexsapi::TValue val{4711};
    CHECK_FALSE(val.isEmpty());
    CHECK(val.getValue<int64_t>() == 4711);
  }

  SUBCASE("integer default value")
  {
    rexsapi::TValue val{};
    CHECK(val.isEmpty());
    CHECK(val.getValue<int64_t>(4711) == 4711);
  }

  SUBCASE("char string value")
  {
    rexsapi::TValue val{"My String!"};
    CHECK_FALSE(val.isEmpty());
    CHECK(val.getValue<std::string>() == "My String!");
  }

  SUBCASE("string value")
  {
    std::string s{"My std String!"};
    rexsapi::TValue val{s};
    CHECK_FALSE(val.isEmpty());
    CHECK(val.getValue<std::string>() == "My std String!");
  }

  SUBCASE("vector of integer")
  {
    std::vector<int64_t> aofi{42, 815, 4711};
    rexsapi::TValue val{aofi};
    CHECK_FALSE(val.isEmpty());
    CHECK(val.getValue<std::vector<int64_t>>().size() == 3);
  }
}
