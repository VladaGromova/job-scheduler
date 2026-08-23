#include "tasks/FileTask.hpp"

#include <filesystem>

bool FileTask::execute() {
  if (!std::filesystem::exists(fileName)) {
    return false;
  }
  return true;
}

std::string FileTask::taskTypeName() const {
  return "FileTask";
}