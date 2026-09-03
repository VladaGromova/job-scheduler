#pragma once
#include "Logger.hpp"
#include "tasks/Task.hpp"

#include <map>
#include <queue>
#include <string>
#include <thread>

struct TaskInfo {
  std::string id;
  TaskStatus status;
  int priority;
  TaskMode mode;
  std::chrono::system_clock::time_point nextExecutionTime;
  std::chrono::milliseconds cycleInterval;
};

class Scheduler {
private:
  std::map<std::string, std::unique_ptr<Task>> allTasks;
  std::priority_queue<Task *, std::vector<Task *>, Task::TaskPriorityComparator>
      readyTasks;
  std::priority_queue<Task *, std::vector<Task *>, Task::TaskTimeComparator>
      scheduledTasks;
  std::mutex allTasksMutex;
  std::mutex readyTasksMutex;
  std::mutex scheduledTasksMutex;
  std::condition_variable readyCv;
  Logger &logger;
  std::condition_variable scheduledCv;
  std::thread schedulerThread;
  std::atomic<bool> running{true};
  void schedulerLoop();

public:
  explicit Scheduler(Logger &logger);
  void addTask(std::unique_ptr<Task> task);
  Task *getReadyTask();
  bool cancelTask(const std::string &id);
  void rescheduleTask(Task *task);
  std::vector<TaskInfo> getAllTasksStatus();
  std::vector<TaskInfo> getTasksByStatus(TaskStatus status);
  std::optional<TaskStatus> getTaskStatus(const std::string &name);
  void start();
  void stop();
};