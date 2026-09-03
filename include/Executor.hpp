#pragma once
#include "Scheduler.hpp"
#include "tasks/Task.hpp"

#include <thread>

class Executor {
private:
  std::atomic<bool> running{false};
  std::vector<std::thread> workers;
  Scheduler &scheduler;
  Logger &logger;
  int numOfThreads;
  void worker(int workerId);
  std::mutex coutMutex;
  void log(int workerId, std::string taskId,
           const std::string &message);

public:
  Executor(Scheduler &scheduler, Logger &logger, int numOfThreads);
  ~Executor();
  void start();
  void stop();
};