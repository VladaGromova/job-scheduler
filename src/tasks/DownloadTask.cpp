#include "tasks/DownloadTask.hpp"

#include <thread>

bool DownloadTask::execute() {
  for (int chunk = 0; chunk < 5; ++chunk) {
    if (isCancelRequested()) return false;
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
  }
  return true;
}

std::string DownloadTask::taskTypeName() const { return "DownloadTask"; }