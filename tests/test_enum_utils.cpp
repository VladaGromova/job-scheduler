#include "catch2/catch_test_macros.hpp"
#include "helpers/EnumUtils.hpp"

TEST_CASE("taskStatusToString converts all statuses correctly", "[EnumUtils]") {
  REQUIRE(EnumUtils::taskStatusToString(TaskStatus::SCHEDULED) == "SCHEDULED");
  REQUIRE(EnumUtils::taskStatusToString(TaskStatus::WAITING) == "WAITING");
  REQUIRE(EnumUtils::taskStatusToString(TaskStatus::RUNNING) == "RUNNING");
  REQUIRE(EnumUtils::taskStatusToString(TaskStatus::FINISHED) == "FINISHED");
  REQUIRE(EnumUtils::taskStatusToString(TaskStatus::FAILED) == "FAILED");
  REQUIRE(EnumUtils::taskStatusToString(TaskStatus::CANCELLED) == "CANCELLED");
}

TEST_CASE("stringToTaskStatus parses valid strings", "[EnumUtils]") {
  REQUIRE(EnumUtils::stringToTaskStatus("RUNNING") == TaskStatus::RUNNING);
  REQUIRE(EnumUtils::stringToTaskStatus("FAILED") == TaskStatus::FAILED);
}

TEST_CASE("stringToTaskStatus returns nullopt for invalid input", "[EnumUtils]") {
  REQUIRE(EnumUtils::stringToTaskStatus("NOT_A_STATUS") == std::nullopt);
  REQUIRE(EnumUtils::stringToTaskStatus("") == std::nullopt);
  REQUIRE(EnumUtils::stringToTaskStatus("running") == std::nullopt);  // case-sensitive
}

TEST_CASE("taskModeToShortString returns correct tokens", "[EnumUtils]") {
  REQUIRE(EnumUtils::taskModeToShortString(TaskMode::ONCE) == "-");
  REQUIRE(EnumUtils::taskModeToShortString(TaskMode::SCHEDULED) == "H");
  REQUIRE(EnumUtils::taskModeToShortString(TaskMode::CYCLIC) == "C");
}

TEST_CASE("stringToCommand works correctly", "[EnumUtils]") {
  REQUIRE(EnumUtils::stringToCommand("status") == Command::STATUS);
  REQUIRE(EnumUtils::stringToCommand("cancel") == Command::CANCEL);
  REQUIRE(EnumUtils::stringToCommand("q") == Command::QUIT);
  REQUIRE(EnumUtils::stringToCommand("gibberish") == Command::UNKNOWN);
}