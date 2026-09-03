#pragma once
#include "Scheduler.hpp"
#include "tasks/Task.hpp"

#include <thread>

/**
 * @brief A class with thread pool executing tasks pulled from the Scheduler.
 *
 * Each thread loop: fetch a ready task (Scheduler::getReadyTask), run it,
 * then update its status, retry it or pass it back to the Scheduler.
 */
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

  /// @brief Starts the pool of worker threads, sized numOfThreads.
  void start();

  /// @brief Signals shutdown and waits for all worker threads to join.
  void stop();
};