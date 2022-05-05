
#include <rexsapi/database/Model.hxx>

#include <doctest.h>


TEST_CASE("Status test")
{
  SUBCASE("Existing status from string")
  {
    CHECK(rexsapi::database::statusFromString("RELEASED") == rexsapi::database::TStatus::RELEASED);
    CHECK(rexsapi::database::statusFromString("IN_DEVELOPMENT") == rexsapi::database::TStatus::IN_DEVELOPMENT);
  }

  SUBCASE("Non existing status from string")
  {
    CHECK_THROWS_WITH(rexsapi::database::statusFromString("PUSCHEL"), "status 'PUSCHEL' unkown");
  }
}

TEST_CASE("Database Model test")
{
  SUBCASE("Create model")
  {
    rexsapi::database::TModel model("1.4", "de", "2022-04-20T14:18:11.344+02:00", rexsapi::database::TStatus::RELEASED);
    CHECK(model.getVersion() == "1.4");
    CHECK(model.getLanguage() == "de");
    CHECK(model.getDate() == "2022-04-20T14:18:11.344+02:00");
    CHECK(model.isReleased());
  }

  SUBCASE("Not released model")
  {
    rexsapi::database::TModel model("1.4", "de", "2022-04-20T14:18:11.344+02:00",
                                    rexsapi::database::TStatus::IN_DEVELOPMENT);
    CHECK_FALSE(model.isReleased());
  }

  SUBCASE("non existing component")
  {
    rexsapi::database::TModel model("1.4", "de", "2022-04-20T14:18:11.344+02:00", rexsapi::database::TStatus::RELEASED);
    CHECK_THROWS_WITH((void)model.findComponentById("hutzli"), "component 'hutzli' not found");
  }
}
