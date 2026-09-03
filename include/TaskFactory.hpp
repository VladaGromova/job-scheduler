#pragma once
#include "tasks/Task.hpp"

class TaskFactory {
private:
  int defaultMaxRetries;
  std::atomic<int> taskId{1};
  std::string generateTaskId(const std::string &fileName);

public:
  explicit TaskFactory(int defaultMaxRetries);
  std::unique_ptr<Task> createTaskFromLine(const std::string &line);
};