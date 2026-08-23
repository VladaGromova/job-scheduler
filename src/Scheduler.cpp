#include "Scheduler.hpp"
#include "LogMessage.hpp"

#include <iostream>

Scheduler::Scheduler(Logger& logger) : logger(logger) {}

void Scheduler::addTask(std::unique_ptr<Task>  task) {
  if (task == nullptr) {
    std::cerr << "[Scheduler] addTask called with null task, ignoring.\n";
    return;
  }

  Task* rawPtr = task.get();

  {
    std::lock_guard<std::mutex> lock(allTasksMutex);
    allTasks[rawPtr->getId()] = std::move(task);
  }

  switch (rawPtr->getMode()) {
  case TaskMode::ONCE:
    rawPtr->updateStatus(TaskStatus::WAITING);
    {
    std::lock_guard<std::mutex> lock(readyTasksMutex);
    readyTasks.push(rawPtr);
    }
    readyCv.notify_one();
    break;
  case TaskMode::SCHEDULED:
  case TaskMode::CYCLIC:
    rawPtr->updateStatus(TaskStatus::SCHEDULED);
    {
    std::lock_guard<std::mutex> lock(scheduledTasksMutex);
    scheduledTasks.push(rawPtr);
    }
    scheduledCv.notify_one();
    break;
  }
  logger.log(LogMessage{
    std::chrono::system_clock::now(),
    rawPtr->getId(),
    rawPtr->getStatus(),
    "Task added to scheduler"
});
}

void Scheduler::start() {
  schedulerThread = std::thread(&Scheduler::schedulerLoop, this);
}

void Scheduler::stop() {
  running.store(false);
  scheduledCv.notify_all();
  readyCv.notify_all();
  if (schedulerThread.joinable()) {
    schedulerThread.join();
  }
}

void Scheduler::schedulerLoop() {
  std::unique_lock<std::mutex> lock(scheduledTasksMutex);

  while (running.load()) {
    if (scheduledTasks.empty()) {
      scheduledCv.wait(lock, [this] {
          return !scheduledTasks.empty() || !running.load();
      });
      continue;
    }

    Task* next = scheduledTasks.top();
    auto wakeTime = next->getNextExecutionTime();

    bool notified = scheduledCv.wait_until(lock, wakeTime, [this] {
        return !running.load();
    });

    if (!running.load()) break;

    if (!scheduledTasks.empty() && scheduledTasks.top()->getNextExecutionTime() <= std::chrono::system_clock::now()) {
      Task* ready = scheduledTasks.top();
      scheduledTasks.pop();

      if (ready->getStatus() == TaskStatus::CANCELLED) {
        continue;
      }

      ready->updateStatus(TaskStatus::WAITING);

      {
        std::lock_guard<std::mutex> readyLock(readyTasksMutex);
        readyTasks.push(ready);
      }
      readyCv.notify_one();
    }
  }
}

// std::vector<std::pair<std::string, TaskStatus>> Scheduler::getAllTasksStatus() {
//     std::lock_guard<std::mutex> lock(allTasksMutex);
//     std::vector<std::pair<std::string, TaskStatus>> result;
//     result.reserve(allTasks.size());
//     for (const auto& [id, task] : allTasks) {
//       result.emplace_back(id, task->getStatus());
//     }
//     return result;
//   }
//
// std::vector<std::pair<std::string, TaskStatus>> Scheduler::getTasksByStatus(TaskStatus status) {
//   std::lock_guard<std::mutex> lock(allTasksMutex);
//   std::vector<std::pair<std::string, TaskStatus>> result;
//   result.reserve(allTasks.size());
//   for (const auto& [id, task] : allTasks) {
//     if (task->getStatus() == status) {
//       result.emplace_back(id, status);
//     }
//   }
//   return result;
// }

std::vector<TaskInfo> Scheduler::getAllTasksStatus() {
  std::lock_guard<std::mutex> lock(allTasksMutex);
  std::vector<TaskInfo> result;
  result.reserve(allTasks.size());
  for (const auto& [id, task] : allTasks) {
    result.push_back(TaskInfo{
        task->getId(),
        task->getStatus(),
        task->getPriority(),
        task->getMode(),
        task->getNextExecutionTime(),
        task->getCycleInterval()
    });
  }
  return result;
}

std::vector<TaskInfo> Scheduler::getTasksByStatus(TaskStatus status) {
  std::lock_guard<std::mutex> lock(allTasksMutex);
  std::vector<TaskInfo> result;
  for (const auto& [id, task] : allTasks) {
    if (task->getStatus() == status) {
      result.push_back(TaskInfo{
          task->getId(), task->getStatus(), task->getPriority(),
          task->getMode(), task->getNextExecutionTime(), task->getCycleInterval()
      });
    }
  }
  return result;
}

std::optional<TaskStatus> Scheduler::getTaskStatus(const std::string& name) {
  std::lock_guard<std::mutex> lock(allTasksMutex);
  auto it = allTasks.find(name);
  if (it == allTasks.end()) return std::nullopt;
  return it->second->getStatus();
}

bool Scheduler::cancelTask(const std::string& id) {
  std::lock_guard<std::mutex> lock(allTasksMutex);
  auto it = allTasks.find(id);
  if (it == allTasks.end()) return false;
  it->second->requestCancel();
  it->second->updateStatus(TaskStatus::CANCELLED);
  return true;
}

Task* Scheduler::getReadyTask() {
  std::unique_lock<std::mutex> lock(readyTasksMutex);
  readyCv.wait(lock, [this] {
    return !readyTasks.empty() || !running.load();
  });
  if (!running.load()) {
    return nullptr;
  }

  while (!readyTasks.empty()) {
    Task* task = readyTasks.top();
    readyTasks.pop();

    if (task->getStatus() == TaskStatus::CANCELLED) {
      continue;
    }

    return task;
  }

  return nullptr;
}

void Scheduler::rescheduleTask(Task* task) {
  if (task->getStatus() == TaskStatus::SCHEDULED) {
    {
      std::lock_guard<std::mutex> lock(scheduledTasksMutex);
      scheduledTasks.push(task);
    }
    scheduledCv.notify_one();
  } else if (task->getStatus() == TaskStatus::WAITING) {
    {
      std::lock_guard<std::mutex> lock(readyTasksMutex);
      readyTasks.push(task);
    }
    readyCv.notify_one();
  }
}