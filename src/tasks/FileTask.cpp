#include "tasks/FileTask.hpp"

#include <filesystem>

bool FileTask::execute() {
  if (isCancelRequested()) return false;

  return (std::rand() % 100) < 70;
}

std::string FileTask::taskTypeName() const { return "FileTask"; }