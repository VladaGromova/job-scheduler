#pragma once

#include "enums/Command.hpp"
#include "enums/InputMode.hpp"
#include "enums/TaskMode.hpp"
#include "enums/TaskStatus.hpp"

#include <string>

namespace EnumUtils {
inline std::string taskStatusToString(TaskStatus s) {
  switch (s) {
  case TaskStatus::SCHEDULED:
    return "SCHEDULED";
  case TaskStatus::WAITING:
    return "WAITING";
  case TaskStatus::RUNNING:
    return "RUNNING";
  case TaskStatus::FINISHED:
    return "FINISHED";
  case TaskStatus::FAILED:
    return "FAILED";
  case TaskStatus::CANCELLED:
    return "CANCELLED";
  }
  return "UNKNOWN";
}

inline std::optional<TaskStatus> stringToTaskStatus(const std::string &s) {
  if (s == "SCHEDULED")
    return TaskStatus::SCHEDULED;
  if (s == "WAITING")
    return TaskStatus::WAITING;
  if (s == "RUNNING")
    return TaskStatus::RUNNING;
  if (s == "FINISHED")
    return TaskStatus::FINISHED;
  if (s == "FAILED")
    return TaskStatus::FAILED;
  if (s == "CANCELLED")
    return TaskStatus::CANCELLED;
  return std::nullopt;
}

inline std::string taskModeToShortString(TaskMode mode) {
  switch (mode) {
  case TaskMode::ONCE:
    return "-";
  case TaskMode::SCHEDULED:
    return "H";
  case TaskMode::CYCLIC:
    return "C";
  }
  return "?";
}

inline Command stringToCommand(const std::string &cmd) {
  if (cmd == "q")
    return Command::QUIT;
  if (cmd == "task")
    return Command::TASK;
  if (cmd == "status")
    return Command::STATUS;
  if (cmd == "cancel")
    return Command::CANCEL;
  if (cmd == "report")
    return Command::REPORT;
  return Command::UNKNOWN;
}

inline InputMode stringToInputMode(const std::string& mode) {
  if (mode == "1") return InputMode::FROM_FILE;
  if (mode == "2") return InputMode::MANUAL;
  if (mode == "q") return InputMode::QUIT;
  return InputMode::UNKNOWN;
}
}