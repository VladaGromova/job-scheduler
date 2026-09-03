#pragma once
#include "Logger.hpp"
#include "tasks/Task.hpp"

#include <map>
#include <queue>
#include <string>
#include <thread>

/**
 * @brief A snapshot of task data, safe to read and transmit outside the Scheduler.
 *
 * A copy of selected Task fields, created under mutex protection.
 */
struct TaskInfo {
  std::string id;
  TaskStatus status;
  int priority;
  TaskMode mode;
  std::chrono::system_clock::time_point nextExecutionTime;
  std::chrono::milliseconds cycleInterval;
};

/**
 * @brief Central component managing task queuing and lifecycle.
 *
 * Maintains three structures: a map of all tasks (allTasks, the only one owner
 * of Task objects), a queue of tasks ready to run (readyTasks, sorted by
 * priority), and a queue of tasks waiting for their scheduled time
 * (scheduledTasks, sorted by execution time). A dedicated thread
 * (schedulerLoop) monitors scheduledTasks and moves tasks into
 * readyTasks. Implemented methods are thread-safe.
 */
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

  /**
     * @brief Registers a new task and routes it to the appropriate queue.
     *
     * SCHEDULED tasks whose execution time is already in the past are
     * immediately marked FAILED, without entering any queue.
     */
  void addTask(std::unique_ptr<Task> task);

  /**
     * @brief Blocks the calling thread until a ready task becomes available.
     * @return A pointer to the task to execute, or nullptr on shutdown.
     *
     * Skips tasks that were cancelled in the meantime.
     */
  Task *getReadyTask();
  bool cancelTask(const std::string &id);

  /// @brief Returns a task to the appropriate queue.
  void rescheduleTask(Task *task);
  std::vector<TaskInfo> getAllTasksStatus();
  std::vector<TaskInfo> getTasksByStatus(TaskStatus status);
  std::optional<TaskStatus> getTaskStatus(const std::string &name);

  /// @brief Starts the background thread that monitors the schedule (schedulerLoop).
  void start();

  /// @brief Shuts the system down, wakes all waiting threads, and joins them.
  void stop();
};