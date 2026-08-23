#pragma once
#include "enums/TaskStatus.hpp"

#include <chrono>

struct LogMessage {
  std::chrono::system_clock::time_point timestamp;
  std::string taskId;
  TaskStatus taskStatus;
  std::string message;
};