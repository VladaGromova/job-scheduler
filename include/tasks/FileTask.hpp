#pragma once
#include "Task.hpp"

class FileTask : public Task {
public:
  using Task::Task;

  bool execute() override;
  std::string taskTypeName() const override;
};