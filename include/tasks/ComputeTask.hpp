#pragma once
#include "Task.hpp"

/// @brief A computational task
class ComputeTask : public Task {
public:
  using Task::Task;

  bool execute() override;
  std::string taskTypeName() const override;
};