#include "tasks/ComputeTask.hpp"

#include <thread>

bool ComputeTask::execute() {
  for (int i = 0; i < 10; ++i) {
    if (isCancelRequested()) return false;
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
  }
  return true;
}

std::string ComputeTask::taskTypeName() const { return "ComputeTask"; }