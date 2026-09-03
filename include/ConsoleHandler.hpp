#pragma once
#include "Scheduler.hpp"
#include "TaskFactory.hpp"

/**
 * @brief Handles user interaction through the terminal.
 *
 * Responsible for loading the initial set of tasks (from a file or
 * entered manually), then running the command loop (status/cancel/report/
 * task/q) concurrently with the Scheduler and Executor threads.
 */
class ConsoleHandler {
private:
  bool isRunning = false;
  Scheduler &scheduler;
  TaskFactory &taskFactory;
  Logger &logger;
  bool getTasks();
  void processCommand(const std::string &command);
  bool loadTasksFromFile();
  bool parseInput();
  [[nodiscard]] bool addTaskFromLine(const std::string &line) const;
  void printAllStatuses() const;
  void printStatusesByStatus(const std::string &statusStr) const;
  void printStatusForTask(const std::string &taskName) const;
  int addTasksFromStream(std::istream &input);
  void printStatusTable(const std::vector<TaskInfo> &tasks,
                        const std::string &message) const;

public:
  ConsoleHandler(Scheduler &sched, TaskFactory &factory, Logger &logger);

  /// @brief Runs console interface (task loading, command loop)
  void start();
};