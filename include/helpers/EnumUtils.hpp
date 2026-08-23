#include "enums/TaskStatus.hpp"
#include "enums/TaskMode.hpp"

#include <string>

inline std::string taskStatusToString(TaskStatus s) {
  switch (s) {
  case TaskStatus::SCHEDULED: return "SCHEDULED";
  case TaskStatus::WAITING:   return "WAITING";
  case TaskStatus::RUNNING:   return "RUNNING";
  case TaskStatus::FINISHED:  return "FINISHED";
  case TaskStatus::FAILED:    return "FAILED";
  case TaskStatus::CANCELLED: return "CANCELLED";
  }
  return "UNKNOWN";
}

inline std::optional<TaskStatus> stringToTaskStatus(const std::string& s) {
  if (s == "SCHEDULED") return TaskStatus::SCHEDULED;
  if (s == "WAITING")   return TaskStatus::WAITING;
  if (s == "RUNNING")   return TaskStatus::RUNNING;
  if (s == "FINISHED")  return TaskStatus::FINISHED;
  if (s == "FAILED")    return TaskStatus::FAILED;
  if (s == "CANCELLED") return TaskStatus::CANCELLED;
  return std::nullopt;
}

inline std::string taskModeToShortString(TaskMode mode) {
  switch (mode) {
  case TaskMode::ONCE:      return "-";
  case TaskMode::SCHEDULED: return "H";
  case TaskMode::CYCLIC:    return "C";
  }
  return "?";
}