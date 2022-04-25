
#include <rexsapi/database/Interval.hxx>

#include <doctest.h>
#include <limits>

TEST_CASE("Endpoint test")
{
  SUBCASE("Default endpoint")
  {
    rexsapi::database::TIntervalEndpoint endpoint{};
    CHECK_FALSE(endpoint.isSet());
    CHECK(endpoint >= 4711);
    CHECK(endpoint <= 4711);
  }

  SUBCASE("Open endpoint")
  {
    rexsapi::database::TIntervalEndpoint endpoint{42, rexsapi::database::TIntervalType::OPEN};
    CHECK(endpoint.isSet());
    CHECK_FALSE(endpoint <= 42);
    CHECK_FALSE(endpoint <= 0);
    CHECK_FALSE(endpoint <= -42);
    CHECK(endpoint <= 43);
    CHECK_FALSE(endpoint >= 42);
    CHECK(endpoint >= 41);
  }

  SUBCASE("Closed endpoint")
  {
    rexsapi::database::TIntervalEndpoint endpoint{42, rexsapi::database::TIntervalType::CLOSED};
    CHECK(endpoint.isSet());
    CHECK_FALSE(endpoint <= 41);
    CHECK(endpoint <= 42);
    CHECK(endpoint <= 43);
    CHECK_FALSE(endpoint >= 43);
    CHECK(endpoint >= 42);
    CHECK(endpoint >= 41);
  }
}

TEST_CASE("Interval test")
{
  SUBCASE("Default interval")
  {
    rexsapi::database::TInterval interval{};
    CHECK(interval.check(4711));
  }

  SUBCASE("Left open interval")
  {
    rexsapi::database::TInterval interval{rexsapi::database::TIntervalEndpoint{0, rexsapi::database::TIntervalType::OPEN}, {}};
    CHECK(interval.check(1));
    CHECK(interval.check(42));
    CHECK_FALSE(interval.check(0));
    CHECK_FALSE(interval.check(-42));
  }

  SUBCASE("Right open interval")
  {
    rexsapi::database::TInterval interval{{}, rexsapi::database::TIntervalEndpoint{500, rexsapi::database::TIntervalType::OPEN}};
    CHECK(interval.check(499));
    CHECK(interval.check(0));
    CHECK(interval.check(-500));
    CHECK_FALSE(interval.check(500));
    CHECK_FALSE(interval.check(500000));
  }

  SUBCASE("Left and right closed interval")
  {
    rexsapi::database::TInterval interval{rexsapi::database::TIntervalEndpoint{-10, rexsapi::database::TIntervalType::CLOSED},
                                          rexsapi::database::TIntervalEndpoint{10, rexsapi::database::TIntervalType::CLOSED}};
    CHECK(interval.check(-10));
    CHECK(interval.check(-9));
    CHECK(interval.check(0));
    CHECK(interval.check(9));
    CHECK(interval.check(10));

    CHECK_FALSE(interval.check(-11));
    CHECK_FALSE(interval.check(11));
    CHECK_FALSE(interval.check(-500));
    CHECK_FALSE(interval.check(500));
  }
}
