#pragma once
#include "Task.hpp"

/// @brief A data downloading task
class DownloadTask : public Task {
public:
  using Task::Task;

  bool execute() override;
  std::string taskTypeName() const override;
};