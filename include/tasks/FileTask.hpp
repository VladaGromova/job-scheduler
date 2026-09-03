#pragma once
#include "Task.hpp"

/// @brief A file-operation task
class FileTask : public Task {
public:
  using Task::Task;

  bool execute() override;
  std::string taskTypeName() const override;
};