#include <catch2/catch_test_macros.hpp>
#include "helpers/DateTimeUtils.hpp"

TEST_CASE("parseDateTime parses valid date strings", "[DateTimeUtils]") {
  auto result = DateTimeUtils::parseDateTime("2026-08-21_17:57");
  REQUIRE(result.has_value());
}

TEST_CASE("parseDateTime returns nullopt for incorrect input", "[DateTimeUtils]") {
  REQUIRE(DateTimeUtils::parseDateTime("not-a-date") == std::nullopt);
  REQUIRE(DateTimeUtils::parseDateTime("2026-13-99_25:99") == std::nullopt);
  REQUIRE(DateTimeUtils::parseDateTime("") == std::nullopt);
}

TEST_CASE("parseDateTime and formatDateTime converts date correctlly", "[DateTimeUtils]") {
  std::string original = "2026-08-21_17:57";
  auto parsed = DateTimeUtils::parseDateTime(original);
  REQUIRE(parsed.has_value());

  std::string formatted = DateTimeUtils::formatDateTime(*parsed);
  REQUIRE(formatted == original);
}

TEST_CASE("parseDateTime correctly sorting two different times", "[DateTimeUtils]") {
  auto earlier = DateTimeUtils::parseDateTime("2026-01-01_00:00");
  auto later = DateTimeUtils::parseDateTime("2026-06-01_12:00");
  REQUIRE(earlier.has_value());
  REQUIRE(later.has_value());
  REQUIRE(*earlier < *later);
}