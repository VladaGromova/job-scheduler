#pragma once
#include "Scheduler.hpp"
#include "TaskFactory.hpp"

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
  void start();
};