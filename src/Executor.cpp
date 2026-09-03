#include "Executor.hpp"

#include <iostream>

Executor::Executor(Scheduler &scheduler, Logger &logger, int numOfThreads)
    : scheduler(scheduler), logger(logger), numOfThreads(numOfThreads) {}

Executor::~Executor() { stop(); }

void Executor::start() {
  running.store(true);
  workers.reserve(numOfThreads);
  for (int i = 0; i < numOfThreads; ++i) {
    workers.emplace_back(&Executor::worker, this, i);
  }
}

void Executor::stop() {
  if (!running.exchange(false)) {
    return;
  }
  for (auto &t : workers) {
    if (t.joinable()) {
      t.join();
    }
  }
  workers.clear();
}

void Executor::worker(int workerId) {
  while (running.load()) {
    Task *task = scheduler.getReadyTask();

    if (task == nullptr) {
      continue;
    }

    task->setExecutorProcessId(workerId);
    task->updateStatus(TaskStatus::RUNNING);
    task->setStartedAt(std::chrono::system_clock::now());

    log(workerId, task->getId(), "started");
    logger.log(LogMessage{std::chrono::system_clock::now(), task->getId(),
                          task->getStatus(), "Task started"});

    bool success = task->execute();
    task->setFinishedAt(std::chrono::system_clock::now());

    if (success) {
      if (task->getMode() == TaskMode::CYCLIC) {
        auto next = std::chrono::system_clock::now() + task->getCycleInterval();
        task->setNextExecutionTime(next);
        task->updateStatus(TaskStatus::SCHEDULED);
        scheduler.rescheduleTask(task);

        log(workerId, task->getId(), "finished, rescheduled (cyclic)");
        logger.log(LogMessage{std::chrono::system_clock::now(), task->getId(),
                              task->getStatus(),
                              "Task finished, rescheduled for next cycle"});
      } else {
        task->updateStatus(TaskStatus::FINISHED);

        log(workerId, task->getId(), "finished");
        logger.log(LogMessage{std::chrono::system_clock::now(), task->getId(),
                              task->getStatus(), "Task finished successfully"});
      }
    } else {
      task->incrementRetries();

      if (task->getCurrentRetries() < task->getMaxRetries()) {
        task->updateStatus(TaskStatus::WAITING);
        scheduler.rescheduleTask(task);

        log(workerId, task->getId(), "failed, retrying");
        logger.log(LogMessage{
            std::chrono::system_clock::now(), task->getId(), task->getStatus(),
            "Task failed, retrying (" +
                std::to_string(task->getCurrentRetries()) + "/" +
                std::to_string(task->getMaxRetries()) + ")"});
      } else {
        task->updateStatus(TaskStatus::FAILED);

        log(workerId, task->getId(), "failed permanently");
        logger.log(LogMessage{std::chrono::system_clock::now(), task->getId(),
                              task->getStatus(),
                              "Task failed permanently, no retries left"});
      }
    }
  }
}

void Executor::log(const int workerId, const std::string taskId,
                   const std::string &message) {
  std::lock_guard<std::mutex> lock(coutMutex);
  std::cout << "[" << workerId << "] [" << taskId << "] " << message << "\n";
}