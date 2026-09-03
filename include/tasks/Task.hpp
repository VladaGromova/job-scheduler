#pragma once

#include "../enums/TaskMode.hpp"
#include "../enums/TaskStatus.hpp"
#include <string>

/**
 * @brief Representation of a task in Job Scheduler system.
 *
 * Holds the common state (priority, status, schedule, retry count) shared
 * by all task types. Concrete execution logic is delegated to derived
 * classes via execute(). Task manages its metadata.
 */
class Task {
protected:
  std::string id;
  std::string fileName;
  int priority;
  int maxRetries{0};
  int currentRetries{0};
  TaskStatus status;
  TaskMode mode{ONCE};
  std::chrono::time_point<std::chrono::system_clock> requestedStartTime;
  std::chrono::milliseconds cycleInterval;
  std::chrono::time_point<std::chrono::system_clock> createdAt;
  std::chrono::time_point<std::chrono::system_clock> startedAt;
  std::chrono::time_point<std::chrono::system_clock> finishedAt;
  std::chrono::time_point<std::chrono::system_clock> cancelledAt;
  std::chrono::time_point<std::chrono::system_clock> nextExecutionTime;
  std::atomic<int> executorProcessId{-1};
  std::atomic<bool> cancelRequested{false};

public:
  /**
    * @brief Constructs a task with the given parameters.
    * @param requestedStart Required for SCHEDULED/CYCLIC mode.
    * @param cycle Required for CYCLIC mode.
    * @throws std::invalid_argument if the mode requires a time parameter that was not provided.
    */
  Task(std::string id, std::string fileName, int priority, int maxRetries,
       TaskMode mode,
       std::optional<std::chrono::time_point<std::chrono::system_clock>>
           requestedStart,
       std::optional<std::chrono::milliseconds> cycle);
  virtual ~Task() = default;
  Task(const Task &) = delete;
  Task &operator=(const Task &) = delete;

  /**
     * @brief Simulates task execution.
     * @return true on success, false on failure
     */
  virtual bool execute() = 0;

  /// @brief Returns a human-readable task type name (for logging and display).
  virtual std::string taskTypeName() const = 0;
  void requestCancel() { cancelRequested.store(true); }
  void updateStatus(TaskStatus newStatus) { status = newStatus; }
  bool isCancelRequested() const { return cancelRequested.load(); }
  const std::string &getId() const { return id; }
  TaskMode getMode() const { return mode; }
  int getPriority() const { return priority; }
  int getMaxRetries() const { return maxRetries; }
  int getCurrentRetries() const { return currentRetries; }
  TaskStatus getStatus() const { return status; }
  const std::string &getFileName() const { return fileName; }
  std::chrono::milliseconds getCycleInterval() const { return cycleInterval; }
  std::chrono::time_point<std::chrono::system_clock> getCreatedAt() const {
    return createdAt;
  }
  std::chrono::time_point<std::chrono::system_clock> getStartedAt() const {
    return startedAt;
  }
  std::chrono::time_point<std::chrono::system_clock> getFinishedAt() const {
    return finishedAt;
  }
  std::chrono::time_point<std::chrono::system_clock>
  getNextExecutionTime() const {
    return nextExecutionTime;
  }
  void incrementRetries() { currentRetries++; }
  void
  setNextExecutionTime(std::chrono::time_point<std::chrono::system_clock> t) {
    nextExecutionTime = t;
  }
  void setStartedAt(std::chrono::time_point<std::chrono::system_clock> t) {
    startedAt = t;
  }
  void setFinishedAt(std::chrono::time_point<std::chrono::system_clock> t) {
    finishedAt = t;
  }
  void setCancelledAt(std::chrono::time_point<std::chrono::system_clock> t) {
    cancelledAt = t;
  }
  void setExecutorProcessId(int id) { executorProcessId.store(id); }
  int getExecutorProcessId() const { return executorProcessId.load(); }

  struct TaskPriorityComparator {
    bool operator()(const Task *a, const Task *b) const {
      return a->getPriority() < b->getPriority();
    }
  };

  struct TaskTimeComparator {
    bool operator()(const Task *a, const Task *b) const {
      return a->getNextExecutionTime() > b->getNextExecutionTime();
    }
  };
};
