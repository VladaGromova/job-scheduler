#pragma once
#include "tasks/Task.hpp"

struct TaskPriorityComparator {
  bool operator()(const Task* a, const Task* b) const {
    return a->getPriority() < b->getPriority();
  }
};

struct TaskTimeComparator {
  bool operator()(const Task* a, const Task* b) const {
    return a->getNextExecutionTime() > b->getNextExecutionTime();
  }
};