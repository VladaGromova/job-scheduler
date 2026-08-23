#pragma once
#include "Task.hpp"

class DownloadTask : public Task {
public:
  using Task::Task;

  bool execute() override;
  std::string taskTypeName() const override;
};